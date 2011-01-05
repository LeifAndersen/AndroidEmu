

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "unzip.h"



//#include <sys/stat.h>

#include "defs.h"
#include "regs.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "rc.h"


char *strdup();

static int mbc_table[256] =
{
	0, 1, 1, 1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 3, 3, 3, 0, 0, 0, 0, 0, 5, 5, 5, MBC_RUMBLE, MBC_RUMBLE, MBC_RUMBLE, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MBC_HUC3, MBC_HUC1
};

static int rtc_table[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static int batt_table[256] =
{
	0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0,
	1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
	0
};

static int romsize_table[256] =
{
	2, 4, 8, 16, 32, 64, 128, 256, 512,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 128, 128, 128
	/* 0, 0, 72, 80, 96  -- actual values but bad to use these! */
};

static int ramsize_table[256] =
{
	1, 1, 1, 4, 16,
	4 /* FIXME - what value should this be?! */
};


static char *sramfile;
static char rtcfile[512];
static char *saveprefix;

static char *savename;
static char *savedir;

static int saveslot;

static int forcebatt, nobatt;
static int forcedmg=0, gbamode;

static int memfill = 0, memrand = -1; //-1,-1


static void initmem(void *mem, int size)
{
	char *p = mem;
/*	if (memrand >= 0)
	{
		srand(memrand ? memrand : time(0));
		while(size--) *(p++) = rand();
	}
	else*/ if (memfill >= 0)
		memset(p, memfill, size);
}


static byte *loadfile(const char *filename, int *len)
{
	unzFile *fd;
	unz_file_info info;
	byte *buf;
	int rv;

	fd = unzOpen(filename);
	if (fd != NULL) {
		rv = unzGoToFirstFile(fd);
		if (rv != UNZ_OK) {
			unzClose(fd);
			return NULL;
		}
		rv = unzGetCurrentFileInfo(fd, &info, NULL, 0, NULL, 0, NULL, 0);
		if (rv != UNZ_OK) {
			unzClose(fd);
			return NULL;
		}
		rv = unzOpenCurrentFile(fd);
		if (rv != UNZ_OK) {
			unzClose(fd);
			return NULL;
		}

		buf = malloc(info.uncompressed_size);
		rv = unzReadCurrentFile(fd, buf, info.uncompressed_size);
		if (rv != info.uncompressed_size) {
			free(buf);
			unzCloseCurrentFile(fd);
			unzClose(fd);
			return NULL;
		}
		*len = info.uncompressed_size;
		unzCloseCurrentFile(fd);
		unzClose(fd);

	} else {
		FILE *file = fopen(filename, "rb");
		if (file == NULL)
			return NULL;

		fseek(file, 0, SEEK_END);
		int n = ftell(file);
		fseek(file, 0, SEEK_SET);
		buf = malloc(n);
		fread(buf, n, 1, file);
		fclose(file);

		*len = n;
	}
	return buf;
}


static byte *inf_buf;
static int inf_pos, inf_len;

static void inflate_callback(byte b)
{
	if (inf_pos >= inf_len)
	{
		inf_len += 512;
		inf_buf = realloc(inf_buf, inf_len);
		if (!inf_buf) die("out of memory inflating file @ %d bytes\n", inf_pos);
	}
	inf_buf[inf_pos++] = b;
}

static byte *decompress(byte *data, int *len)
{
	unsigned long pos = 0;
	if (data[0] != 0x1f || data[1] != 0x8b)
		return data;
	inf_buf = 0;
	inf_pos = inf_len = 0;
/*	if (unzip(data, &pos, inflate_callback) < 0)
		return data;*/
	*len = inf_pos;
	return inf_buf;
}


extern unsigned long fgbZ_romCrc;

int rom_load(const char *romfile)
{
	byte c, *data, *header;
	int len = 0, rlen;

    header = data = loadfile(romfile, &len);	
	if (data == NULL)
		return 0;

    memcpy(rom.name, header+0x0134, 16);
	if (rom.name[14] & 0x80) rom.name[14] = 0;
	if (rom.name[15] & 0x80) rom.name[15] = 0;
	rom.name[16] = 0;

	c = header[0x0147];
	mbc.type = mbc_table[c];
	mbc.batt = (batt_table[c] && !nobatt) || forcebatt;
	rtc.batt = rtc_table[c];
	mbc.romsize = romsize_table[header[0x0148]];
	mbc.ramsize = ramsize_table[header[0x0149]];

	if (!mbc.romsize) {
		die("unknown ROM size %02X\n", header[0x0148]);
		free(data);
		return 0;
	}
	if (!mbc.ramsize) {
		die("unknown SRAM size %02X\n", header[0x0149]);
		free(data);
		return 0;
	}

	rlen = 16384 * mbc.romsize;
    rom.bank = (void *)data;
	ram.sbank = malloc(8192 * mbc.ramsize);

    initmem(ram.sbank, 8192 * mbc.ramsize);
    initmem(ram.ibank, 4096 * 8);

	mbc.rombank = 1;
	mbc.rambank = 0;

	c = header[0x0143];
	hw.cgb = ((c == 0x80) || (c == 0xc0)) && !forcedmg;
	hw.gba = (hw.cgb && gbamode);

	return 1;
}

extern int sram_load();
extern int sram_save();

extern void state_save(int n);
extern void state_load(int n);


void rtc_save()
{
	FILE *f;
	if (!rtc.batt) return;
	if (!(f = fopen(rtcfile, "wb"))) return;
	rtc_save_internal(f);
	fclose(f);
}

void rtc_load()
{
	FILE *f;
	if (!rtc.batt) return;
	if (!(f = fopen(rtcfile, "r"))) return;
	rtc_load_internal(f);
	fclose(f);
}


void loader_unload()
{
	if (rom.bank) free(rom.bank);
	if (ram.sbank) free(ram.sbank);
	sramfile = saveprefix = 0;
	rom.bank = 0;
	ram.sbank = 0;
	mbc.type = mbc.romsize = mbc.ramsize = mbc.batt = 0;
}

static char *base(char *s)
{
	char *p;
	p = (char*)strrchr(s, '/');
	if (p) return p+1;
	return s;
}

static char *ldup(char *s)
{
	int i;
	char *n, *p;
	p = n = (char*)malloc(strlen(s));
	for (i = 0; s[i]; i++) if (isalnum(s[i])) *(p++) = tolower(s[i]);
	*p = 0;
	return n;
}

static void cleanup()
{
//	sram_save();
	rtc_save();
	/* IDEA - if error, write emergency savestate..? */
}

int loader_init(char *romfile)
{
	char *name, *p;

	sprintf(rtcfile, "%s.rtc", romfile);
	if (!rom_load(romfile))
		return 0;

	//init rtc
	/*if (rtc)*/ {
		struct tm *Tm;
		unsigned long i;
		//february is always 28,....
		unsigned long month_len[12]={31,28,31,30,31,30,31,31,30,31,30,31};

		rtc.last=time(NULL);
		Tm=localtime(&rtc.last);

		rtc.d=(int)Tm->tm_wday;

		for (i=0;i<Tm->tm_mon;i++) {
			rtc.d+=month_len[i];
		}

		rtc.h=(int)Tm->tm_hour;
		rtc.m=(int)Tm->tm_min;
		rtc.s=(int)Tm->tm_sec;
		rtc.t=(int)0;
	}

	rtc_load();
	return 1;
}

rcvar_t loader_exports[] =
{
	RCV_STRING("savedir", &savedir),
	RCV_STRING("savename", &savename),
	RCV_INT("saveslot", &saveslot),
	RCV_BOOL("forcebatt", &forcebatt),
	RCV_BOOL("nobatt", &nobatt),
	RCV_BOOL("forcedmg", &forcedmg),
	RCV_BOOL("gbamode", &gbamode),
	RCV_INT("memfill", &memfill),
	RCV_INT("memrand", &memrand),
	RCV_END
};









