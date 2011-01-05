#ifndef _APP_H_
#define _APP_H_

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <fileio.h>

#include "Cyclone/Cyclone.h"
//#include "./m68k/m68k.h"
#include "DrZ80/DrZ80.h"
#include "DrMD.h"
#include "DrSMS.h"
#include "fm.h"
#include "sn76496.h"
#include "unzip.h"
#include "zip.h"
#include "savestate.h"

#include <linux/errno.h>
#include <dirent.h>
#include <errno.h>

//define emulation modes
#define EMU_MODE_NONE   0
#define EMU_MODE_MD     1
#define EMU_MODE_SMS    2
#define EMU_MODE_GG     3

extern struct DrMD drmd;
extern struct DrSMS drsms;
extern struct DrZ80 drz80;
#ifdef EMU_C68K
extern struct Cyclone cyclone;
#endif

extern unsigned short sample_count_lookup[];
extern int sound_stage_lookup[];
extern int fm_timera_tab[];   /* Precalculated timer A values */
extern int fm_timerb_tab[];   /* Precalculated timer B values */
extern unsigned char work_ram[]; // scratch ram
extern unsigned char sram[];
extern unsigned short vram[];
extern unsigned char zram[]; // Z80 ram
extern unsigned short cram[];
extern unsigned short vsram[];
extern unsigned int pal_lookup[];
extern short soundbuffer[];
extern unsigned char RomData[];
extern int CurrentEmuMode;
extern int laststage;
extern int sound_on;

#ifdef EMU_M68K
int vdp_int_ack_callback(int int_level);
#endif

#define SOUND_RATE		22050
#define FRAME_LIMIT		60

static const unsigned int sound_rate = SOUND_RATE;
static const unsigned int sn_UPDATESTEP = 12920;
static const unsigned int frame_limit = FRAME_LIMIT;
static const unsigned int sound_buffer_size = SOUND_RATE / FRAME_LIMIT;

#endif /* _APP_H_ */





