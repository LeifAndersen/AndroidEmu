
#include <stdio.h>
#include <stdlib.h>

//#include <tapwave.h>

#include "defs.h"
#include "cpu.h"
#include "cpuregs.h"
#include "hw.h"
#include "regs.h"
#include "lcd.h"
#include "rtc.h"
#include "mem.h"
#include "sound.h"


#include "save.h"
#include "fileio.h"


/*static*/ int ver;
/*static*/ int sramblock, iramblock, vramblock;
/*static*/ int hramofs, hiofs, palofs, oamofs, wavofs;

struct svar svars[] =
{
	I4("GbSs", &ver),

	I2("PC  ", &PC),
	I2("SP  ", &SP),
	I2("BC  ", &BC),
	I2("DE  ", &DE),
	I2("HL  ", &HL),
	I2("AF  ", &AF),

	I4("IME ", &cpu.ime),
	I4("ima ", &cpu.ima),
	I4("spd ", &cpu.speed),
	I4("halt", &cpu.halt),
	I4("div ", &cpu.div),
	I4("tim ", &cpu.tim),
	I4("lcdc", &cpu.lcdc),
	I4("snd ", &cpu.snd),

	I1("ints", &hw.ilines),
	I1("pad ", &hw.pad),
	I4("cgb ", &hw.cgb),
	I4("gba ", &hw.gba),

	I4("mbcm", &mbc.model),
	I4("romb", &mbc.rombank),
	I4("ramb", &mbc.rambank),
	I4("enab", &mbc.enableram),
	I4("batt", &mbc.batt),

	I4("rtcR", &rtc.sel),
	I4("rtcL", &rtc.latch),
	I4("rtcC", &rtc.carry),
	I4("rtcS", &rtc.stop),
	I4("rtcd", &rtc.d),
	I4("rtch", &rtc.h),
	I4("rtcm", &rtc.m),
	I4("rtcs", &rtc.s),
	I4("rtct", &rtc.t),
	I1("rtR8", &rtc.regs[0]),
	I1("rtR9", &rtc.regs[1]),
	I1("rtRA", &rtc.regs[2]),
	I1("rtRB", &rtc.regs[3]),
	I1("rtRC", &rtc.regs[4]),

	I4("S1on", &snd.ch[0].on),
	I4("S1p ", &snd.ch[0].pos),
	I4("S1c ", &snd.ch[0].cnt),
	I4("S1ec", &snd.ch[0].encnt),
	I4("S1sc", &snd.ch[0].swcnt),
	I4("S1sf", &snd.ch[0].swfreq),

	I4("S2on", &snd.ch[1].on),
	I4("S2p ", &snd.ch[1].pos),
	I4("S2c ", &snd.ch[1].cnt),
	I4("S2ec", &snd.ch[1].encnt),

	I4("S3on", &snd.ch[2].on),
	I4("S3p ", &snd.ch[2].pos),
	I4("S3c ", &snd.ch[2].cnt),

	I4("S4on", &snd.ch[3].on),
	I4("S4p ", &snd.ch[3].pos),
	I4("S4c ", &snd.ch[3].cnt),
	I4("S4ec", &snd.ch[3].encnt),

	I4("hdma", &hw.hdma),

	I4("sram", &sramblock),
	I4("iram", &iramblock),
	I4("vram", &vramblock),
	I4("hi  ", &hiofs),
	I4("pal ", &palofs),
	I4("oam ", &oamofs),
	I4("wav ", &wavofs),

	/* NOSAVE is a special code to prevent the rest of the table
	 * from being saved, used to support old stuff for backwards
	 * compatibility... */
	NOSAVE,

	/* the following are obsolete as of 0x104 */

	I4("hram", &hramofs),

	R(P1), R(SB), R(SC),
	R(DIV), R(TIMA), R(TMA), R(TAC),
	R(IE), R(IF),
	R(LCDC), R(STAT), R(LY), R(LYC),
	R(SCX), R(SCY), R(WX), R(WY),
	R(BGP), R(OBP0), R(OBP1),
	R(DMA),

	R(VBK), R(SVBK), R(KEY1),
	R(BCPS), R(BCPD), R(OCPS), R(OCPD),

	R(NR10), R(NR11), R(NR12), R(NR13), R(NR14),
	R(NR21), R(NR22), R(NR23), R(NR24),
	R(NR30), R(NR31), R(NR32), R(NR33), R(NR34),
	R(NR41), R(NR42), R(NR43), R(NR44),
	R(NR50), R(NR51), R(NR52),

	I1("DMA1", &R_HDMA1),
	I1("DMA2", &R_HDMA2),
	I1("DMA3", &R_HDMA3),
	I1("DMA4", &R_HDMA4),
	I1("DMA5", &R_HDMA5),

	END
};


