#define LOG_TAG "libgens"
#include <utils/Log.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include "app.h"

#define SOUND_RATE			22050
#define SOUND_UPDATESTEP	6459

#define SRAM_SAVE_LOOPS		600

#ifdef EMU_C68K
struct Cyclone cyclone;
#endif
struct DrZ80 drz80;
struct DrMD drmd;

unsigned char VBuf[320 * 240];
unsigned int VPalette[256];
unsigned int pal_lookup[0x1000];
unsigned char RomData[0x500000];

unsigned char work_ram[0x10000]; // scratch ram
unsigned char zram[0x4000];
unsigned char sram[0x10000]; // sram
unsigned short vram[0x8000];
unsigned short vsram[0x40];
unsigned short cram[0x40];
unsigned short sample_count_lookup[400];
short soundbuffer[44100 * 2 / 50];
unsigned int sound_buffer_size;
int sram_modified;
int laststage;
int sound_on;
int frame_limit;
int PAL;

static char sram_filename[1024];
static int enable_sram;
static int sram_save_counter;

static int RomSize; 
static int pad_1_type;


static void Detect_Country_Genesis(int *Game_Mode, int *CPU_Mode)
{
	int c_tab[3] = {4, 1, 8};
	int gm_tab[3] = {1, 0, 1};
	int cm_tab[3] = {0, 0, 1};
	int i, coun = 0;
	char c;
	
	if (!strncasecmp((char *) &RomData[0x1F0], "eur", 3)) coun |= 8;
	else if (!strncasecmp((char *) &RomData[0x1F0], "usa", 3)) coun |= 4;
	else if (!strncasecmp((char *) &RomData[0x1F0], "jap", 3)) coun |= 1;
	else for(i = 0; i < 4; i++)
	{
		c = toupper(RomData[0x1F0 + i]);
		
		if (c == 'U') coun |= 4;
		else if (c == 'J') coun |= 1;
		else if (c == 'E') coun |= 8;
		else if (c < 16) coun |= c;
		else if ((c >= '0') && (c <= '9')) coun |= c - '0';
		else if ((c >= 'A') && (c <= 'F')) coun |= c - 'A' + 10;
	}

	if (coun & c_tab[0])
	{
		*Game_Mode = gm_tab[0];
		*CPU_Mode = cm_tab[0];
	}
	else if (coun & c_tab[1])
	{
		*Game_Mode = gm_tab[1];
		*CPU_Mode = cm_tab[1];
	}
	else if (coun & c_tab[2])
	{
		*Game_Mode = gm_tab[2];
		*CPU_Mode = cm_tab[2];
	}
	else if (coun & 2)
	{
		*Game_Mode = 0;
		*CPU_Mode = 1;
	}
	else
	{
		*Game_Mode = 1;
		*CPU_Mode = 0;
	}       
}

static void Genesis_Init(void)
{
  int Game_Mode;

  Detect_Country_Genesis(&Game_Mode, &PAL);

  if(PAL)
  {
     drmd.vdp_status=1;
     drmd.lines_per_frame = 312;
     drmd.cpl_z80 = round((((double) MD_CLOCK_PAL / 15.0) / 50.0) / 312.0);
     drmd.cpl_m68k = round((((double) MD_CLOCK_PAL / 7.0) / 50.0) / 312.0);
     frame_limit=50;
  }
  else
  {
     drmd.vdp_status=0;
     drmd.lines_per_frame = 262;
     drmd.cpl_z80 = round((((double) MD_CLOCK_NTSC / 15.0) / 60.0) / 262.0);
     drmd.cpl_m68k = round((((double) MD_CLOCK_NTSC / 7.0) / 60.0) / 262.0);
     frame_limit=60;
  }
	/*
	(sound rate/frame_rate) =  buffer size
	60 fps
	11Khz = 92
	16Khz = 138
	22Khz = 184
	
	50 fps
	11Khz = 111
	16Khz = 165
22Khz = 221
	
	*/

  drmd.region=((((Game_Mode)<<1)|(PAL))<<6)|0x20;  
}

void illegal_memory_io(unsigned int address, char *error_text)
{
}

#ifdef EMU_C68K
int SekInterrupt(int irq)
{

  cyclone.irq=(unsigned char)irq;
  return 0;
}

static unsigned int DrMDCheckPc(unsigned int pc)
{
  pc-=cyclone.membase; // Get real pc
  pc&=0xffffff;

  if (pc<drmd.romsize)
  {
    cyclone.membase=(int)drmd.cart_rom; // Program Counter in Rom
  }
  else if ((pc&0xe00000)==0xe00000)
  {
    cyclone.membase=(int)drmd.work_ram-(pc&0xff0000); // Program Counter in Ram
  }
  else
  {
    // Error - Program Counter is invalid
    cyclone.membase=(int)drmd.cart_rom;
  }
  return cyclone.membase+pc;
}

