#define LOG_TAG "libgbc"
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "defs.h"
#include "regs.h"
#include "lcd.h"
#include "fb.h"
#include "input.h"
#include "rc.h"
#include "pcm.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "sound.h"
#include "save.h"

#define SRAM_SAVE_LOOPS		600

struct fb fb;
char vidram[160*144*2];

int save_sram;
static char sram_filename[512];
static int enable_sram;
static int sram_save_counter;

void vid_preinit(void) {}

void vid_init(void)
{
   fb.w = 160;
   fb.h = 144;
   fb.pelsize = 2;
   fb.pitch = 160*2;
   fb.ptr = (unsigned char *)&vidram[0]; //0x0C7B4000;
   fb.enabled = 1;
   fb.dirty = 1; ///1????
   fb.yuv = 0;

   fb.indexed = 0;
   fb.cc[0].l = 11; fb.cc[0].r = 3;
   fb.cc[1].l = 5; fb.cc[1].r = 2;
   fb.cc[2].l = 0; fb.cc[2].r = 3;
}

void vid_begin(void)
{
}

void vid_close(void) {}

void vid_settitle(void) {}

void vid_setpal(int i, int r, int g, int b)
{
}

rcvar_t vid_exports[] =
{
	RCV_END
};

rcvar_t joy_exports[] =
{
	RCV_END
};

struct pcm pcm;
rcvar_t pcm_exports[] =
{
	RCV_END
};

void pcm_init(int rate)
{
   pcm.hz = rate;
   pcm.stereo = 1;
   pcm.len = 512 * 1;
   pcm.buf = NULL; //malloc(pcm.len);
   pcm.pos = 0;
}

int pcm_submit()
{
	if (!pcm.buf) return 0;
	if (pcm.pos < pcm.len) return 1;

	pcm.pos = 0;
	return 1;
}

void pcm_close(void)
{
}

void doevents(void)
{
}

static int sram_save(void)
{
	FILE* f;
	
    /* If we crash before we ever loaded sram, DO NOT SAVE! */
    if (!mbc.batt || !ram.loaded || !mbc.ramsize)  return -1;
	
	/* Consider sram loaded at this point, even if file doesn't exist */
	ram.loaded = 1;

	
	f = fopen(sram_filename, "wb");

    if(!f) return 0;

    fwrite((void *)&ram.sbank[0],8192*mbc.ramsize,1,f);

    fclose(f);

    return 1;
}

static int sram_load(void)
{
	FILE* f;

    if (!mbc.batt) return 0;

	/* Consider sram loaded at this point, even if file doesn't exist */
	ram.loaded = 1;

	
	f = fopen(sram_filename, "rb");
	
	if(!f) return 1; //not existent

    fread((void *)&ram.sbank[0],8192*mbc.ramsize,1,f);

    fclose(f);

    return 1;
}

static void gbcLoadSRAM()
{
	save_sram = 0;
	if (enable_sram)
		sram_load();
}

static void gbcSaveSRAM()
{
	if (enable_sram && save_sram) {
		sram_save();
		save_sram = 0;
	}
}


int gbcInitialize(int rate)
{
    vid_init();
    pcm_init(rate);

    return 1;
}

void gbcCleanup(void)
{
}

int gbcLoadRom(const char *filename)
{
	char *p;

    if (!loader_init(filename))
		return 0;

	strcpy(sram_filename, filename);
	p = strrchr(sram_filename, '.');
	if (p)
		*p = '\0';
	strcat(sram_filename, ".sav");

	sram_save_counter = SRAM_SAVE_LOOPS;
	gbcLoadSRAM();

    emu_reset();
    return 1;
}

void gbcUnloadRom()
{
	gbcSaveSRAM();
	loader_unload();
}

void gbcReset()
{
    emu_reset();
}

void gbcRunFrame(int render_video)
{
	fb.enabled = render_video;
    emu_doframe();

	if (--sram_save_counter <= 0) {
		sram_save_counter = SRAM_SAVE_LOOPS;
		gbcSaveSRAM();
	}
}

void gbcHandleInput(unsigned int keys)
{
	hw.pad = keys;
    pad_refresh();
}

int gbcSaveState(const char* filename)
{
    savestate(filename);
	return 1;
}

int gbcLoadState(const char* filename)
{
    loadstate(filename);

    vram_dirty();
    pal_dirty();
    sound_dirty();
    mem_updatemap();

    return 1;
}

void gbcEnableSRAM(int enable)
{
	enable_sram = enable;
}

