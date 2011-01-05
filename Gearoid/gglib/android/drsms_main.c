#define LOG_TAG "libgg"
#include <utils/Log.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include "app.h"

#define GAMEPAD_START	0x80

#ifdef EMU_C68K
struct Cyclone cyclone;
#endif
struct DrZ80 drz80;
struct DrSMS drsms;

unsigned char VBuf[320 * 240];
unsigned int VPalette[256];
unsigned int pal_lookup[0x1000];
unsigned char RomData[0x500000];

unsigned char work_ram[0x10000]; // scratch ram
unsigned char zram[0x4000];
unsigned char sram[0x10000]; // sram
unsigned int tile_cache[0x4000>>2];
unsigned short vram[0x8000];
unsigned short vsram[0x40];
unsigned short cram[0x40];
unsigned short sample_count_lookup[400];
short soundbuffer[44100 * 2 / 50];
int CurrentEmuMode=EMU_MODE_NONE;
int laststage;
int sound_on;

static unsigned int last_pad;
static int RomSize; 


static int guessEmuMode(const char *filename)
{
	const char *ext = strrchr(filename, '.');
	if (ext) {
		ext++;
		if (strcasecmp(ext, "sms") == 0)
			return EMU_MODE_SMS;
		if (strcasecmp(ext, "gg") == 0)
			return EMU_MODE_GG;
	}
	return EMU_MODE_NONE;
}

static void DrSMS_DrZ80_Set_Irq(unsigned int irq)
{
    drz80.z80irqvector = 0xFF;
	drz80.Z80_IRQ = irq;
}

static void DrSMS_DrZ80_irq_callback(void)
{
    //drz80.Z80_IRQ=0;  // lower irq when in accepted
}

static void DrSMS_DrZ80_Init()
{
  drz80.z80_write8=DrSMS_Z80_write_8;
  drz80.z80_write16=DrSMS_Z80_write_16;
  drz80.z80_in=DrSMS_Z80_In;
  drz80.z80_out=DrSMS_Z80_Out;
  drz80.z80_read8=DrSMS_Z80_read_8;
  drz80.z80_read16=DrSMS_Z80_read_16;
  drz80.z80_rebasePC=DrSMS_Z80_Rebase_PC;
  drz80.z80_rebaseSP=DrSMS_Z80_Rebase_SP;
  drz80.z80_irq_callback=DrSMS_DrZ80_irq_callback;
}

static void DrSMS_DrZ80_Reset()
{
  drz80.Z80A = 0xFF000000;;
  drz80.Z80F = 1<<1;  // set ZFlag
  drz80.Z80BC = 0xFFFF0000;
  drz80.Z80DE = 0xFFFF0000;
  drz80.Z80HL = 0xFFFF0000;
  drz80.Z80A2 = 0;
  drz80.Z80F2 = 1<<2;  // set ZFlag
  drz80.Z80BC2 = 0;
  drz80.Z80DE2 = 0;
  drz80.Z80HL2 = 0;
  drz80.Z80IX = 0xFFFF0000;
  drz80.Z80IY = 0xFFFF0000;
  drz80.Z80I = 0;
  drz80.Z80IM = 1;
  drz80.Z80_IRQ = 0;
  drz80.Z80IF = 0;
  drz80.Z80PC=DrSMS_Z80_Rebase_PC(0);
  drz80.Z80SP=DrSMS_Z80_Rebase_SP(0xDFFF);
}

static void SMS_update_sound_timing(void)
{
	int i;
	float sample_time;
	
	PSG_sn_UPDATESTEP = sn_UPDATESTEP;
     
	sample_time = (sound_rate/(float)frame_limit)/((float)drsms.lines_per_frame-1.0);
	for(i=0;i<=drsms.lines_per_frame-1;i++)
	{
		sample_count_lookup[i] = sample_time * i;
	}
}

