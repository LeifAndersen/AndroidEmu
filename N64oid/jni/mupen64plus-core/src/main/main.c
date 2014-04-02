/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - main.c                                                  *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2008-2009 Richard Goedeken                              *
 *   Copyright (C) 2008 Ebenblues Nmn Okaygo Tillin9                       *
 *   Copyright (C) 2002 Hacktarux                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This is MUPEN64's main entry point. It contains code that is common
 * to both the gui and non-gui versions of mupen64. See
 * gui subdirectories for the gui-specific code.
 * if you want to implement an interface, you should look here
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "api/m64p_types.h"
#include "api/callbacks.h"
#include "api/config.h"
#include "api/debugger.h"

#include "main.h"
#include "ticks.h"
#include "rom.h"
#include "savestates.h"

#include "memory/memory.h"
#include "osal/files.h"
#include "osal/preproc.h"
#include "plugin/plugin.h"
#include "r4300/r4300.h"

#ifdef DBG
#include "debugger/dbg_types.h"
#include "debugger/debugger.h"
#endif

#ifdef WITH_LIRC
#include "lirc.h"
#endif //WITH_LIRC

/** globals **/
m64p_handle g_CoreConfig = NULL;

m64p_frame_callback g_FrameCallback = NULL;

int         g_MemHasBeenBSwapped = 0;   // store byte-swapped flag so we don't swap twice when re-playing game
int         g_EmulatorRunning = 0;      // need separate boolean to tell if emulator is running, since --nogui doesn't use a thread

/** static (local) variables **/
static int   l_CurrentFrame = 0;         // frame counter
static int   l_SpeedFactor = 100;        // percentage of nominal game speed at which emulator is running
static int   l_FrameAdvance = 0;         // variable to check if we pause on next frame

/*********************************************************************************************************
* helper functions
*/
char *get_savespath()
{
    static char path[1024];

    snprintf(path, 1024, "%ssave%c", ConfigGetUserDataPath(), OSAL_DIR_SEPARATOR);
    path[1023] = 0;

    /* make sure the directory exists */
    if (osal_mkdirp(path, 0700) != 0)
        return NULL;

    return path;
}

/*********************************************************************************************************
* timer functions
*/
static float VILimit = 60.0;
static double VILimitMilliseconds = 1000.0/60.0;

static int GetVILimit(void)
{
    switch (ROM_HEADER->Country_code&0xFF)
    {
        // PAL codes
        case 0x44:
        case 0x46:
        case 0x49:
        case 0x50:
        case 0x53:
        case 0x55:
        case 0x58:
        case 0x59:
            return 50;
            break;

        // NTSC codes
        case 0x37:
        case 0x41:
        case 0x45:
        case 0x4a:
            return 60;
            break;

        // Fallback for unknown codes
        default:
            return 60;
    }
}

/*********************************************************************************************************
* global functions, for adjusting the core emulator behavior
*/

void main_set_core_defaults(void)
{
    /* parameters controlling the operation of the core */
#if defined(DYNAREC)
    ConfigSetDefaultInt(g_CoreConfig, "R4300Emulator", 2, "Use Pure Interpreter if 0, Cached Interpreter if 1, or Dynamic Recompiler if 2 or more");
#else
    ConfigSetDefaultInt(g_CoreConfig, "R4300Emulator", 1, "Use Pure Interpreter if 0, Cached Interpreter if 1, or Dynamic Recompiler if 2 or more");
#endif
    ConfigSetDefaultBool(g_CoreConfig, "NoCompiledJump", 0, "Disable compiled jump commands in dynamic recompiler (should be set to False) ");
    ConfigSetDefaultBool(g_CoreConfig, "DisableExtraMem", 0, "Disable 4MB expansion RAM pack. May be necessary for some games");
    ConfigSetDefaultBool(g_CoreConfig, "AutoStateSlotIncrement", 0, "Increment the save state slot after each save operation");
    ConfigSetDefaultInt(g_CoreConfig, "CurrentStateSlot", 0, "Save state slot (0-9) to use when saving/loading the emulator state");
    ConfigSetDefaultString(g_CoreConfig, "SaveStatePath", "", "Path to directory where save states are saved. If this is blank, the default value of ${UserConfigPath}/save will be used");
    ConfigSetDefaultString(g_CoreConfig, "SharedDataPath", "", "Path to a directory to search when looking for shared data files");
}