static void Cyclone_Init()
{
  CycloneInit();

  cyclone.checkpc=DrMDCheckPc;
  cyclone.fetch8 =cyclone.read8 =m68k_read_memory_8;
  cyclone.fetch16=cyclone.read16=m68k_read_memory_16;
  cyclone.fetch32=cyclone.read32=m68k_read_memory_32;
  cyclone.write8 =m68k_write_memory_8;
  cyclone.write16=m68k_write_memory_16;
  cyclone.write32=m68k_write_memory_32;
  cyclone.IrqCallback = (unsigned int)&m68k_irq_callback;
}

static void Cyclone_Reset()
{
  cyclone.srh = 0x27;
  cyclone.a[7]=m68k_read_memory_32(0);
  cyclone.membase = 0;
  cyclone.pc=DrMDCheckPc(m68k_read_memory_32(4));
}

int Cyclone_ClearIRQ(int irq)
{
	cyclone.irq=irq;
	return CYCLONE_INT_ACK_AUTOVECTOR;
}
#endif

#ifdef EMU_M68K
static int vdp_int_ack_callback(int int_level)
{
    if(drmd.vdp_reg1&0x20)
	{
		drmd.vint_pending = 0;
	}
	else
	{
		drmd.hint_pending = 0;
		drmd.vint_pending = 0;
	}

    return M68K_INT_ACK_AUTOVECTOR;
}
#endif

static void DrMD_DrZ80_Set_Irq(unsigned int irq)
{
    drz80.z80irqvector = 0xFF;
	drz80.Z80_IRQ = irq;
}

static void DrMD_DrZ80_irq_callback(void)
{
    drz80.Z80_IRQ=0;  // lower irq when in accepted
}

static void DrMD_DrZ80_Init()
{
  drz80.z80_write8=DrMD_Z80_write_8;
  drz80.z80_write16=DrMD_Z80_write_16;
  drz80.z80_in=DrMD_Z80_In;
  drz80.z80_out=DrMD_Z80_Out;
  drz80.z80_read8=DrMD_Z80_read_8;
  drz80.z80_read16=DrMD_Z80_read_16;
  drz80.z80_rebasePC=DrMD_Z80_Rebase_PC;
  drz80.z80_rebaseSP=DrMD_Z80_Rebase_SP;
  drz80.z80_irq_callback=DrMD_DrZ80_irq_callback;
}

static void DrMD_DrZ80_Reset()
{
  drz80.Z80A = 0;
  drz80.Z80F = 1<<2;  // set ZFlag
  drz80.Z80BC = 0;
  drz80.Z80DE = 0;
  drz80.Z80HL = 0;
  drz80.Z80A2 = 0;
  drz80.Z80F2 = 1<<2;  // set ZFlag
  drz80.Z80BC2 = 0;
  drz80.Z80DE2 = 0;
  drz80.Z80HL2 = 0;
  drz80.Z80IX = 0xFFFF0000;
  drz80.Z80IY = 0xFFFF0000;
  drz80.Z80I = 0;
  drz80.Z80IM = 0;
  drz80.Z80_IRQ = 0;
  drz80.Z80IF = 0;
  drz80.Z80PC=DrMD_Z80_Rebase_PC(0);
  drz80.Z80SP=DrMD_Z80_Rebase_SP(0);
}

static void MD_update_sound_timing(void)
{
     int i;
     float sample_time;
     float cpuspeed;
     
     if(frame_limit==60) cpuspeed=MD_CLOCK_NTSC;
     else cpuspeed=MD_CLOCK_PAL;
     
     drmd.cpl_fm = (float)(((cpuspeed / 7.0) / SOUND_RATE) / 144.0);
	 // hack for speed
	 //OPN.eg_timer_add = 211570;
	 // correct values
     OPN.eg_timer_add = (1<<EG_SH)  *  drmd.cpl_fm;  // 16K 211570.22106782106782106782106782	                                                // 11K 316635.70499946010150091782744844
						        // 8K  423140.44213564213564213564213564
     OPN.eg_timer_overflow = (( 3 ) * (1<<EG_SH))<<1;  // always 196608
     timer_base = ((((cpuspeed / 7.0)/(float)frame_limit)/((float)drmd.lines_per_frame-1.0))/144.0)* 4096.0;
     //timer_base = drmd.cpl_fm * 4096.0;  // 16k 13223.138816738816738816738816739
                                         // 11k  19789.731562466256343807364215527
					 // 8k 26446.277633477633477633477633478
     sound_buffer_size=((SOUND_RATE/frame_limit));
     PSG_sn_UPDATESTEP = SOUND_UPDATESTEP;
     update_tables();
     
     sample_time = (SOUND_RATE/(float)frame_limit)/((float)drmd.lines_per_frame-1.0);
     for(i=0;i<=drmd.lines_per_frame-1;i++)
     {
	sample_count_lookup[i] = sample_time * i;
     }
}