static void SMS_load_pal(void)
{
   unsigned short smspal=0;
   unsigned char pal_decode[4] = {0x00,0x0A,0x15,0x1F};
   unsigned char R,G,B;

   for(smspal=0;smspal<0x100;smspal++)
   {
      R=pal_decode[smspal&3]; 
	  G=pal_decode[(smspal>>2)&3]; 
	  B=pal_decode[(smspal>>4)&3]; 

      pal_lookup[smspal] = (R << 11) | (G << 6) | B;
   }
}

static void GG_load_pal(void)
{
   unsigned short pal=0;
   unsigned char R,G,B;
   for(pal=0;pal<0x1000;pal++)
   {
      R=(pal>>0)&0xF; // 0000 0RRR - 3 bits Red
      G=(pal>>4)&0xF; 
      B=(pal>>8)&0xF;

	  pal_lookup[pal] = (R << 12) | (G << 7) | (B << 1);
   }
}

static void DrSMS_Init()
{
  /*
   Need to add code to init drsms depending on PAL or NTSC - NTSC defaulted for now
  */
  if(RomSize&0xFFF) drsms.cart_rom = (unsigned int)RomData+0x200;
  else drsms.cart_rom = (unsigned int)RomData;
  drsms.zram = (unsigned int)work_ram;
  drsms.sram = (unsigned int)sram;
  drsms.sram1 = (unsigned int)sram;
  drsms.sram2 = (unsigned int)sram+0x4000;
  drsms.vdp_memory = (unsigned int)vram;
  drsms.vdp_cram = (unsigned int)cram;
  drsms.tile_cache = (unsigned int)tile_cache;
  drsms.local_pal = (unsigned int) VPalette;

  drsms.z80_context = (unsigned int)&drz80;
  drsms.z80_run = DrZ80Run;
  drsms.z80_set_irq = DrSMS_DrZ80_Set_Irq;
  drsms.z80_reset = DrSMS_DrZ80_Reset;
  drsms.psg_write = psg_write;

  DrSMS_DrZ80_Init();
}

static void DrSMS_Reset()
{
  // reset DrSMS
  // clear memory
  memset(&work_ram,0,0x10000);
  memset(&sram,0,0x10000);
  memset(&vram,0,0x10000);
  memset(&cram,0,0x80);
  memset(&tile_cache,0,0x4000);
  
  drsms.memory_reg_C=0;  // SRAM page in reg
  drsms.memory_reg_D=0;  // rom bank 0
  drsms.memory_reg_E=1;  // rom bank 1
  drsms.memory_reg_F=2;  // rom bank 2
  
  drsms.vdp_status=0;
  drsms.vdp_line=0;
  drsms.vdp_left=0;
  drsms.vdp_pending=0;
  drsms.vdp_type=0;
  drsms.first_byte=0;
  drsms.vdp_buffer=0;
  drsms.vdp_addr=0;
  drsms.sms_port0=128;
  drsms.sms_port5=1<<2;
  drsms.sms_port63=255;
  drsms.map_addr=0x3E00;
  drsms.sprite_addr=0x3F00;
  drsms.vdp_reg0=0;
  drsms.vdp_reg1=0;
  drsms.vdp_reg2=0xFF;
  drsms.vdp_reg3=0xFF;
  drsms.vdp_reg4=0xFF;
  drsms.vdp_reg5=0xFF;
  drsms.vdp_reg6=0xFF;
  drsms.vdp_reg7=0xFF;
  drsms.vdp_reg8=0xFF;
  drsms.vdp_reg9=0xFF;
  drsms.vdp_regA=0xFF;
  drsms.vdp_regB=0xFF;
  drsms.vdp_regC=0xFF;
  drsms.vdp_regD=0xFF;
  drsms.vdp_regE=0xFF;
  drsms.vdp_regF=0xFF;
  
  drsms.max_rom_pages=0x3f;
  drsms.lines_per_frame = 262;
  if (CurrentEmuMode==EMU_MODE_SMS)
		drsms.gg_mode = 0;
  else
		drsms.gg_mode = 1;
		
  DrSMS_Rebase_Banks();
  
  drsms.banks[0xC]=(unsigned int)work_ram-0xC000;
  drsms.banks[0xD]=(unsigned int)work_ram-0xC000;
  drsms.banks[0xE]=(unsigned int)work_ram-0xE000;
  drsms.banks[0xF]=(unsigned int)work_ram-0xE000;
  
  SMS_update_sound_timing();

  SN76496_sh_start();
  
  DrSMS_DrZ80_Reset();
}