void main_set_fastforward(int enable)
{
    static int ff_state = 0;
    static int SavedSpeedFactor = 100;

    if (enable && !ff_state)
    {
        ff_state = 1; /* activate fast-forward */
        SavedSpeedFactor = l_SpeedFactor;
        l_SpeedFactor = 250;
        setSpeedFactor(l_SpeedFactor);  /* call to audio plugin */
        StateChanged(M64CORE_SPEED_FACTOR, l_SpeedFactor);
    }
    else if (!enable && ff_state)
    {
        ff_state = 0; /* de-activate fast-forward */
        l_SpeedFactor = SavedSpeedFactor;
        setSpeedFactor(l_SpeedFactor);  // call to audio plugin
        StateChanged(M64CORE_SPEED_FACTOR, l_SpeedFactor);
    }

}

int main_is_paused(void)
{
    return (g_EmulatorRunning && rompause);
}

void main_toggle_pause(void)
{
    if (!g_EmulatorRunning)
        return;

    if (rompause)
    {
        DebugMessage(M64MSG_STATUS, "Emulation continued.");
        StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
    }
    else
    {
        DebugMessage(M64MSG_STATUS, "Emulation paused.");
        StateChanged(M64CORE_EMU_STATE, M64EMU_PAUSED);
    }

    rompause = !rompause;
    l_FrameAdvance = 0;
}

void main_advance_one(void)
{
    l_FrameAdvance = 1;
    rompause = 0;
    StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
}

void main_state_set_slot(int slot)
{
    if (slot < 0 || slot > 9)
    {
        DebugMessage(M64MSG_WARNING, "Invalid savestate slot '%i' in main_state_set_slot().  Using 0", slot);
        slot = 0;
    }

    savestates_select_slot(slot);
}

void main_state_inc_slot(void)
{
    savestates_inc_slot();
}

static unsigned char StopRumble[64] = {0x23, 0x01, 0x03, 0xc0, 0x1b, 0x00, 0x00, 0x00,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0};

void main_state_load(const char *filename)
{

    if (filename != NULL)
        savestates_select_filename(filename);

    if (g_EmulatorRunning) {
        controllerCommand(0, StopRumble);
        controllerCommand(1, StopRumble);
        controllerCommand(2, StopRumble);
        controllerCommand(3, StopRumble);
    }

    savestates_job |= LOADSTATE;
}

void main_state_save(int format_pj64, const char *filename)
{
    if (filename != NULL)
        savestates_select_filename(filename);

    savestates_job |= SAVESTATE;
    if (format_pj64)
        savestates_job |= SAVEPJ64STATE;
}

m64p_error main_set_speed(int percent)
{
    if (percent < 10 || percent > 300)
        return M64ERR_INPUT_ASSERT;

    l_SpeedFactor = percent;
    setSpeedFactor(l_SpeedFactor);  // call to audio plugin
    StateChanged(M64CORE_SPEED_FACTOR, l_SpeedFactor);
    return M64ERR_SUCCESS;
}

m64p_error main_core_state_query(m64p_core_param param, int *rval)
{
    if (rval == NULL)
        return M64ERR_INPUT_ASSERT;

    switch (param)
    {
        case M64CORE_EMU_STATE:
            if (!g_EmulatorRunning)
                *rval = M64EMU_STOPPED;
            else if (rompause)
                *rval = M64EMU_PAUSED;
            else
                *rval = M64EMU_RUNNING;
            break;
        case M64CORE_SAVESTATE_SLOT:
            *rval = savestates_get_slot();
            break;
        case M64CORE_SPEED_FACTOR:
            *rval = l_SpeedFactor;
            break;
        default:
            return M64ERR_INPUT_INVALID;
    }

    return M64ERR_SUCCESS;
}

/*********************************************************************************************************
* global functions, callbacks from the r4300 core or from other plugins
*/

void new_frame(void)
{
    if (g_FrameCallback != NULL)
        (*g_FrameCallback)(l_CurrentFrame);

    /* advance the current frame */
    l_CurrentFrame++;

    if (l_FrameAdvance) {
        rompause = 1;
        l_FrameAdvance = 0;
        StateChanged(M64CORE_EMU_STATE, M64EMU_PAUSED);
    }
}