static void MD_load_pal(void)
{
   unsigned short mdpal;
   for (mdpal = 0; mdpal < 0x1000; mdpal++) {
      // md  0000 bbb0 ggg0 rrr0
      // us  rrrr rggg ggbb bbbi
      pal_lookup[mdpal] =
	  	((mdpal & 0x000e) << 12) |
		((mdpal & 0x00e0) << 3) |
		((mdpal & 0x0e00) >> 7);
   }
}

static void DrMD_Init()
{
#ifdef EMU_C68K
  drmd.m68k_context = (unsigned int)&cyclone; // pointer to M68K emulator
  drmd.m68k_run = CycloneRun;     //pointer to Mk68k run functoin
  drmd.m68k_set_irq = SekInterrupt;
#endif

#ifdef EMU_M68K
  drmd.m68k_context = (unsigned int)0; // pointer to M68K emulator
  drmd.m68k_run = m68k_execute;     //pointer to Mk68k run functoin
  drmd.m68k_set_irq = m68k_set_irq;
#endif

  drmd.z80_context = (unsigned int)&drz80;
  drmd.z80_run = DrZ80Run;
  drmd.z80_set_irq = DrMD_DrZ80_Set_Irq;
  drmd.z80_reset = DrMD_DrZ80_Reset;
  drmd.fm_write = fm_write;
  drmd.fm_read = fm_read;
  drmd.psg_write = psg_write;
  drmd.cart_rom = (unsigned int)RomData;
  drmd.work_ram = (unsigned int)work_ram;
  drmd.zram = (unsigned int)zram;
  drmd.vram = (unsigned int)vram;
  drmd.cram = (unsigned int)cram;
  drmd.gp32_pal = (unsigned int) VPalette;
  drmd.vsram = (unsigned int)vsram;
  drmd.sram = (unsigned int)sram;  
}