int drsmsInitialize()
{
#ifdef EMU_C68K
	memset(&cyclone, 0, sizeof(cyclone));
#endif
	memset(&drz80, 0, sizeof(drz80));
	memset(&drsms, 0, sizeof(drsms));
	return 1;
}

void drsmsCleanup()
{
}

void drsmsSetSound(int level)
{
	sound_on = level;
}

void drsmsReset()
{
	DrSMS_Init();
	DrSMS_Reset();
}

int drsmsLoadRom(const char *filename)
{
	char buffer[1024];

	last_pad = 0;
	memset(VBuf, 0, sizeof(VBuf));
	memset(RomData, 0, sizeof(RomData));

	if (check_zip(filename)) {
		RomSize = sizeof(RomData);
		if (!load_archive(filename, NULL, RomData, &RomSize,
				buffer, sizeof(buffer)))
			return 0;

		filename = buffer;

	} else {
		FILE *stream = fopen(filename, "rb");
		if (stream == NULL)
			return 0;

		fseek(stream, 0, SEEK_END);
		RomSize = ftell(stream);
		fseek(stream, 0, SEEK_SET);
		fread(RomData, RomSize, 1, stream);
		fclose(stream);
	}

	CurrentEmuMode = guessEmuMode(filename);
	if (CurrentEmuMode == EMU_MODE_NONE)
		return 0;

	DrSMS_Init();
	DrSMS_Reset();

	drsms.frame_buffer = (unsigned int) VBuf;

	if (CurrentEmuMode == EMU_MODE_SMS) {
		drsms.render_line = sms_render_8;
		SMS_load_pal();
	} else {
		drsms.render_line = gg_render_8;
		GG_load_pal();
	}
	SMS_update_sound_timing();
	return 1;
}

void drsmsUnloadRom()
{
	// nothing to do here
}

int drsmsSaveState(const char *filename)
{
	struct SMS_SAVESTATE state;

	savestate_mem(&state);
	int rv = save_archive(filename, "GEAROID",
			(const char *) &state, sizeof(state));
	sync();
	return rv;
}

int drsmsLoadState(const char *filename)
{
	struct SMS_SAVESTATE state;
	int size;

	size = sizeof(state);
	if (!load_archive(filename, "GEAROID", (char *) &state, &size, NULL, 0))
		return 0;

	loadstate_mem(&state);

	DrSMS_Init();
	// now need to update pointers to memory locations because these may be
	// different from when the save state was created.
	
	DrSMS_Rebase_Banks();
	DrSMS_Z80_Rebase_PC(drz80.Z80PC-drz80.Z80PC_BASE);
	DrSMS_Z80_Rebase_SP(drz80.Z80SP-drz80.Z80SP_BASE);
	update_tile_cache();

	return 1;
}

void drsmsRunFrame(int render_video, unsigned int pad)
{
	laststage = 0;

	// SMS pause button
	if (CurrentEmuMode == EMU_MODE_SMS &&
			(pad & GAMEPAD_START) && !(last_pad & GAMEPAD_START))
		drz80.Z80_NMI = 1;
	last_pad = pad;
	drsms.pad = pad;

	DrSMSRun(render_video);

	if (sound_on) {
		if ((unsigned int) laststage < sound_buffer_size) {
			RenderSound(soundbuffer + (laststage << 1),
					sound_buffer_size - laststage);
		}
	}
}