void new_vi(void)
{
    int Dif;
    unsigned int CurrentFPSTime;
    static unsigned int LastFPSTime = 0;
    static unsigned int CounterTime = 0;
    static unsigned int CalculatedTime ;
    static int VI_Counter = 0;

    playAudio();

    double AdjustedLimit = VILimitMilliseconds * 100.0 / l_SpeedFactor;  // adjust for selected emulator speed
    int time;

    start_section(IDLE_SECTION);
    VI_Counter++;

#ifdef DBG
    if(g_DebuggerActive) DebuggerCallback(DEBUG_UI_VI, 0);
#endif

    if(LastFPSTime == 0)
    {
        LastFPSTime = CounterTime = ticksGetTicks();
        return;
    }
    CurrentFPSTime = ticksGetTicks();
    
    Dif = CurrentFPSTime - LastFPSTime;
    
    if (Dif < AdjustedLimit) 
    {
        CalculatedTime = (unsigned int) (CounterTime + AdjustedLimit * VI_Counter);
        time = (int)(CalculatedTime - CurrentFPSTime);
        if (time > 0)
        {
            usleep(time * 1000);
        }
        CurrentFPSTime = CurrentFPSTime + time;
    }

    if (CurrentFPSTime - CounterTime >= 1000.0 ) 
    {
        CounterTime = ticksGetTicks();
        VI_Counter = 0 ;
    }
    
    LastFPSTime = CurrentFPSTime ;
    end_section(IDLE_SECTION);
}

/*********************************************************************************************************
* emulation thread - runs the core
*/
m64p_error main_run(void)
{
    VILimit = (float) GetVILimit();
    VILimitMilliseconds = (double) 1000.0/VILimit; 

    /* take the r4300 emulator mode from the config file at this point and cache it in a global variable */
    r4300emu = ConfigGetParamInt(g_CoreConfig, "R4300Emulator");

    /* set some other core parameters based on the config file values */
    savestates_set_autoinc_slot(ConfigGetParamBool(g_CoreConfig, "AutoStateSlotIncrement"));
    savestates_select_slot(ConfigGetParamInt(g_CoreConfig, "CurrentStateSlot"));
    no_compiled_jump = ConfigGetParamBool(g_CoreConfig, "NoCompiledJump");

    // initialize memory, and do byte-swapping if it's not been done yet
    if (g_MemHasBeenBSwapped == 0)
    {
        init_memory(1);
        g_MemHasBeenBSwapped = 1;
    }
    else
    {
        init_memory(0);
    }

    // Attach rom to plugins
    if (!romOpen_gfx())
    {
        free_memory(); return M64ERR_PLUGIN_FAIL;
    }
    if (!romOpen_audio())
    {
        romClosed_gfx(); free_memory(); return M64ERR_PLUGIN_FAIL;
    }
    if (!romOpen_input())
    {
        romClosed_audio(); romClosed_gfx(); free_memory(); return M64ERR_PLUGIN_FAIL;
    }

#ifdef WITH_LIRC
    lircStart();
#endif // WITH_LIRC

#ifdef DBG
    if (ConfigGetParamBool(g_CoreConfig, "EnableDebugger"))
        init_debugger();
#endif

    PumpEvents();

    g_EmulatorRunning = 1;
    StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);

    /* call r4300 CPU core and run the game */
    r4300_reset_hard();
    r4300_reset_soft();
    r4300_execute();

#ifdef WITH_LIRC
    lircStop();
#endif // WITH_LIRC

#ifdef DBG
    if (g_DebuggerActive)
        destroy_debugger();
#endif

    romClosed_RSP();
    romClosed_input();
    romClosed_audio();
    romClosed_gfx();
    free_memory();

    // clean up
    g_EmulatorRunning = 0;
    StateChanged(M64CORE_EMU_STATE, M64EMU_STOPPED);

    return M64ERR_SUCCESS;
}

void main_stop(void)
{
    /* note: this operation is asynchronous.  It may be called from a thread other than the
       main emulator thread, and may return before the emulator is completely stopped */
    if (!g_EmulatorRunning)
        return;

    DebugMessage(M64MSG_STATUS, "Stopping emulation.");
    if (rompause)
    {
        rompause = 0;
        StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
    }
    stop = 1;
#ifdef DBG
    if(g_DebuggerActive)
    {
        debugger_step();
    }
#endif        
}

/*********************************************************************************************************
* main function
*/
int main(int argc, char *argv[])
{
    return 1;
}