static void DrMD_Reset()
{
  unsigned int x=0;
  // reset DrMD
  drmd.m68k_aim = 0;
  drmd.m68k_total = 0;
  drmd.zbank = 0;
  drmd.romsize = RomSize;
  // hword variables
  drmd.vdp_line = 0;
  //drmd.vdp_status = 0;
  drmd.vdp_addr = 0;
  drmd.vdp_addr_latch = 0;
  // byte variables
  drmd.pad = 0;
  drmd.padselect = 0x40;
  drmd.zbusreq = 0;
  drmd.zbusack = 1;
  drmd.zreset = 0;
  drmd.vdp_reg0 = 0;
  drmd.vdp_reg1 = 0;
  drmd.vdp_reg2 = 0;
  drmd.vdp_reg3 = 0;
  drmd.vdp_reg4 = 0;
  drmd.vdp_reg5 = 0;
  drmd.vdp_reg6 = 0;
  drmd.vdp_reg7 = 0;
  drmd.vdp_reg8 = 0;
  drmd.vdp_reg9 = 0;
  drmd.vdp_reg10 = 0;
  drmd.vdp_reg11 = 0;
  drmd.vdp_reg12 = 0;
  drmd.vdp_reg13 = 0;
  drmd.vdp_reg14 = 0;
  drmd.vdp_reg15 = 0;
  drmd.vdp_reg16 = 0;
  drmd.vdp_reg17 = 0;
  drmd.vdp_reg18 = 0;
  drmd.vdp_reg19 = 0;
  drmd.vdp_reg20 = 0;
  drmd.vdp_reg21 = 0;
  drmd.vdp_reg22 = 0;
  drmd.vdp_reg23 = 0;
  drmd.vdp_reg24 = 0;
  drmd.vdp_reg25 = 0;
  drmd.vdp_reg26 = 0;
  drmd.vdp_reg27 = 0;
  drmd.vdp_reg28 = 0;
  drmd.vdp_reg29 = 0;
  drmd.vdp_reg30 = 0;
  drmd.vdp_reg31 = 0;
  drmd.vdp_reg32 = 0;
  drmd.vdp_counter = 255;
  drmd.hint_pending = 0;
  drmd.vint_pending = 0;
  drmd.vdp_pending = 0;
  drmd.vdp_code = 0;
  drmd.vdp_dma_fill = 0;
  drmd.pad_1_type = pad_1_type;
  drmd.pad_1_status = 0xFF;  
  drmd.pad_1_com = 0x00; 
  drmd.pad_2_status = 0xFF;  
  drmd.pad_2_com = 0x00;
  drmd.sram_start = 0;
  drmd.sram_end = 0;
  drmd.sram_flags = 0;
  //for(x=0;x<0x10;x++)
  //{
  // drmd.genesis_rom_banks,0,0x10); //default memory rom banks
  //}
  genesis_rom_banks_reset();
  
  // setup sram values
  if ((RomData[424 + 8] == 'R') && (RomData[424 + 9] == 'A') && (RomData[424 + 10] & 0x40))
	{
		drmd.sram_start = RomData[436] << 24;
		drmd.sram_start |= RomData[437] << 16;
		drmd.sram_start |= RomData[438] << 8;
		drmd.sram_start |= RomData[439];
		drmd.sram_start &= 0x0F80000;		// multiple de 0x080000
		
		drmd.sram_end = RomData[440] << 24;
		drmd.sram_end |= RomData[441] << 16;
		drmd.sram_end |= RomData[442] << 8;
		drmd.sram_end |= RomData[443];
	}
	else
	{
		drmd.sram_start = 0x200000;
		drmd.sram_end = 0x200000 + (64 * 1024) - 1;
	}

	if ((drmd.sram_start > drmd.sram_end) || ((drmd.sram_end - drmd.sram_start) >= (64 * 1024)))
		drmd.sram_end = drmd.sram_start + (64 * 1024) - 1;

	if (drmd.romsize <= (2 * 1024 * 1024))
	{
		drmd.sram_flags = (1<<0)|(1<<1);
	}

	drmd.sram_start &= 0xFFFFFFFE;
	drmd.sram_end |= 0x00000001;

//		sprintf(Str_Err, "deb = %.8X end = %.8X", SRAM_Start, SRAM_End);
//		MessageBox(NULL, Str_Err, "", MB_OK);

	//if ((drmd.sram_end - drmd.sram_start) <= 2) SRAM_Custom = 1;
	//else SRAM_Custom = 0;
}

static void reset_drmd(void)
{
// DrMD
  DrMD_Init();
  DrMD_Reset();

  // now pick best hardware
  Genesis_Init();
  
#ifdef EMU_C68K
  // reset Cyclone
  Cyclone_Init();
  Cyclone_Reset();
#endif
#ifdef EMU_M68K
  m68k_set_cpu_type(M68K_CPU_TYPE_68000);
  m68k_pulse_reset();
#endif

  // reset DrZ80
  DrMD_DrZ80_Init();
  DrMD_DrZ80_Reset();
  
  // clear memory
  memset(&work_ram,0,0x10000);
  memset(&vram,0,0x10000);
  memset(&cram,0,0x80);
  memset(&zram,0,0x4000);
  memset(&vsram,0,0x40);

  MD_update_sound_timing();
  SN76496_sh_start();
  YM2612Init();
}

static void Byteswap(unsigned char *data,int len)
{
  int i=0;

  if (len<2) return; // Too short

  do
  {
    unsigned short *pd=(unsigned short *)(data+i);
    int value=*pd; // Get 2 bytes

    value=(value<<8)|(value>>8); // Byteswap it
    *pd=(unsigned short)value; // Put 2b ytes
    i+=2;
  }  
  while (i+2<=len);
}

// Interleve a 16k block and byteswap
static int InterleveBlock(unsigned char *dest,unsigned char *src)
{
  int i=0;
  for (i=0;i<0x2000;i++) dest[(i<<1)  ]=src[       i]; // Odd
  for (i=0;i<0x2000;i++) dest[(i<<1)+1]=src[0x2000+i]; // Even
  return 0;
}

// Decode a SMD file
static int DecodeSmd(unsigned char *data,int len)
{
  unsigned char *temp=NULL;
  int i=0;

  temp=(unsigned char *)malloc(0x4000);
  if (temp==NULL) return 1;
  memset(temp,0,0x4000);

  // Interleve each 16k block and shift down by 0x200:
  for (i=0; i+0x4200<=len; i+=0x4000)
  {
    InterleveBlock(temp,data+0x200+i); // Interleve 16k to temporary buffer
    memcpy(data+i,temp,0x4000); // Copy back in
  }

  free(temp);
  return 0;
}