void loadstate(const char *filename)
{
	int i, j;
	byte *buf;//[4096];
	byte *bufptr;


	//menu_message("Loading");

	int irl = hw.cgb ? 8 : 2;
	int vrl = hw.cgb ? 4 : 2;
	int srl = mbc.ramsize << 1;
	int bufsize = (1 + irl + vrl + srl) * 4096;

	buf=(byte*)malloc(bufsize);
	bufptr = buf;

	int n = load_archive(filename, "GBCOID", buf, bufsize);
	if (n < 0) {
		FILE *file = fopen(filename, "rb");
		if (file != NULL) {
			n = fread(buf, 1, bufsize, file);
			fclose(file);
		}
	}
	if (n != bufsize) {
		free(buf);
		return;
	}

	un32 (*header)[2] = (un32 (*)[2])buf;
	un32 d;

	ver = hramofs = hiofs = palofs = oamofs = wavofs = 0;



	bufptr += 4096;

	for (j = 0; header[j][0]; j++)
	{
		for (i = 0; svars[i].ptr; i++)
		{
			if (header[j][0] != *(un32 *)svars[i].key)
				continue;
			d = LIL(header[j][1]);
			switch (svars[i].len)
			{
			case 1:
				*(byte *)svars[i].ptr = d;
				break;
			case 2:
				*(un16 *)svars[i].ptr = d;
				break;
			case 4:
				*(un32 *)svars[i].ptr = d;
				break;
			}
			break;
		}
	}

	/* obsolete as of version 0x104 */
	if (hramofs) memcpy(ram.hi+128, buf+hramofs, 127);

	if (hiofs) memcpy(ram.hi, buf+hiofs, sizeof ram.hi);
	if (palofs) memcpy(lcd.pal, buf+palofs, sizeof lcd.pal);
	if (oamofs) memcpy(lcd.oam.mem, buf+oamofs, sizeof lcd.oam);

	if (wavofs) memcpy(snd.wave, buf+wavofs, sizeof snd.wave);
	else memcpy(snd.wave, ram.hi+0x30, 16); /* patch data from older files */

	//fseek(f, iramblock<<12, SEEK_SET);
	memcpy(ram.ibank, bufptr, 4096 * irl);
	bufptr += 4096 * irl;

	//fseek(f, vramblock<<12, SEEK_SET);
	memcpy(lcd.vbank, bufptr, 4096 * vrl);
	bufptr += 4096 * vrl;

	//fseek(f, sramblock<<12, SEEK_SET);
	memcpy(ram.sbank, bufptr, 4096 * srl);
	bufptr += 4096 * srl;

	free(buf);
}

void savestate(const char *filename)
{
	int i;
	byte *buf;//[4096];
	byte *bufptr;


	//menu_message("Saving");


	int irl = hw.cgb ? 8 : 2;
	int vrl = hw.cgb ? 4 : 2;
	int srl = mbc.ramsize << 1;
	int bufsize = (1 + irl + vrl + srl) * 4096;

	buf=(byte*)malloc(bufsize);
	bufptr = buf;
	un32 (*header)[2] = (un32 (*)[2])buf;
	un32 d = 0;

	ver = 0x105;
	iramblock = 1;
	vramblock = 1+irl;
	sramblock = 1+irl+vrl;
	wavofs = 4096 - 784;
	hiofs = 4096 - 768;
	palofs = 4096 - 512;
	oamofs = 4096 - 256;


	memset(buf, 0, 4096);

	for (i = 0; svars[i].len > 0; i++)
	{


		header[i][0] = *(un32 *)svars[i].key;

		switch (svars[i].len)
		{
		case 1:
			d = *(byte *)svars[i].ptr;
			break;
		case 2:
			d = *(un16 *)svars[i].ptr;
			break;
		case 4:
			d = *(un32 *)svars[i].ptr;

			break;
		}

		header[i][1] = LIL(d);

	}
	header[i][0] = header[i][1] = 0;


	memcpy(buf+hiofs, ram.hi, sizeof ram.hi);
	memcpy(buf+palofs, lcd.pal, sizeof lcd.pal);
	memcpy(buf+oamofs, lcd.oam.mem, sizeof lcd.oam);
	memcpy(buf+wavofs, snd.wave, sizeof snd.wave);

	bufptr += 4096;

	//fseek(f, iramblock<<12, SEEK_SET);
	memcpy(bufptr, ram.ibank, 4096 * irl);
	bufptr += 4096 * irl;

	//fseek(f, vramblock<<12, SEEK_SET);
	memcpy(bufptr, lcd.vbank, 4096 * vrl);
	bufptr += 4096 * vrl;

	//fseek(f, sramblock<<12, SEEK_SET);
	memcpy(bufptr, ram.sbank, 4096 * srl);
	bufptr += 4096 * srl;

	save_archive(filename, "GBCOID", buf, bufsize);

	free(buf);
}



