static void drmdLoadSRAM()
{
	sram_modified = 0;

	if (enable_sram) {
		FILE *stream = fopen(sram_filename, "rb");
		if (stream) {
			fread(sram, sizeof(sram), 1, stream);
			fclose(stream);
		}
	}
}

static void drmdSaveSRAM()
{
	if (enable_sram && sram_modified) {
		FILE *stream = fopen(sram_filename, "wb");
		if (stream) {
			fwrite(sram, sizeof(sram), 1, stream);
			fclose(stream);
		}
		sram_modified = 0;
	}
}

int drmdInitialize()
{
#ifdef EMU_C68K
	memset(&cyclone, 0, sizeof(cyclone));
#endif
	memset(&drz80, 0, sizeof(drz80));
	memset(&drmd, 0, sizeof(drmd));
	return 1;
}

void drmdCleanup()
{
}

void drmdEnableSRAM(int enable)
{
	enable_sram = enable;
}

void drmdSetPadType(int type)
{
	pad_1_type = type;
}

void drmdSetSound(int level)
{
	sound_on = level;
}

void drmdPower()
{
	reset_drmd();
}

void drmdReset()
{
	Cyclone_Reset();
}

int drmdLoadRom(const char *filename)
{
	char *p;

	memset(VBuf, 0, sizeof(VBuf));
	memset(RomData, 0, sizeof(RomData));

	if (check_zip(filename)) {
		RomSize = sizeof(RomData);
		if (!load_archive(filename, NULL, RomData, &RomSize))
			return 0;
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

	if ((RomSize & 0x3fff) == 0x200) {
		DecodeSmd(RomData, RomSize);
		RomSize -= 0x200;
	} else {
		Byteswap(RomData, RomSize);
	}

	memset(&sram, 0, 0x10000);
	reset_drmd();
	drmd.render_line = md_render_8;
	drmd.frame_buffer = (unsigned int) VBuf;

	MD_load_pal();
	update_md_pal();
	MD_update_sound_timing();

	// get sram file name
	strcpy(sram_filename, filename);
	p = strrchr(sram_filename, '.');
	if (p)
		*p = '\0';
	strcat(sram_filename, ".sav");

	sram_save_counter = SRAM_SAVE_LOOPS;
	drmdLoadSRAM();
	return 1;
}

void drmdUnloadRom()
{
	drmdSaveSRAM();
}

int drmdSaveState(const char *filename)
{
	struct MD_SAVESTATE state;

	savestate_mem(&state);
	int rv = save_archive(filename, "GENSOID",
			(const char *) &state, sizeof(state));
	sync();
	return rv;
}

int drmdLoadState(const char *filename)
{
	struct MD_SAVESTATE state;
	int size;
	int x;

	size = sizeof(state);
	if (!load_archive(filename, "GENSOID", (char *) &state, &size))
		return 0;

	loadstate_mem(&state);

	// update direct memory pointers, this is because
	// for example if DrMD is compiled with slight changes
	// the location of RomData or main md memory is
	// moved but the Z80PC will still be pointing at a memory
	// address based on the old location of rom data
	
	DrMD_Init();
#ifdef EMU_C68K
	Cyclone_Init();
#endif
	DrMD_DrZ80_Init();
#ifdef EMU_C68K
	// update cyclone pointers
	cyclone.pc=DrMDCheckPc(cyclone.pc);  // rebase pc
#endif
	// update drz80 pointers
	drz80.Z80PC=drz80.Z80PC-drz80.Z80PC_BASE;
	drz80.Z80PC=DrMD_Z80_Rebase_PC(drz80.Z80PC);
	
	drz80.Z80SP=drz80.Z80SP-drz80.Z80SP_BASE;
	drz80.Z80SP=DrMD_Z80_Rebase_SP(drz80.Z80SP);
	
	//Now need to reload banked rom banks
	for(x=0;x<7;x++)
	{
	   genesis_rom_bank_set(x,drmd.genesis_rom_banks[x]);
	}

	// re-sync gp32 pal and md pal;
	update_md_pal();
	return 1;
}

void drmdRunFrame(int render_video)
{
	laststage = 0;
	current_sample = 0;
	last_sample = 0;
	dac_sample = 0;

	DrMDRun(render_video);

	if (sound_on) {
		RefreshFm();
		if ((unsigned int) laststage < sound_buffer_size) {
			YM2612UpdateOne(soundbuffer + (laststage << 1),
					sound_buffer_size - laststage);
		}
	}

	if (--sram_save_counter <= 0) {
		sram_save_counter = SRAM_SAVE_LOOPS;
		drmdSaveSRAM();
	}
}
