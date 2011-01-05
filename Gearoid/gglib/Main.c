#include "app.h"


#ifdef __GP32__
#include "gbax.h"
#include "gp32.h"
#endif

#ifdef __GP2X__
#include "gbax_gp2x.h"
#endif

#ifdef __GIZ__
#include "sponser_giz.h"
#include <FrameworkAudio.h>
#endif



extern struct RomList_Item romlist[];
#if defined (__GP2X__) || defined (__GIZ__)
unsigned int sound_buffer_size;
#endif

unsigned char gamma_conv[32*29]={   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                    0, 2, 3, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 29, 30, 31,
                                    0, 3, 5, 7, 8, 9, 10, 11, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 30, 31,
                                    0, 4, 6, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 30, 31,
                                    0, 6, 8, 10, 11, 12, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31,
                                    0, 7, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31,
                                    0, 8, 10, 12, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31,
                                    0, 9, 11, 13, 15, 16, 17, 18, 19, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31,
                                    0, 10, 12, 14, 16, 17, 18, 19, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31,
                                    0, 11, 13, 15, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31,
                                    0, 12, 14, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31,
                                    0, 12, 15, 17, 18, 19, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31,
                                    0, 13, 16, 17, 19, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31,
                                    0, 14, 16, 18, 19, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31,
                                    0, 14, 17, 18, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31,
                                    0, 15, 17, 19, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31,
                                    0, 16, 18, 19, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31,
                                    0, 16, 18, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31,
                                    0, 17, 19, 20, 21, 22, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31,
                                    0, 17, 19, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31,
                                    0, 17, 20, 21, 22, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31,
                                    0, 18, 20, 21, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31,
                                    0, 18, 20, 22, 23, 23, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 19, 21, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 19, 21, 22, 23, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 19, 21, 22, 23, 24, 25, 25, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 20, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 20, 22, 23, 24, 24, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31,
                                    0, 20, 22, 23, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31};

char system_dir[MAX_PATH+1];  // system path currently in use
char system_root_dir[MAX_PATH+1]; 
char md_system_dir[MAX_PATH+1];
char sms_system_dir[MAX_PATH+1];
char gg_system_dir[MAX_PATH+1];
char last_save_name[MAX_PATH+1];
char last_rom_name[MAX_PATH+1];
unsigned char pad_config[32];
char show_fps=0;
char frameskip=0;
char sound_rate=0;
char sound_on=0;
char cpu_speed=0;
char pad1;
char pad2;
char pad3;
static char fps_display[256];
unsigned char work_ram[0x10000]; // scratch ram
unsigned char zram[0x4000];
unsigned char sram[0x10000]; // sram
unsigned int tile_cache[0x4000>>2];
unsigned short vram[0x8000];
unsigned short vsram[0x40];
unsigned short cram[0x40];
unsigned short sample_count_lookup[400];

extern volatile int Timer;
int gp32_lcdver=1;
int prevFlip=0;
int Flip=0;
int CurrentEmuMode=EMU_MODE_NONE;
int savestatesize=0;
int romloaded=0;
int Frames=0,Taken=0; // Frames per Time
static int Game_Mode=0,CPU_Mode=0;
static int frame_limit=60;
static int sms_pause_button;
int laststage;
unsigned int pal_lookup[0x1000];
unsigned int sn_UPDATESTEP[5]={2422,3230,4845,6459,12920};

float sound_rates[5]={8250.0,11025.0,16500.0,22050.0,44100.0};

unsigned char *RomData=NULL;
#ifdef __GP32__
unsigned char* framebuffer8[3];
unsigned short* framebuffer16[3];
#endif
void *EmuFB[4];
void *EmuFBcentered[4];
char *volume=NULL;
unsigned int *frame_buffer=NULL;
unsigned int *pad=NULL;
short *soundbuffer;

#ifdef EMU_C68K
struct Cyclone cyclone;
#endif
struct DrZ80 drz80;
#if defined (__EMU_MD__)
struct DrMD drmd;
#endif
#if defined (__EMU_SMS__)
struct DrSMS drsms;
#endif


//Conv
#if defined (__EMU_MD__)
#if defined (__GIZ__)
void DrMDBlit8To16(int line)
{
	//unsigned int pixels=0;
	unsigned int offset8=(drmd.vdp_reg1&0x8)?0:320*8;
	unsigned int offset16=(drmd.vdp_reg1&0x8)?0:642*8;
	unsigned int pixTo=(unsigned int)GizPrimaryFrameBuffer;
	unsigned int pixFrom=(unsigned int)framebuffer8[Flip];
	
	pixTo+=(((642 * line) + offset16));
	pixFrom+=(((320 * line) + offset8));

	Blit8To16Asm(pixFrom,pixTo, &gp2x_palette,320);
}
#endif
#endif
#if defined (__EMU_SMS__)
#if defined (__GIZ__)
void DrSMSBlit8To16(int line)
{
	//unsigned int pixels=0;
	if (drsms.gg_mode)
	{
		if((line>=24)&&(line<176))
		{
			unsigned int offset8=80+(320*32);
			unsigned int offset16=160+(642*32);
			unsigned int pixTo=(unsigned int)GizPrimaryFrameBuffer;
			unsigned int pixFrom=(unsigned int)framebuffer8[Flip];
			
			pixTo+=(((642 * (line-24)) + offset16));
			pixFrom+=(((320 * (line-24)) + offset8));

			Blit8To16Asm(pixFrom,pixTo, &gp2x_palette,160);
		}
	}
	else
	{
		unsigned int offset8=36+(320*24);
		unsigned int offset16=72+(642*24);
		unsigned int pixTo=(unsigned int)GizPrimaryFrameBuffer;
		unsigned int pixFrom=(unsigned int)framebuffer8[Flip];
		
		pixTo+=(((642 * line) + offset16));
		pixFrom+=(((320 * line) + offset8));

		Blit8To16Asm(pixFrom,pixTo, &gp2x_palette,240);
	}
	
}
#endif
#endif
void update_menu_graphics_gamma(void)
{
   unsigned int curr_pix=0;
   unsigned short pixel=0;
   unsigned char R,G,B;
   for(curr_pix=0;curr_pix<15360;curr_pix++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      pixel=menu_header_orig[curr_pix];
      R=(pixel>>11)&0x1F; // 0000 0RRR - 3 bits Red
      G=(pixel>>6)&0x1F; 
      B=(pixel>>1)&0x1F;
      
      // Do gamma correction  
      R=gamma_conv[R+(0<<5)];
      G=gamma_conv[G+(0<<5)];
      B=gamma_conv[B+(0<<5)];

      pixel=RGB(R,G,B);
      menu_header[curr_pix]=pixel;
   }
   for(curr_pix=0;curr_pix<5120;curr_pix++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      pixel=highlightbar_orig[curr_pix];
      R=(pixel>>11)&0x1F; // 0000 0RRR - 3 bits Red
      G=(pixel>>6)&0x1F; 
      B=(pixel>>1)&0x1F;
      
      // Do gamma correction  
      R=gamma_conv[R+(0<<5)];
      G=gamma_conv[G+(0<<5)];
      B=gamma_conv[B+(0<<5)];

      pixel=RGB(R,G,B);
      highlightbar[curr_pix]=pixel;
   
   }
   
   for(curr_pix=0;curr_pix<(menutile_width*menutile_height);curr_pix++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      pixel=menutile_orig[curr_pix];
      R=(pixel>>11)&0x1F; // 0000 0RRR - 3 bits Red
      G=(pixel>>6)&0x1F; 
      B=(pixel>>1)&0x1F;
      
      // Do gamma correction  
      R=gamma_conv[R+(0<<5)];
      G=gamma_conv[G+(0<<5)];
      B=gamma_conv[B+(0<<5)];

      pixel=RGB(R,G,B);
      menutile[curr_pix]=pixel;
   
   }
}
#if defined (__GP32__) && defined (__EMU_MD__)
void MD_load_pal(void)
{
   unsigned short mdpal=0;
   unsigned char R,G,B;
   for(mdpal=0;mdpal<0x1000;mdpal++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      R=(mdpal>>1)&7; // 0000 0RRR - 3 bits Red
      R=(R|R<<3)>>1;  // 000R RRRR - 5 bits Red
      G=(mdpal>>5)&7; 
      G=(G|G<<3)>>1;  
      B=(mdpal>>9)&7;
      B=(B|B<<3)>>1; 
      
      // Do gamma correction  
      R=gamma_conv[R+(md_menu_options.gamma<<5)];
      G=gamma_conv[G+(md_menu_options.gamma<<5)];
      B=gamma_conv[B+(md_menu_options.gamma<<5)];

	  pal_lookup[mdpal]=RGB(R,G,B);
   }
}
#endif

#if defined (__GP2X__) 
void MD_load_pal(void)
{
   unsigned short mdpal=0;
   unsigned char R,G,B;
   for(mdpal=0;mdpal<0x1000;mdpal++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      R=(mdpal>>1)&7; // 0000 0RRR - 3 bits Red
      R=(R|R<<3)>>1;  // 000R RRRR - 5 bits Red
      G=(mdpal>>5)&7; 
      G=(G|G<<3)>>1;  
      B=(mdpal>>9)&7;
      B=(B|B<<3)>>1; 
      
      // Do gamma correction  
      R=gamma_conv[R+(md_menu_options.gamma<<5)];
      G=gamma_conv[G+(md_menu_options.gamma<<5)];
      B=gamma_conv[B+(md_menu_options.gamma<<5)];

	  R=(R|(R<<5))>>2; // 0x000R RRRR
	                   // 0xRRRR RRRR RR
	  G=(G|(G<<5))>>2;
	  B=(B|(B<<5))>>2;

	  pal_lookup[mdpal]=(G<<8)|(B<<0)|(R<<16);
   }
}
#endif

#if defined (__GIZ__)
void MD_load_pal(void)
{
   unsigned short mdpal=0;
   unsigned char R,G,B;
   for(mdpal=0;mdpal<0x1000;mdpal++)
   {
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      R=(mdpal>>1)&7; // 0000 0RRR - 3 bits Red
      R=(R|R<<3)>>1;  // 000R RRRR - 5 bits Red
      G=(mdpal>>5)&7; 
      G=(G|G<<3)>>1;  
      B=(mdpal>>9)&7;
      B=(B|B<<3)>>1; 
      
      // Do gamma correction  
      R=gamma_conv[R+(md_menu_options.gamma<<5)];
      G=gamma_conv[G+(md_menu_options.gamma<<5)];
      B=gamma_conv[B+(md_menu_options.gamma<<5)];

	  /*R=(R|(R<<5))>>2; // 0x000R RRRR
	                   // 0xRRRR RRRR RR
	  G=(G|(G<<5))>>2;
	  B=(B|(B<<5))>>2;
	  
	  pal_lookup[mdpal]=(G<<8)|(B<<16)|(R<<0);*/
	  
	  pal_lookup[mdpal]=RGB(R,G,B);
	  
   }
}
#endif

#if defined (__EMU_MD__)
void MD_update_sound_timing(void)
{
     int i;
     float sample_time;
     float cpuspeed;
     
     if(frame_limit==60) cpuspeed=MD_CLOCK_NTSC;
     else cpuspeed=MD_CLOCK_PAL;
     
     drmd.cpl_fm = (float)(((cpuspeed / 7.0) / sound_rates[md_menu_options.sound_rate]) / 144.0);
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
     sound_buffer_size=((sound_rates[md_menu_options.sound_rate]/frame_limit));
     PSG_sn_UPDATESTEP = sn_UPDATESTEP[md_menu_options.sound_rate];
     update_tables();
     
     sample_time = (sound_rates[md_menu_options.sound_rate]/(float)frame_limit)/((float)drmd.lines_per_frame-1.0);
     for(i=0;i<=drmd.lines_per_frame-1;i++)
     {
	sample_count_lookup[i] = sample_time * i;
     }
}
#endif

#if defined (__EMU_SMS__) && defined (__GP32__)		    
void SMS_load_pal(void)
{
	unsigned short smspal=0;
	unsigned char pal_decode[4] = {0x00,0x0A,0x15,0x1F};
	unsigned char R,G,B;

	unsigned int *pal = (unsigned int*)PALETTE;
	for(smspal=0;smspal<64;smspal++)
	{

		R=pal_decode[smspal&3]; 
		G=pal_decode[(smspal>>2)&3]; 
		B=pal_decode[(smspal>>4)&3]; 
      
		// Do gamma correction  
		R=gamma_conv[R+(sms_menu_options.gamma<<5)];
		G=gamma_conv[G+(sms_menu_options.gamma<<5)];
		B=gamma_conv[B+(sms_menu_options.gamma<<5)];
		*pal++ = (R<<11)|(G<<6)|(B<<1);
	}
}
void GG_load_pal(void)
{
	unsigned short smspal=0;
	unsigned char pal_decode[4] = {0x00,0x0A,0x15,0x1F};
	unsigned char R,G,B;

	unsigned int *pal = (unsigned int*)PALETTE;
	for(smspal=0;smspal<64;smspal++)
	{

		R=pal_decode[smspal&3]; 
		G=pal_decode[(smspal>>2)&3]; 
		B=pal_decode[(smspal>>4)&3]; 
      
		// Do gamma correction  
		R=gamma_conv[R+(sms_menu_options.gamma<<5)];
		G=gamma_conv[G+(sms_menu_options.gamma<<5)];
		B=gamma_conv[B+(sms_menu_options.gamma<<5)];
		*pal++ = (R<<11)|(G<<6)|(B<<1);
	}
}
#endif

#if defined (__GP2X__) || defined (__GIZ__)		    
void SMS_load_pal(void)
{
   unsigned short smspal=0;
   unsigned char pal_decode[4] = {0x00,0x0A,0x15,0x1F};
   unsigned char R,G,B;
   unsigned int *pal=(unsigned int*)pal_lookup;
   for(smspal=0;smspal<0x100;smspal++)
   {

      R=pal_decode[smspal&3]; 
	  G=pal_decode[(smspal>>2)&3]; 
	  B=pal_decode[(smspal>>4)&3]; 
      
      // Do gamma correction  
      R=gamma_conv[R+(sms_menu_options.gamma<<5)];
      G=gamma_conv[G+(sms_menu_options.gamma<<5)];
      B=gamma_conv[B+(sms_menu_options.gamma<<5)];
	  
#ifdef __GIZ__
      pal_lookup[smspal]=RGB(R,G,B);
#endif
#ifdef __GP2X__
	  R=(R|(R<<5))>>2; // 0x000R RRRR
	                   // 0xRRRR RRRR RR
	  G=(G|(G<<5))>>2;
	  B=(B|(B<<5))>>2;
      *pal++ = (G<<8)|(B<<0)|(R<<16);
#endif
   }
   gp2x_video_setpalette();
}
#endif

#if defined (__GP2X__) || defined (__GIZ__)	    
void GG_load_pal(void)
{
   unsigned short pal=0;
   unsigned char R,G,B;
   for(pal=0;pal<0x1000;pal++)
   {
      R=(pal>>0)&0xF; // 0000 0RRR - 3 bits Red
      G=(pal>>4)&0xF; 
      B=(pal>>8)&0xF;
      
	  R=(R<<1)|(R>>3);
	  G=(G<<1)|(G>>3);
	  B=(B<<1)|(B>>3);
      // Do gamma correction  
      R=gamma_conv[R+(gg_menu_options.gamma<<5)];
      G=gamma_conv[G+(gg_menu_options.gamma<<5)];
      B=gamma_conv[B+(gg_menu_options.gamma<<5)];

	  pal_lookup[pal]=RGB(R,G,B);
   }
}
#endif

#if defined (__EMU_SMS__)
void SMS_update_sound_timing(void)
{
	int i;
	float sample_time;
	
	sound_buffer_size=((sound_rates[sms_menu_options.sound_rate]/frame_limit));
	PSG_sn_UPDATESTEP = sn_UPDATESTEP[sms_menu_options.sound_rate];
     
	sample_time = (sound_rates[sms_menu_options.sound_rate]/(float)frame_limit)/((float)drsms.lines_per_frame-1.0);
	for(i=0;i<=drsms.lines_per_frame-1;i++)
	{
		sample_count_lookup[i] = sample_time * i;
	}
}

void GG_update_sound_timing(void)
{
	int i;
	float sample_time;
	
	sound_buffer_size=((sound_rates[gg_menu_options.sound_rate]/frame_limit));
	PSG_sn_UPDATESTEP = sn_UPDATESTEP[gg_menu_options.sound_rate];
     
	sample_time = (sound_rates[gg_menu_options.sound_rate]/(float)frame_limit)/((float)drsms.lines_per_frame-1.0);
	for(i=0;i<=drsms.lines_per_frame-1;i++)
	{
		sample_count_lookup[i] = sample_time * i;
	}
}
#endif

int Round_Double(double val)
{
	if ((val - (double) (int) val) > 0.5) return (int) (val + 1);
	else return (int) val;
}

void clear_screen(unsigned int *buffer,unsigned int data)
{
   int x=0;
   for(x=0;x<((320*240)/4);x++)
   {
     *buffer++ = data;
   }
}

void clear_screen16(unsigned int *buffer,unsigned int data)
{
   int x=0;
   for(x=0;x<((320*240*2)/4);x++)
   {
     *buffer++ = data;
   }
}

void clear_screen8(unsigned int *buffer,unsigned int data)
{
   int x=0;
   for(x=0;x<((320*240)/4);x++)
   {
     *buffer++ = data;
   }
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

/*void DrMD_Fix_Controllers(void)
{
	if(drmd.pad_1_delay > 25)
	{
		drmd.pad_1_counter = 0;
	}
	else
	{
		drmd.pad_1_delay++;
	}
	
	if(drmd.pad_2_delay > 25)
	{
		drmd.pad_2_counter = 0;
	}
	else
	{
		drmd.pad_2_delay++;
	}

}*/


int CheckSram()
{
	int i=0,c=0;
	for (i=0;i<0x10000;i++)
	{
		if(sram[i]!=0)c++;
	}
	return c;
}

void LoadSram(char *path,char *romname,char *ext,char *srammem)
{
	char filename[MAX_PATH+1];
	char oldext[MAX_PATH+1];
	char fullfilename[MAX_PATH+MAX_PATH+1];
	FILE *stream;
	int size=0;
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Loading...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
		
	SplitFilename(romname,filename,oldext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,filename,ext);
	stream=fopen(fullfilename,"rb");
	if(stream)
	{
		// File exists do try to load it
		fclose(stream);
		size=0;
		
		load_archive(fullfilename, srammem, &size, 0);
	}
}

void SaveSram(char *path,char *romname,char *ext,char *srammem)
{
	char filename[MAX_PATH+1];
	char oldext[MAX_PATH+1];
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Saving...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
	SplitFilename(romname,filename,oldext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,filename,ext);
	gp_setCpuspeed(MENU_FAST_CPU_SPEED);
	save_archive(fullfilename,srammem,0x10000,prevFlip);
	sync();
	gp_setCpuspeed(MENU_CPU_SPEED);		
}

void DeleteSram(char *path,char *romname,char *ext)
{
	char filename[MAX_PATH+1];
	char oldext[MAX_PATH+1];
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Saving...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
	SplitFilename(romname,filename,oldext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,filename,ext);
	remove(fullfilename);		
	sync();
}

#if defined (__EMU_MD__)	
static
void MDDefaultMenuOptions(void)
{
	// no options file loaded, so set to defaults
	md_menu_options.menu_ver=MD_OPTIONS_VER;
	md_menu_options.frameskip=0;
	md_menu_options.sound_on = 1;  // psg and fm on
	md_menu_options.volume=50; 
	memset(md_menu_options.pad_config,0xFF,sizeof(md_menu_options.pad_config));
#ifdef __GP32__
	md_menu_options.cpu_speed=9; // default 144
#endif
#if defined (__GP2X__) || defined (__GIZ__)
	md_menu_options.cpu_speed=19; // default 200
#endif
	// set default key options
	
#ifdef __GP32__
	md_menu_options.pad_config[INP_BUTTON_UP]=MD_BUTTON_UP;
	md_menu_options.pad_config[INP_BUTTON_DOWN]=MD_BUTTON_DOWN;
	md_menu_options.pad_config[INP_BUTTON_LEFT]=MD_BUTTON_LEFT;
	md_menu_options.pad_config[INP_BUTTON_RIGHT]=MD_BUTTON_RIGHT;
	md_menu_options.pad_config[INP_BUTTON_SELECT]=MD_BUTTON_A;
	md_menu_options.pad_config[INP_BUTTON_B]=MD_BUTTON_B;
	md_menu_options.pad_config[INP_BUTTON_A]=MD_BUTTON_C;
	md_menu_options.pad_config[INP_BUTTON_R]=0;
	md_menu_options.pad_config[INP_BUTTON_L]=0;
	md_menu_options.pad_config[INP_BUTTON_START]=MD_BUTTON_START;
#endif
#if defined (__GP2X__)
	md_menu_options.pad_config[INP_BUTTON_UP]=MD_BUTTON_UP;
	md_menu_options.pad_config[INP_BUTTON_DOWN]=MD_BUTTON_DOWN;
	md_menu_options.pad_config[INP_BUTTON_LEFT]=MD_BUTTON_LEFT;
	md_menu_options.pad_config[INP_BUTTON_RIGHT]=MD_BUTTON_RIGHT;
	md_menu_options.pad_config[INP_BUTTON_A]=MD_BUTTON_A;
	md_menu_options.pad_config[INP_BUTTON_X]=MD_BUTTON_B;
	md_menu_options.pad_config[INP_BUTTON_B]=MD_BUTTON_C;
	md_menu_options.pad_config[INP_BUTTON_L]=MD_BUTTON_X;
	md_menu_options.pad_config[INP_BUTTON_Y]=MD_BUTTON_Y;
	md_menu_options.pad_config[INP_BUTTON_R]=MD_BUTTON_Z;
	md_menu_options.pad_config[INP_BUTTON_VOL_UP]=MD_BUTTON_MODE;
	md_menu_options.pad_config[INP_BUTTON_START]=MD_BUTTON_START;
#endif
#if defined (__GIZ__)
	md_menu_options.pad_config[INP_BUTTON_UP]=MD_BUTTON_UP;
	md_menu_options.pad_config[INP_BUTTON_DOWN]=MD_BUTTON_DOWN;
	md_menu_options.pad_config[INP_BUTTON_LEFT]=MD_BUTTON_LEFT;
	md_menu_options.pad_config[INP_BUTTON_RIGHT]=MD_BUTTON_RIGHT;
	md_menu_options.pad_config[INP_BUTTON_REWIND]=MD_BUTTON_A;
	md_menu_options.pad_config[INP_BUTTON_PLAY]=MD_BUTTON_B;
	md_menu_options.pad_config[INP_BUTTON_FORWARD]=MD_BUTTON_C;
	md_menu_options.pad_config[INP_BUTTON_L]=MD_BUTTON_X;
	md_menu_options.pad_config[INP_BUTTON_STOP]=MD_BUTTON_Y;
	md_menu_options.pad_config[INP_BUTTON_R]=MD_BUTTON_Z;
	md_menu_options.pad_config[INP_BUTTON_BRIGHT]=MD_BUTTON_MODE;
	md_menu_options.pad_config[INP_BUTTON_HOME]=MD_BUTTON_START;
#endif	

	md_menu_options.force_region=0;  // auto region
	md_menu_options.show_fps=1;
	md_menu_options.gamma=0;
	md_menu_options.sound_rate=2;

}
#endif

#if defined (__EMU_SMS__)
static
void SMSDefaultMenuOptions(void)
{
	sms_menu_options.menu_ver=SMS_OPTIONS_VER;
	sms_menu_options.frameskip=0;
	sms_menu_options.sound_on = 1;  // psg and fm on
	sms_menu_options.volume=50; 
	// set default key options
	memset(sms_menu_options.pad_config,0xFF,sizeof(sms_menu_options.pad_config));
#ifdef __GP32__
	sms_menu_options.pad_config[INP_BUTTON_UP]=SMS_BUTTON_UP;
	sms_menu_options.pad_config[INP_BUTTON_DOWN]=SMS_BUTTON_DOWN;
	sms_menu_options.pad_config[INP_BUTTON_LEFT]=SMS_BUTTON_LEFT;
	sms_menu_options.pad_config[INP_BUTTON_RIGHT]=SMS_BUTTON_RIGHT;
	sms_menu_options.pad_config[INP_BUTTON_B]=SMS_BUTTON_1;
	sms_menu_options.pad_config[INP_BUTTON_A]=SMS_BUTTON_2;
	sms_menu_options.pad_config[INP_BUTTON_START]=SMS_BUTTON_START;
	sms_menu_options.cpu_speed=6; // default 133
#endif
#if defined (__GP2X__)
	sms_menu_options.pad_config[INP_BUTTON_UP]=SMS_BUTTON_UP;
	sms_menu_options.pad_config[INP_BUTTON_DOWN]=SMS_BUTTON_DOWN;
	sms_menu_options.pad_config[INP_BUTTON_LEFT]=SMS_BUTTON_LEFT;
	sms_menu_options.pad_config[INP_BUTTON_RIGHT]=SMS_BUTTON_RIGHT;
	sms_menu_options.pad_config[INP_BUTTON_START]=SMS_BUTTON_START;
	sms_menu_options.pad_config[INP_BUTTON_X]=SMS_BUTTON_1;
	sms_menu_options.pad_config[INP_BUTTON_B]=SMS_BUTTON_2;
	sms_menu_options.cpu_speed=13; // default 130
#endif
#if defined (__GIZ__)
	sms_menu_options.pad_config[INP_BUTTON_UP]=SMS_BUTTON_UP;
	sms_menu_options.pad_config[INP_BUTTON_DOWN]=SMS_BUTTON_DOWN;
	sms_menu_options.pad_config[INP_BUTTON_LEFT]=SMS_BUTTON_LEFT;
	sms_menu_options.pad_config[INP_BUTTON_RIGHT]=SMS_BUTTON_RIGHT;
	sms_menu_options.pad_config[INP_BUTTON_STOP]=SMS_BUTTON_START;
	sms_menu_options.pad_config[INP_BUTTON_PLAY]=SMS_BUTTON_1;
	sms_menu_options.pad_config[INP_BUTTON_FORWARD]=SMS_BUTTON_2;
	sms_menu_options.cpu_speed=13; // default 130
#endif
	sms_menu_options.force_region=0;  // auto region
	sms_menu_options.show_fps=1;
	sms_menu_options.gamma=0;
	sms_menu_options.sound_rate=4;
}

static
void GGDefaultMenuOptions(void)
{
	gg_menu_options.menu_ver=SMS_OPTIONS_VER;
	gg_menu_options.frameskip=0;
	gg_menu_options.sound_on = 1;  // psg and fm on
	gg_menu_options.volume=50; 
	// set default key options
	memset(gg_menu_options.pad_config,0xFF,sizeof(gg_menu_options.pad_config));
#ifdef __GP32__
	gg_menu_options.pad_config[INP_BUTTON_UP]=GG_BUTTON_UP;
	gg_menu_options.pad_config[INP_BUTTON_DOWN]=GG_BUTTON_DOWN;
	gg_menu_options.pad_config[INP_BUTTON_LEFT]=GG_BUTTON_LEFT;
	gg_menu_options.pad_config[INP_BUTTON_RIGHT]=GG_BUTTON_RIGHT;
	gg_menu_options.pad_config[INP_BUTTON_B]=GG_BUTTON_1;
	gg_menu_options.pad_config[INP_BUTTON_A]=GG_BUTTON_2;
	gg_menu_options.pad_config[INP_BUTTON_START]=GG_BUTTON_START;
	gg_menu_options.cpu_speed=6; // default 133
#endif
#if defined (__GP2X__)
	gg_menu_options.pad_config[INP_BUTTON_UP]=GG_BUTTON_UP;
	gg_menu_options.pad_config[INP_BUTTON_DOWN]=GG_BUTTON_DOWN;
	gg_menu_options.pad_config[INP_BUTTON_LEFT]=GG_BUTTON_LEFT;
	gg_menu_options.pad_config[INP_BUTTON_RIGHT]=GG_BUTTON_RIGHT;
	gg_menu_options.pad_config[INP_BUTTON_START]=GG_BUTTON_START;
	gg_menu_options.pad_config[INP_BUTTON_X]=GG_BUTTON_1;
	gg_menu_options.pad_config[INP_BUTTON_B]=GG_BUTTON_2;
	gg_menu_options.cpu_speed=13; // default 133
#endif
#if defined (__GIZ__)
	gg_menu_options.pad_config[INP_BUTTON_UP]=GG_BUTTON_UP;
	gg_menu_options.pad_config[INP_BUTTON_DOWN]=GG_BUTTON_DOWN;
	gg_menu_options.pad_config[INP_BUTTON_LEFT]=GG_BUTTON_LEFT;
	gg_menu_options.pad_config[INP_BUTTON_RIGHT]=GG_BUTTON_RIGHT;
	gg_menu_options.pad_config[INP_BUTTON_STOP]=GG_BUTTON_START;
	gg_menu_options.pad_config[INP_BUTTON_PLAY]=GG_BUTTON_1;
	gg_menu_options.pad_config[INP_BUTTON_FORWARD]=GG_BUTTON_2;
	gg_menu_options.cpu_speed=13; // default 133
#endif
	gg_menu_options.force_region=0;  // auto region
	gg_menu_options.show_fps=1;
	gg_menu_options.gamma=0;
	gg_menu_options.sound_rate=4;
}
#endif

int LoadMenuOptions(char *path, char *filename, char *ext, char *optionsmem, int maxsize)
{
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char _filename[MAX_PATH+1];
	char _ext[MAX_PATH+1];
	FILE *stream;
	int size=0;
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Loading...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
					
    SplitFilename(filename, _filename, _ext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,_filename,ext);
	stream=fopen(fullfilename,"rb");
	if(stream)
	{
		// File exists do try to load it
		fseek(stream,0,SEEK_END);
		size=ftell(stream);
		if (size>maxsize) size=maxsize;
		fseek(stream,0,SEEK_SET);
		fread(optionsmem, 1, size, stream);
		fclose(stream);
		return(0);
	}
	else
	{
		return(1);
	}
}

int SaveMenuOptions(char *path, char *filename, char *ext, char *optionsmem, int maxsize)
{
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char _filename[MAX_PATH+1];
	char _ext[MAX_PATH+1];
	FILE *stream;
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Saving...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
	SplitFilename(filename, _filename, _ext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,_filename,ext);
	stream=fopen(fullfilename,"wb");
	if(stream)
	{
		fwrite(optionsmem, 1, maxsize, stream);
		fclose(stream);
		sync();
		return(0);
	}
	else
	{
		return(1);
	}
}

int DeleteMenuOptions(char *path, char *filename, char *ext)
{
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char _filename[MAX_PATH+1];
	char _ext[MAX_PATH+1];
	char text[50];
	PrintBar(prevFlip,240-16);
	sprintf(text,"Deleting...");
	gp_drawString(40,228,strlen(text),text,(unsigned short)RGB(0,0,0),framebuffer16[prevFlip]);
	SplitFilename(filename, _filename, _ext);
	sprintf(fullfilename,"%s%s%s.%s",path,DIR_SEPERATOR,_filename,ext);
	remove(fullfilename);
	sync();
	return(0);
}

#if defined (__EMU_MD__)
static int MDRomLoad()
{
	char filename[MAX_PATH+1];
	int check;
	FILE *stream=NULL;
  
	//Save current rom shortname for save state etc
	strcpy(currentrom_shortname,romlist[currentrom].shortname);
	
	// get full filename
	sprintf(filename,"%s%s%s",RomDir,DIR_SEPERATOR,currentrom_shortname);

	gp_setCpuspeed(MENU_FAST_CPU_SPEED); // give GP32 a little more power to decompress files faster.
  
	if(check_zip(filename))
	{
		PrintTile(Flip);
		PrintTitle(Flip);
		gp_drawString(8,50,19,"Unzipping Rom......",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		memset(RomData,0,ROM_SIZE);  // clear mem ?
		check=load_archive(filename, RomData, &romlist[currentrom].romsize,Flip);
		PrintTile(Flip);
		PrintTitle(Flip);
		if(check)
			gp_drawString(8,50,22,"Unzipping Rom......Ok!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else
	    {
	        gp_drawString(8,50,26,"Unzipping Rom......Failed!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
			MenuFlip();
			MenuPause();
			return(0);
        }
		MenuFlip();
	}
	else
	{
		PrintTile(Flip);
		PrintTitle(Flip);
		gp_drawString(8,50,17,"Loading Rom......",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		memset(RomData,0,ROM_SIZE);  // clear mem ?
		romlist[currentrom].filesize = 0;
		stream=fopen(filename,"rb");
		if (stream)
		{
			fseek(stream,0,SEEK_END);
			romlist[currentrom].filesize = ftell(stream);
			fclose(stream);
		}		
		romlist[currentrom].romsize = romlist[currentrom].filesize;
		stream=fopen(filename,"rb");
		if(stream)
		{
			// File exists do try to load it
			fread(RomData, 1, romlist[currentrom].romsize, stream);
			fclose(stream);
		}
		else
		{
			gp_drawString(8,50,26,"Loading Rom......Failed!",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
			MenuPause();
			return(0);
		}
	}
	 
	if ((romlist[currentrom].romsize&0x3fff)==0x200) 
	{ 
		gp_drawString(8,60,20,"Decoding SMD file...",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
		DecodeSmd(RomData,romlist[currentrom].romsize); 
		romlist[currentrom].romsize-=0x200; 
	} // Decode and byteswap SMD
	else 
	{
		gp_drawString(8,60,20,"Byte Swapping Rom...",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
		Byteswap(RomData,romlist[currentrom].romsize); // Just byteswap Rom
	}
	
	gp_drawString(8,80,20,"Starting Emulation..",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
	reset_drmd();
  
	//auto load default config for this rom if one exists
	if (LoadMenuOptions(md_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options)))
	{
		if (LoadMenuOptions(md_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options)))
		{
			MDDefaultMenuOptions();
		}
	}
		
	// LOAD SRAM
	if (md_menu_options.auto_sram)
	{
		LoadSram(md_system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
	}

	return(1);
}
#endif

#if defined (__EMU_SMS__)
static int SMSRomLoad()
{
	char filename[MAX_PATH+1];
	int check;
	char text[256];
	FILE *stream=NULL;
  
	//Save current rom shortname for save state etc
	strcpy(currentrom_shortname,romlist[currentrom].shortname);
	
	// get full filename
	sprintf(filename,"%s%s%s",RomDir,DIR_SEPERATOR,currentrom_shortname);

	gp_setCpuspeed(MENU_FAST_CPU_SPEED); // give GP32 a little more power to decompress files faster.
  	
	if(check_zip(filename))
	{
		PrintTile(Flip);
		PrintTitle(Flip);
		gp_drawString(8,50,19,"Unzipping Rom......",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		memset(RomData,0,ROM_SIZE);  // clear mem ?
		check=load_archive(filename, RomData, &romlist[currentrom].romsize,Flip);
		PrintTile(Flip);
		PrintTitle(Flip);
		if(check)
			gp_drawString(8,50,22,"Unzipping Rom......Ok!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else
	    {
	        gp_drawString(8,50,26,"Unzipping Rom......Failed!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
			MenuFlip();
			MenuPause();
			return(0);
        }
		MenuFlip();
	}
	else
	{
		PrintTile(Flip);
		PrintTitle(Flip);
		gp_drawString(8,50,17,"Loading Rom......",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		memset(RomData,0,ROM_SIZE);  // clear mem ?
		romlist[currentrom].filesize = 0;
		stream=fopen(filename,"rb");
		if (stream)
		{
			fseek(stream,0,SEEK_END);
			romlist[currentrom].filesize = ftell(stream);
			fclose(stream);
		}		
		romlist[currentrom].romsize = romlist[currentrom].filesize;
		stream=fopen(filename,"rb");
		if(stream)
		{
			// File exists do try to load it
			fread(RomData, 1, romlist[currentrom].romsize, stream);
			fclose(stream);
		}
		else
		{
			gp_drawString(8,50,26,"Loading Rom......Failed!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
			MenuPause();
			return(0);
		}
	}

	gp_drawString(8,80,20,"Starting Emulation..",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	DrSMS_Init();
	DrSMS_Reset();
     
	//auto load default config for this rom if one exists
	if (CurrentEmuMode==EMU_MODE_SMS)
	{
		if(LoadMenuOptions(sms_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options)))
		{
			if(LoadMenuOptions(sms_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options)))
			{
				SMSDefaultMenuOptions();
			}
		}
	}
	else
	{
		if(LoadMenuOptions(gg_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options)))
		{
			if(LoadMenuOptions(gg_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options)))
			{
				GGDefaultMenuOptions();
			}
		}
	}
		// LOAD SRAM
	if (CurrentEmuMode==EMU_MODE_SMS)
	{
		if (sms_menu_options.auto_sram)
		{
			LoadSram(sms_system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
		}
	}
	else
	{
		if (gg_menu_options.auto_sram)
		{
			LoadSram(gg_system_dir, currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
		}
	}

	return(1);
}
#endif

#if defined (__EMU_MD__)
void reset_drmd(void)
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
  memset(&sram,0,0x10000);
  memset(&vram,0,0x10000);
  memset(&cram,0,0x80);
  memset(&zram,0,0x4000);
  memset(&vsram,0,0x40);

  MD_update_sound_timing();
  SN76496_sh_start();
  YM2612Init();
}
void Genesis_Init(void)
{
  switch(md_menu_options.force_region)
	{
		default:
		case 0:  // auto
			Detect_Country_Genesis();
			break;
		case 1: // Usa 60 fps
			Game_Mode = 1;
			CPU_Mode = 0;
			break;
		case 2: // Europe 50 fps
			Game_Mode = 1;
			CPU_Mode = 1;
			break;
		case 3: // Japan 60 fps
			Game_Mode = 0;
			CPU_Mode = 0;
			break;
		case 4: // Japan 50 fps
			Game_Mode = 0;
			CPU_Mode = 1;
			break;
	}

  if(CPU_Mode) // pal
  {
     drmd.vdp_status=1;
     drmd.lines_per_frame = 312;
     drmd.cpl_z80 = Round_Double((((double) MD_CLOCK_PAL / 15.0) / 50.0) / 312.0);
     drmd.cpl_m68k = Round_Double((((double) MD_CLOCK_PAL / 7.0) / 50.0) / 312.0);
     frame_limit=50;
  }
  else // ntsc
  {
     drmd.vdp_status=0;
     drmd.lines_per_frame = 262;
     drmd.cpl_z80 = Round_Double((((double) MD_CLOCK_NTSC / 15.0) / 60.0) / 262.0);
     drmd.cpl_m68k = Round_Double((((double) MD_CLOCK_NTSC / 7.0) / 60.0) / 262.0);
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

  drmd.region=((((Game_Mode)<<1)|(CPU_Mode))<<6)|0x20;  

}

void Detect_Country_Genesis(void)
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
		Game_Mode = gm_tab[0];
		CPU_Mode = cm_tab[0];
	}
	else if (coun & c_tab[1])
	{
		Game_Mode = gm_tab[1];
		CPU_Mode = cm_tab[1];
	}
	else if (coun & c_tab[2])
	{
		Game_Mode = gm_tab[2];
		CPU_Mode = cm_tab[2];
	}
	else if (coun & 2)
	{
		Game_Mode = 0;
		CPU_Mode = 1;
	}
	else
	{
		Game_Mode = 1;
		CPU_Mode = 0;
	}       
}

void InvalidOpCallback(unsigned int opcode)
{
   /*char text[256];
   gp_initFramebuffer(framebuffer16[0],16,60,menu_options.lcdver); 
   gp_clearFramebuffer16(framebuffer16[0],(unsigned short)RGB(0,0,0));
   sprintf(text,"Invalid opcode : %x",opcode);
   gp_drawString(8,8,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[0]);
   while(1==1)
   {
   }*/
}

void illegal_memory_io(unsigned int address, char *error_text)
{
   /*char text[256];
   gp_initFramebuffer(framebuffer16[0],16,60,menu_options.lcdver); 
   gp_clearFramebuffer16(framebuffer16[0],(unsigned short)RGB(0,0,0));
   sprintf(text,"Invalid address io : %x",address);
   gp_drawString(8,8,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[0]);
   gp_drawString(8,16,strlen(error_text),error_text,(unsigned short)RGB(31,31,31),framebuffer16[0]);
   while(1==1)
   {
   }
   */
}

#ifdef EMU_C68K
int SekInterrupt(int irq)
{

  cyclone.irq=(unsigned char)irq;
  return 0;
}

unsigned int DrMDCheckPc(unsigned int pc)
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



void Cyclone_Init()
{
  cyclone.checkpc=DrMDCheckPc;
  cyclone.fetch8 =cyclone.read8 =m68k_read_memory_8;
  cyclone.fetch16=cyclone.read16=m68k_read_memory_16;
  cyclone.fetch32=cyclone.read32=m68k_read_memory_32;
  cyclone.write8 =m68k_write_memory_8;
  cyclone.write16=m68k_write_memory_16;
  cyclone.write32=m68k_write_memory_32;
  cyclone.IrqCallback = (unsigned int)&m68k_irq_callback;

}

void Cyclone_Reset()
{
  cyclone.srh = 0x27;
  cyclone.a[7]=m68k_read_memory_32(0);
  cyclone.membase = 0;
  cyclone.pc=DrMDCheckPc(m68k_read_memory_32(4));
}

void Cyclone_ClearIRQ(int irq)
{
	cyclone.irq=irq;
}
#endif



#ifdef EMU_M68K
int vdp_int_ack_callback(int int_level)
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

void DrMD_Init()
{
	char text[256];
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
#ifdef __GP32__
  drmd.gp32_pal = 0x14A00400;
#endif
#if defined (__GP2X__) || defined (__GIZ__)
  drmd.gp32_pal = &gp2x_palette;
#endif
  drmd.vsram = (unsigned int)vsram;
  drmd.sram = (unsigned int)sram;  
}

void DrMD_Reset()
{
  unsigned int x=0;
  // reset DrMD
  drmd.m68k_aim = 0;
  drmd.m68k_total = 0;
  drmd.zbank = 0;
  drmd.romsize = romlist[currentrom].romsize;
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



void DrMD_DrZ80_Init()
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

void DrMD_DrZ80_Reset()
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

void DrMD_DrZ80_Set_Irq(unsigned int irq)
{
    drz80.z80irqvector = 0xFF;
	drz80.Z80_IRQ = irq;
}

void DrMD_DrZ80_irq_callback(void)
{
    drz80.Z80_IRQ=0;  // lower irq when in accepted
}

#endif

#if defined (__EMU_SMS__) 
void DrSMS_DrZ80_Set_Irq(unsigned int irq)
{
    drz80.z80irqvector = 0xFF;
	drz80.Z80_IRQ = irq;
}

void DrSMS_DrZ80_irq_callback(void)
{
    //drz80.Z80_IRQ=0;  // lower irq when in accepted
}

void DrSMS_Init()
{
  /*
   Need to add code to init drsms depending on PAL or NTSC - NTSC defaulted for now
  */
  if(romlist[currentrom].romsize&0xFFF) drsms.cart_rom = (unsigned int)RomData+0x200;
  else drsms.cart_rom = (unsigned int)RomData;
  drsms.zram = (unsigned int)work_ram;
  drsms.sram = (unsigned int)sram;
  drsms.sram1 = (unsigned int)sram;
  drsms.sram2 = (unsigned int)sram+0x4000;
  drsms.vdp_memory = (unsigned int)vram;
  drsms.vdp_cram = (unsigned int)cram;
  drsms.tile_cache = (unsigned int)tile_cache;
#ifdef __GP32__
  drsms.local_pal = (unsigned int)PALETTE;
#endif
#if defined (__GP2X__) || defined (__GIZ__)
  drsms.local_pal = (unsigned int)&gp2x_palette;
#endif
  drsms.z80_context = (unsigned int)&drz80;
  drsms.z80_run = DrZ80Run;
  drsms.z80_set_irq = DrSMS_DrZ80_Set_Irq;
  drsms.z80_reset = DrSMS_DrZ80_Reset;
  drsms.psg_write = psg_write;

  DrSMS_DrZ80_Init();
}

void DrSMS_Reset()
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
  frame_limit=60;
  if (CurrentEmuMode==EMU_MODE_SMS)
		drsms.gg_mode = 0;
  else
		drsms.gg_mode = 1;
		
  DrSMS_Rebase_Banks();
  
  drsms.banks[0xC]=(unsigned int)work_ram-0xC000;
  drsms.banks[0xD]=(unsigned int)work_ram-0xC000;
  drsms.banks[0xE]=(unsigned int)work_ram-0xE000;
  drsms.banks[0xF]=(unsigned int)work_ram-0xE000;
  
  if (CurrentEmuMode==EMU_MODE_SMS)
	SMS_update_sound_timing();
  else
	GG_update_sound_timing();
	
  SN76496_sh_start();
  
  DrSMS_DrZ80_Reset();
}

void DrSMS_DrZ80_Init()
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

void DrSMS_DrZ80_Reset()
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
#endif

#ifdef __GIZ__
static void RenderFPS(void)
{
	unsigned short *pixFrom=framebuffer16[0];
	unsigned short *pixTo=(unsigned short *)GizPrimaryFrameBuffer;
	int x,y;
	
	for(y=0;y<8;y++)
	{
		for(x=0;x<64;x++)
		{
			*pixTo++=*pixFrom++;
		}
		pixTo+=321-64;
		pixFrom+=320-64;
	}
}
#endif

static int input_disabled=0;
static int DoFrame(unsigned int skip)
{
	int quit=0;
	unsigned int *pix;
	int i=0;
	int p=0;

	InputUpdate(1);
	
	// L+R for menu:
	if(input_disabled)
	{
		// waiting for start button to be released
		if(Inp.held[input_disabled-1]==0) input_disabled=0; // wait for START release
	}
	else
	{
	
#ifdef __GP32__
		if (Inp.held[INP_BUTTON_L]==1 && Inp.held[INP_BUTTON_R]   ) quit=1;
		else if (Inp.held[INP_BUTTON_L]    && Inp.held[INP_BUTTON_R]==1) quit=1;
		else if ((Inp.held[INP_BUTTON_L]) && (Inp.held[INP_BUTTON_START]==1))
		{  // quick load
			if(quick_save_present)
			{
				loadstate_mem(quick_state);
				input_disabled=INP_BUTTON_START+1;
			}
		}
#endif
#if defined (__GP2X__) || defined (__GIZ__)
		if (Inp.held[INP_BUTTON_MENU_ENTER]==1) quit=1;
		else if ((Inp.held[INP_BUTTON_MENU_QUICKLOAD1]) && (Inp.held[INP_BUTTON_MENU_QUICKLOAD2]==1))
		{  // quick load
			if(quick_save_present)
			{
				loadstate_mem(quick_state);
				input_disabled=INP_BUTTON_MENU_QUICKLOAD2+1;
			}
		}
#endif
#ifdef __GP32__
		else if ((Inp.held[INP_BUTTON_R]) && (Inp.held[INP_BUTTON_START]==1))
		{  // quick save
			savestate_mem(quick_state);
			quick_save_present=1;
			input_disabled=INP_BUTTON_START+1;
		}
#endif
#if defined (__GP2X__) || defined (__GIZ__)
		else if ((Inp.held[INP_BUTTON_MENU_QUICKSAVE1]) && (Inp.held[INP_BUTTON_MENU_QUICKSAVE2]==1))
		{  // quick save
			savestate_mem(quick_state);
			quick_save_present=1;
			input_disabled=INP_BUTTON_MENU_QUICKSAVE2+1;
		}
#endif
#if defined (__GP2X__)
		else if (Inp.held[INP_BUTTON_VOL_UP])
		{
			volume[0]+=5;
			if (volume[0] > 100) volume[0] = 100;
			gp2x_sound_volume(volume[0],volume[0]);
			input_disabled=INP_BUTTON_VOL_UP+1;
		}
		else if (Inp.held[INP_BUTTON_VOL_DOWN])
		{
			volume[0]-=5;
			if (volume[0] > 100) volume[0] = 0;
			gp2x_sound_volume(volume[0],volume[0]);
			input_disabled=INP_BUTTON_VOL_DOWN+1;
		}
#endif
		else if((CurrentEmuMode==EMU_MODE_SMS)&&(Inp.held[sms_pause_button]))
		{
			drz80.Z80_NMI=1; // raise NMI interrupt
			input_disabled=sms_pause_button+1;
		}
		else
		{
			for(i=0;i<32;i++)
			{
				if ((Inp.held[i])&&(pad_config[i]!=0xFF)) p|=(1<<pad_config[i]);
			}

			if(!quit) *pad=p;
		}
	}
	
	if (CurrentEmuMode==EMU_MODE_MD)
	{
#if defined (__EMU_MD__) 
		DrMDRun(skip);
#endif
#if defined (__GP2X__) || defined (__GIZ__)
		gp2x_video_setpalette();
#endif
	}
	else // GG emu mode
	{
#if defined (__EMU_SMS__)
		DrSMSRun(skip);
#endif
	}

	if(skip) 
	{
		Frames++;
		if(show_fps)
		{
#ifdef __GP32__
			pix=(unsigned int*)EmuFB[Flip]+58;
			for(i=0;i<(8*8);i++)
			{
				pix[0] = 0x50505050;
				pix[1] = 0x50505050;
				pix+=60;
			}
			gp_drawString(0,0,strlen(fps_display),fps_display,0x51,(unsigned char*)EmuFB[Flip]);
#endif
#if defined (__GP2X__) 
            if ((CurrentEmuMode==EMU_MODE_MD)||(CurrentEmuMode==EMU_MODE_SMS))
			{
				pix=(unsigned int*)EmuFB[Flip];
				for(i=0;i<(8);i++)
				{
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					*pix++ = 0x50505050;
					pix+=64;
				}
				gp_drawString(0,0,strlen(fps_display),fps_display,0x51,(unsigned char*)EmuFB[Flip]);
			}
			else
			{
				pix=(unsigned int*)EmuFB[Flip];
				for(i=0;i<(8);i++)
				{
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					*pix++ = 0x0;
					pix+=128;
				}
				gp_drawString(0,0,strlen(fps_display),fps_display,0xFFFF,(unsigned char*)EmuFB[Flip]);
			}
#endif
#if defined (__GIZ__)
			pix=(unsigned int*)framebuffer16[Flip];
			for(i=0;i<(8);i++)
			{
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				*pix++ = 0x0;
				pix+=128;
			}
			gp_drawString(0,0,strlen(fps_display),fps_display,0xFFFF,(unsigned char*)framebuffer16[Flip]);
#endif
		}
#ifdef __GP32__
		gp_setFramebuffer(EmuFB[Flip],0);
		Flip++;
		if(Flip>2) Flip=0;
#endif
#ifdef __GP2X__
		flushcache();
		gp_setFramebuffer(Flip,0);
		Flip++;
		if(Flip>3) Flip=0;
#endif
#ifdef __GIZ__
		prevFlip=Flip=0;
		// Don't call setFramebuffer because emulation code renders directly to
		// Giz framebuffer.  Just update fps if option is enabled
		if (show_fps) RenderFPS();
#endif
		*frame_buffer = EmuFBcentered[Flip];
	}

	return quit;
}

#ifdef __GP32__
static void TIMER4IRQ(void) __attribute__ ((interrupt ("IRQ")));
static void TIMER4IRQ(void) {
     Timer++;
}

static void start_timer4(int freq) {
   int pclk;
   pclk = gp_getPCLK();
   pclk/= 16;
   pclk/= 256;
   pclk/= freq;  // 51Hz timer, tested 33 66 133 Mhz

   rTCFG0 |= (0xFF<<8);   // Presacler for timer 2,3,4 = 256
   rTCFG1 |= (0x03<<16);  // timer4  1/16
   rTCNTB4 = (long)pclk;
   rTCON  = (0x1<<22) | (0x1<<20); // start timer4, auto reload
   IsrInstall(14,(void*)TIMER4IRQ);
}

static void stop_timer4(void) {

   rTCON = BITCLEAR(rTCON,20); // timer4 off
   IsrUninstall(14,(void*)TIMER4IRQ);

}
#endif

#ifdef __GP32__
static int SegAim()
{
  int aim=curseg;  

  aim--; if (aim<0) aim+=5;

  return aim;
}
#endif
#if defined (__GP2X__)
static int SegAim()
{
  int aim=CurrentSoundBank;  

  aim--; if (aim<0) aim+=8;

  return aim;
}
#endif

#if defined (__GIZ__)
static int SegAim()
{
  int aim=FrameworkAudio_GetCurrentBank();  

  aim--; if (aim<0) aim+=8;

  return aim;
}
#endif

static int RunNoAudio()
{
  int quit=0,ticks=0,now=0,done=0,i=0;
  int tick=0,fps=0;
  unsigned int getTimer=0;
  unsigned int frametime=1000/frame_limit;
  
  Timer=0;
  Frames=0;
  // Setup a 60hz timer for frame timer
  
#ifdef __GP32__
  snd_enabled = 0;
  start_timer4(frame_limit);
#endif

  	  while (quit==0)
	  {
#if defined (__GP2X__) || defined (__GIZ__)
	    getTimer=gp2x_timer_read();
		Timer=getTimer/frametime;
#endif
		if(Timer-tick>frame_limit)
	    {
	       fps=Frames;
	       Frames=0;
	       tick=Timer;
	       sprintf(fps_display,"Fps: %d",fps);
	    }  
	    now=Timer;
	    ticks=now-done;

	    if(ticks<1) continue;
	    if(frameskip==0)
	    {
	       if(ticks>10) ticks=10;
	       for (i=0; i<ticks-1; i++)
	       {
		  quit|=DoFrame(0);
	       } 
	       if(ticks>=1)
	       {
		  quit|=DoFrame(1);
	       }
	    }
	    else
	    {
	       if(ticks>frameskip) ticks=frameskip;
	       for (i=0; i<ticks-1; i++)
	       {
		  quit|=DoFrame(0);
	       } 
 	       if(ticks>=1)
	       {
		  quit|=DoFrame(1);
	       }
	    }
	    
	    done=now;
	  }

#ifdef __GP32__
  stop_timer4();
#endif
  
  return 0;
}

#ifdef __GIZ__
static int RunAudio()
{
  int quit=0,done=0,aim=0,i=0,j=0;
  int tick=0,fps=0,skip=0,efps=0;
  unsigned int frametime=1000/frame_limit,frameTimer=0;
  Timer=0;
  Frames=0;
  skip=frameskip-1;

  gp_initSound((int)sound_rates[sound_rate], 16, 1, frame_limit,0x00020008);
  gp2x_sound_volume(volume[0],volume[0]);
  // Get initial position in the audio loop:
  done=SegAim();
  skip=1;
  while (quit==0)
  {
    
    for (i=0;i<10;i++)
    {
	    Timer=gp2x_timer_read();
	    if(Timer-tick>1000)
		{
	       fps=Frames;
	       Frames=0;
	       tick=Timer;
	       sprintf(fps_display,"Fps: %d",fps);
        }
	
      aim=SegAim();
	  
      if ((done)!=(aim))
      {
		 //frameTimer=gp2x_timer_read()+frametime;
		// for(j=0;j<2;j++)
		 //{
		 //We need to render more audio:  
        soundbuffer=FrameworkAudio_GetAudioBank(done);
        laststage=0;
        current_sample=0;
		last_sample=0;
		dac_sample=0;
        done++; if (done>=8) done=0;
        if(frameskip==0)
		{
			if(done==((aim-1)&7))
			{
				efps^=1;
			}
			else efps=0;
			
			if ((done==aim)||(efps>0)) quit|=DoFrame(1); // Render last frame
			else           quit|=DoFrame(0);
		}
		else
		{
			if (skip) 
			{
		        quit|=DoFrame(0); // Render last frame
		        skip--;
		    }
		    else 
		    {  
		        quit|=DoFrame(1);
				skip=frameskip-1;
		    }
		}	
		if (CurrentEmuMode==EMU_MODE_MD)
		{
#if defined (__EMU_MD__) 
			RefreshFm();
			if(laststage<sound_buffer_size) YM2612UpdateOne(soundbuffer+(laststage<<1),sound_buffer_size-laststage);
#endif
		}
		else
		{
#if defined (__EMU_SMS__)
			if(laststage<sound_buffer_size) RenderSound(soundbuffer+(laststage<<1),sound_buffer_size-laststage);
#endif
		}
		//if (gp2x_timer_read() > frameTimer+(frametime>>4))
		//	skip=0;
		//else
		//{
			/*while(gp2x_timer_read() < frameTimer)
			{
			}*/
		//	skip=1;
		//}
		//	frameTimer+=frametime;
		//}
		
	}
      if (done==aim) break; // Up to date now
    }

    done=aim; // Make sure up to date
  }
  
  gp_stopSound();
  return 0;
}
#endif

#if defined (__GP32__) || defined (__GP2X__)
static int RunAudio()
{
  int quit=0,done=0,aim=0,i=0;
  int tick=0,fps=0,skip=0;

  Timer=0;
  Frames=0;
  skip=frameskip-1;
#ifdef __GP32__
  Gp32_AudioStart(0);
#endif
#if defined (__GP2X__)
	// Init sound, adjust fragments occording to cpu speed because low cpu
	// speed require smaller fragments in order to get clear sound because
	// sound thread checks the sound buffer less othen
	if(cpu_speed_lookup[cpu_speed] < 100)
		gp_initSound((int)sound_rates[sound_rate], 16, 1, frame_limit,0x00020008);
	else if(cpu_speed_lookup[cpu_speed] < 150)
		gp_initSound((int)sound_rates[sound_rate], 16, 1, frame_limit,0x00020009);
	else
		gp_initSound((int)sound_rates[sound_rate], 16, 1, frame_limit,0x0002000F);	
		
	SoundThreadFlag = SOUND_THREAD_SOUND_ON;
	gp2x_sound_volume(volume[0],volume[0]);
#endif
  // Get initial position in the audio loop:
  done=SegAim();

  while (quit==0)
  {
    
    for (i=0;i<10;i++)
    {
	    if(Timer-tick>frame_limit)
      {
	       fps=Frames;
	       Frames=0;
	       tick=Timer;
	       sprintf(fps_display,"Fps: %d",fps);
      }
	    
      aim=SegAim();
      if (done!=aim)
      {
         //We need to render more audio:  
        soundbuffer=pOutput[done];
        laststage=0;
        current_sample=0;
		last_sample=0;
		dac_sample=0;
#ifdef __GP32__
        done++; if (done>=5) done=0;
#endif
#if defined (__GP2X__) || defined (__GIZ__)
        done++; if (done>=8) done=0;
#endif
        if(frameskip==0)
		{
			if (done==aim) quit|=DoFrame(1); // Render last frame
			else           quit|=DoFrame(0);
		}
		else
		{
			if (skip) 
			{
		        quit|=DoFrame(0); // Render last frame
		        skip--;
		    }
		    else 
		    {  
		        quit|=DoFrame(1);
				skip=frameskip-1;
		    }
		}	
		if (CurrentEmuMode==EMU_MODE_MD)
		{
#if defined (__EMU_MD__)
			RefreshFm();
			if(laststage<sound_buffer_size) YM2612UpdateOne(soundbuffer+(laststage<<1),sound_buffer_size-laststage);
#endif
		}
		else
		{
#if defined (__EMU_SMS__) 
			if(laststage<sound_buffer_size) RenderSound(soundbuffer+(laststage<<1),sound_buffer_size-laststage);
#endif
		}
	}
      if (done==aim) break; // Up to date now
    }

    done=aim; // Make sure up to date
  }
  
#ifdef __GP32__
  Gp32_AudioStop();
#endif
#if defined (__GP2X__) || defined (__GIZ__)
  gp_stopSound();
#endif
  return 0;
}
#endif
char **g_argv;
int main(int argc, char *argv[])
{
	int i=0,j=0,action=0;
	char text[128];
	unsigned short *pix;
	unsigned short *pixdata;
	unsigned int *pal;
	unsigned int curr_pix=0;
	unsigned short pixel=0;
	unsigned char R,G,B;
	
	g_argv = argv;
	
#if defined (__GP32__)
	//gp_setMMU(FRAMEBUFFER1,FRAMEBUFFER2+0x26000,0xFFE);
	//gp_setMMU(SAMPLEBUFFER0,SAMPLEBUFFER3+0x800,0xFFE);
#endif
		
	Timer=Flip=0;
#ifdef __GP32__
	//setup pointers to default frame buffers - 3 for triple buffering in 8 bit mode
	framebuffer8[0]=(unsigned char*)0x0C7B4780;
	framebuffer8[1]=(unsigned char*)0x0C7C7B00;
	framebuffer8[2]=(unsigned char*)0x0C7DAE80;
	
	// 16bit frame buffer - used for menus
	framebuffer16[0]=(unsigned short*)FRAMEBUFFER1;
	framebuffer16[1]=(unsigned short*)FRAMEBUFFER2;
	
	currentrom_shortname[0]=0; //ensure current rom name is blank
	Flip=0;
	gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
	gp_initFramebuffer(framebuffer16[Flip],16,60,gp32_lcdver); 
	
	prevFlip=Flip;
	Flip=1;
#endif
#if defined(__GP2X__) || defined(__GIZ__)
	gp_initGraphics(16,Flip);
#endif	

	gp_setCpuspeed(MENU_CPU_SPEED);
	update_menu_graphics_gamma();
	
	//Set system directory
#ifdef __GP32__
	sprintf(system_dir,			SYSTEM_DIR);
	sprintf(system_root_dir,	SYSTEM_DIR);
	sprintf(md_system_dir,		 MD_SYSTEM_DIR);
	sprintf(sms_system_dir,	SMS_SYSTEM_DIR);
	sprintf(gg_system_dir,		GG_SYSTEM_DIR);
	strcpy(MD_RomDir,ROM_PATH_MD);
	strcpy(GG_RomDir,ROM_PATH_GG);
	strcpy(SMS_RomDir,ROM_PATH_SMS);
	strcpy(RomDir,MD_RomDir);
#endif
#if defined(__GP2X__)
	getwd(system_dir);
	strcpy(system_root_dir,system_dir);
	sprintf(md_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,MD_SYSTEM_DIR);
	sprintf(sms_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,SMS_SYSTEM_DIR);
	sprintf(gg_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,GG_SYSTEM_DIR);
#endif
#if defined(__GIZ__)
	strcpy(system_dir,SYSTEM_DIR);
	strcpy(system_root_dir,system_dir);
	sprintf(md_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,MD_SYSTEM_DIR);
	sprintf(sms_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,SMS_SYSTEM_DIR);
	sprintf(gg_system_dir,"%s%s%s",system_root_dir,DIR_SEPERATOR,GG_SYSTEM_DIR);
#endif
	// allocate massive buffer that rom data will be loaded to
	// ROMSIZE is defined in app.h
	
	
	RomData=(unsigned char *)malloc(ROM_SIZE);
	
	//Find a nice even number that is bigger than the size
	//of a save state.  Use this number to malloc space to
	//hold savestates as using sizeof struct seems to break it
	//probably some sort of alignment problem.
	
	i=0;
	while(i<=(sizeof(struct MD_SAVESTATE_V6)))
	{
		i+=0x1000;
	}

	quick_state=(char *)malloc(i); // add some more
	temp_state=(char *)malloc(i);
	current_state=(char *)malloc(i);

	savestatesize = i; 
		
	InputInit();  // clear input context

#if defined (__GP2X__) || defined (__GIZ__)	
	for(curr_pix=0;curr_pix<(320*240);curr_pix++)
	{
      // md  0000 bbb0 ggg0 rrr0
      // gp  rrrr rggg ggbb bbbi
      pixel=gbax[curr_pix];
      R=(pixel>>11)&0x1F; // 0000 0RRR - 3 bits Red
      G=(pixel>>6)&0x1F; 
      B=(pixel>>1)&0x1F;
      
      // Do gamma correction  
      R=gamma_conv[R+(0<<5)];
      G=gamma_conv[G+(0<<5)];
      B=gamma_conv[B+(0<<5)];

      pixel=RGB(R,G,B);
      gbax[curr_pix]=pixel;
   
	}
#endif
	
	//load gbax splash screen data into frame buffer
	pixdata=(unsigned short*)gbax;
	pix=framebuffer16[Flip]; 
	for(i=0;i<240;i++)
	{
		for(j=0;j<320;j++)
		{
			*pix++ = *pixdata++;
		}
	}
	MenuFlip();
	MenuPause();
	
	gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
   
	// setup DrMD dir in GPMM
	sprintf(text,"Checking Directories");
	gp_drawString(8,20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	MenuFlip();

	//Make sure required directories exist
#ifdef __GP32__
	i=smc_createdir(system_root_dir);
	if(!i)
#endif
#if defined (__GP2X__)
	i=mkdir(system_root_dir);
	sync();
	if((i)&&(errno!=EEXIST))
#endif
#if defined (__GIZ__)
	i=mkdir(system_root_dir);
	if((i)&&(errno!=EEXIST))
#endif
	{
		sprintf(text,"%s...Failed",system_root_dir);
		gp_drawString(8,30,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		while(1==1)
		{
		}
	}  
	sprintf(text,"%s...OK",system_root_dir);
	gp_drawString(8,30,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	
#ifdef __GP32__
	i=smc_createdir(md_system_dir);
	if(!i)
#endif
#if defined (__GP2X__) 
	i=mkdir(md_system_dir);
	sync();
	if((i)&&(errno!=EEXIST))
#endif
#if defined (__GIZ__)
	i=mkdir(md_system_dir);
	if((i)&&(errno!=EEXIST))
#endif
	{
		sprintf(text,"%s...Failed",md_system_dir);
		gp_drawString(8,40,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		while(1==1)
		{
		}
	}
	sprintf(text,"%s...OK",md_system_dir);
	gp_drawString(8,40,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	
#ifdef __GP32__
	i=smc_createdir(sms_system_dir);
	if(!i)
#endif
#if defined (__GP2X__) 
	i=mkdir(sms_system_dir);
	sync();
	if((i)&&(errno!=EEXIST))
#endif
#if defined (__GIZ__)
	i=mkdir(sms_system_dir);
	if((i)&&(errno!=EEXIST))
#endif
	{
		sprintf(text,"%s...Failed",sms_system_dir);
		gp_drawString(8,40,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		while(1==1)
		{
		}
	}
	sprintf(text,"%s...OK",sms_system_dir);
	gp_drawString(8,40,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);

#ifdef __GP32__
	i=smc_createdir(gg_system_dir);
	if(!i)
#endif
#if defined (__GP2X__)
	i=mkdir(gg_system_dir);
	sync();
	if((i)&&(errno!=EEXIST))
#endif
#if defined (__GIZ__)
	i=mkdir(gg_system_dir);
	if((i)&&(errno!=EEXIST))
#endif
	{
		sprintf(text,"%s...Failed",gg_system_dir);
		gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		MenuFlip();
		while(1==1)
		{
		}
	}
	sprintf(text,"%s...OK",gg_system_dir);
	gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);

#if defined (__EMU_MD__) 
	// Load options
	if (LoadMenuOptions(md_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT,(char*)&md_menu_options, sizeof(md_menu_options)))
	{
		MDDefaultMenuOptions();
	}
#endif
#if defined (__EMU_SMS__)
	if (LoadMenuOptions(sms_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT,(char*)&sms_menu_options, sizeof(sms_menu_options)))
	{
		SMSDefaultMenuOptions();
	}
	if (LoadMenuOptions(gg_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT,(char*)&gg_menu_options, sizeof(gg_menu_options)))
	{
		GGDefaultMenuOptions();
	}
#endif


#if defined (__EMU_MD__) 
	// Load default rom directories
	if (LoadMenuOptions(md_system_dir,DEFAULT_ROM_DIR_FILENAME,DEFAULT_ROM_DIR_EXT,(char*)MD_RomDir, MAX_PATH))
	{
#if defined (__GP2X__) || defined (__GIZ__)	
		strcpy(MD_RomDir,system_dir);
#endif
	}
#endif

#if defined (__EMU_SMS__)
	if (LoadMenuOptions(sms_system_dir,DEFAULT_ROM_DIR_FILENAME,DEFAULT_ROM_DIR_EXT,(char*)SMS_RomDir, MAX_PATH))
	{
#if defined (__GP2X__) || defined (__GIZ__)	
		strcpy(SMS_RomDir,system_dir);
#endif
	}
	if (LoadMenuOptions(gg_system_dir,DEFAULT_ROM_DIR_FILENAME,DEFAULT_ROM_DIR_EXT,(char*)GG_RomDir, MAX_PATH))
	{
#if defined (__GP2X__) || defined (__GIZ__)	
		strcpy(GG_RomDir,system_dir);
#endif
	}
#endif

#ifdef __GP32__
	// black pal - for use in 8bit rendering mode
	pal=(unsigned int*)0x14A00400; 
	for(i=0;i<0x100;i++)
		*pal++ =0;
	pal=(unsigned int*)0x14A00400;
	pal[0x51]=(unsigned short)RGB(31,31,31); // set white color for font
	pal[0xB1]=(unsigned short)RGB(31,31,31);
#endif	
#if defined (__GP2X__) || defined (__GIZ__)
	pal=(unsigned int*)gp2x_palette;
	memset((unsigned char*)&gp2x_palette,0,sizeof(gp2x_palette));
	pal[0x51]=0xFFFFFFFF;
	gp2x_video_setpalette();
#endif

#ifdef EMU_C68K
	memset(&cyclone,0,sizeof(cyclone));
#endif
	memset(&drz80,0,sizeof(drz80));
#if defined (__EMU_MD__)
	memset(&drmd,0,sizeof(drmd));
#endif
#if defined (__EMU_SMS__)
	memset(&drsms,0,sizeof(drsms));
#endif

	//main loop
	for (;;)
	{
		//action=RomList();
		action=MainMenu(action);
		if (action==EVENT_EXIT_APP) break;
#if defined (__EMU_MD__)	
		if (action==EVENT_LOAD_MD_ROM) 
		{
			CurrentEmuMode = EMU_MODE_MD;
			romloaded=MDRomLoad();
			if(romloaded)  	
			{
				action=EVENT_RUN_MD_ROM;   // rom loaded okay so continue with emulation
				strcpy(system_dir,md_system_dir);
				quick_save_present=0;
			}
			else        			action=0;   // rom failed to load so return to menu
		}
#endif
#if defined (__EMU_SMS__)
		if (action==EVENT_LOAD_SMS_ROM) 
		{
			CurrentEmuMode = EMU_MODE_SMS;
			romloaded=SMSRomLoad();
			if(romloaded)  	
			{
				action=EVENT_RUN_SMS_ROM;   // rom loaded okay so continue with emulation
				strcpy(system_dir,sms_system_dir);
				quick_save_present=0;
			}
			else        			action=0;   // rom failed to load so return to menu
		}
		
		if (action==EVENT_LOAD_GG_ROM) 
		{
			CurrentEmuMode = EMU_MODE_GG;
			romloaded=SMSRomLoad();
			if(romloaded)  	
			{
				action=EVENT_RUN_GG_ROM;   // rom loaded okay so continue with emulation
				strcpy(system_dir,gg_system_dir);
				quick_save_present=0;
			}
			else        			action=0;   // rom failed to load so return to menu
		}
#endif
#if defined (__EMU_MD__)
		if ((action==EVENT_RUN_MD_ROM)&&romloaded)
		{
			memcpy(pad_config,md_menu_options.pad_config,sizeof(md_menu_options.pad_config));
			show_fps=md_menu_options.show_fps;
			frameskip=md_menu_options.frameskip;
			sound_rate=md_menu_options.sound_rate;
			cpu_speed=md_menu_options.cpu_speed;
			sound_on=md_menu_options.sound_on;
			gp_setCpuspeed(cpu_speed_lookup[md_menu_options.cpu_speed]);
			sprintf(fps_display,"");
			drmd.render_line = md_render_8;
			drmd.pad_1_type = md_menu_options.pad_type;

#if defined (__GP32__)			
			clear_screen8((unsigned int*)framebuffer8[0],0x50505050);
			clear_screen8((unsigned int*)framebuffer8[1],0x50505050);
			clear_screen8((unsigned int*)framebuffer8[2],0x50505050);
#endif
#if defined (__GP2X__)			
			clear_screen8((unsigned int*)framebuffer8[0],0x50505050);
			clear_screen8((unsigned int*)framebuffer8[1],0x50505050);
			clear_screen8((unsigned int*)framebuffer8[2],0x50505050);
			clear_screen8((unsigned int*)framebuffer8[3],0x50505050);
#endif
#if defined (__GIZ__)			
			clear_screen16((unsigned int*)framebuffer16[0],0x0);
#endif	

#ifdef __GP32__
			gp_initFramebuffer(framebuffer8[0],8,60,gp32_lcdver);
#endif

#if defined (__GIZ__)
			gp_initGraphics(16,Flip);
			MenuFlip();
			volume=(char*)&md_menu_options.volume;
#endif
#if defined (__GP2X__) 
			gp_initGraphics(8,Flip);
			volume=(char*)&md_menu_options.volume;
#endif
			for(i=0;i<4;i++)
			{
				EmuFB[i] = framebuffer8[i];
				EmuFBcentered[i] = framebuffer8[i];
			}
			drmd.frame_buffer=(unsigned int)EmuFBcentered[0];
			frame_buffer = (unsigned int*)&drmd.frame_buffer;
			pad = (unsigned int*)&drmd.pad;
				
			MD_load_pal();
			update_md_pal();
			MD_update_sound_timing(); // update fm tables incase of sound rate change
            if(md_menu_options.sound_on) RunAudio();
			else RunNoAudio();
		}
#endif
#if defined (__EMU_SMS__) 		
		if ((action==EVENT_RUN_SMS_ROM)&&romloaded)
		{
			memcpy(pad_config,sms_menu_options.pad_config,sizeof(sms_menu_options.pad_config));
			for(i=0;i<32;i++)
			{
				if(pad_config[i]==SMS_BUTTON_START)
				{
					sms_pause_button = i;
				}
			}
			show_fps=sms_menu_options.show_fps;
			frameskip=sms_menu_options.frameskip;
			sound_rate=sms_menu_options.sound_rate;
			cpu_speed=sms_menu_options.cpu_speed;
			sound_on=sms_menu_options.sound_on;
			if(sms_menu_options.cpu_speed>MAX_CPU) sms_menu_options.cpu_speed=MAX_CPU;
			gp_setCpuspeed(cpu_speed_lookup[sms_menu_options.cpu_speed]);
			drsms.render_line = sms_render_8;
			sprintf(fps_display,"");
#if defined (__GP32__)			
			clear_screen16((unsigned int*)framebuffer8[0],0x50505050);
			clear_screen16((unsigned int*)framebuffer8[1],0x50505050);
			clear_screen16((unsigned int*)framebuffer8[2],0x50505050);
#endif
#if defined (__GP2X__)			
			clear_screen16((unsigned int*)framebuffer8[0],0x50505050);
			clear_screen16((unsigned int*)framebuffer8[1],0x50505050);
			clear_screen16((unsigned int*)framebuffer8[2],0x50505050);
			clear_screen16((unsigned int*)framebuffer8[3],0x50505050);
#endif
#if defined (__GIZ__)			
			clear_screen16((unsigned int*)framebuffer8[0],0x0);
			clear_screen16((unsigned int*)framebuffer16[0],0x0);
#endif	
#ifdef __GP32__
			gp_initFramebuffer(framebuffer8[0],8,60,gp32_lcdver);
			for(i=0;i<4;i++)
			{
				EmuFB[i] = framebuffer8[i];
				EmuFBcentered[i] = framebuffer8[i];
			}
#endif
#if defined (__GIZ__)
			gp_initGraphics(16,Flip);
			MenuFlip();
			volume=(char*)&sms_menu_options.volume;
#endif
#if defined (__GP2X__)
			gp_initGraphics(8,Flip);
			volume=(char*)&sms_menu_options.volume;
#endif
#if defined (__GP2X__) || defined (__GIZ__)
			if(sms_menu_options.render_mode==0)
			{
				for(i=0;i<4;i++)
				{
					EmuFB[i] = framebuffer8[i];
					EmuFBcentered[i] = framebuffer8[i]+36+(320*24);
				}
				gp2x_video_RGB_setscaling(320,240);
			}
			else
			{
				for(i=0;i<4;i++)
				{
					EmuFB[i] = framebuffer8[i];
					EmuFBcentered[i] = framebuffer8[i];
				}
				gp2x_video_RGB_setscaling(240,192);
			}
#endif
			drsms.frame_buffer=(unsigned int)EmuFBcentered[0];
			frame_buffer = (unsigned int*)&drsms.frame_buffer;
			pad = (unsigned int*)&drsms.pad;
			SMS_load_pal();
			SMS_update_sound_timing(); // update fm tables incase of sound rate change

			if(sms_menu_options.sound_on) RunAudio();
			else RunNoAudio();

		}
		
		if ((action==EVENT_RUN_GG_ROM)&&romloaded)
		{
			memcpy(pad_config,gg_menu_options.pad_config,sizeof(gg_menu_options.pad_config));
			show_fps=gg_menu_options.show_fps;
			frameskip=gg_menu_options.frameskip;
			sound_rate=gg_menu_options.sound_rate;
			cpu_speed=gg_menu_options.cpu_speed;
			sound_on=gg_menu_options.sound_on;
			if(gg_menu_options.cpu_speed>MAX_CPU) gg_menu_options.cpu_speed=MAX_CPU;
			gp_setCpuspeed(cpu_speed_lookup[gg_menu_options.cpu_speed]);
			
			sprintf(fps_display,"");
#if defined (__GP32__)			
			clear_screen16((unsigned int*)framebuffer8[0],0x0);
			clear_screen16((unsigned int*)framebuffer8[1],0x0);
			clear_screen16((unsigned int*)framebuffer8[2],0x0);
#endif
#if defined (__GP2X__)			
			clear_screen16((unsigned int*)framebuffer16[0],0x0);
			clear_screen16((unsigned int*)framebuffer16[1],0x0);
			clear_screen16((unsigned int*)framebuffer16[2],0x0);
			clear_screen16((unsigned int*)framebuffer16[3],0x0);
#endif
#if defined (__GIZ__)			
			clear_screen16((unsigned int*)framebuffer16[0],0x0);
#endif		
#ifdef __GP32__
			gp_initFramebuffer(framebuffer8[0],8,60,gp32_lcdver);
			drsms.render_line = gg_render_8;
			for(i=0;i<4;i++)
			{
				EmuFB[i] = framebuffer8[i];
				EmuFBcentered[i] = framebuffer8[i];
			}
#endif
#if defined (__GP2X__) || defined (__GIZ__)
			gp_initGraphics(16,Flip);
			MenuFlip();
			drsms.render_line = gg_render_8;
			if(gg_menu_options.render_mode==0)
			{
				for(i=0;i<4;i++)
				{
					EmuFB[i] = framebuffer8[i];
					EmuFBcentered[i] = framebuffer8[i]+80+(320*32);
				}
				gp2x_video_RGB_setscaling(320,240);
			}
			else
			{
				for(i=0;i<4;i++)
				{
					EmuFB[i] = framebuffer8[i];
					EmuFBcentered[i] = framebuffer8[i];
				}
				gp2x_video_RGB_setscaling(160,144);
			}
			volume=(char*)&gg_menu_options.volume;
#endif
			drsms.frame_buffer=(unsigned int)EmuFBcentered[0];
			frame_buffer = (unsigned int*)&drsms.frame_buffer;
			pad = (unsigned int*)&drsms.pad;
			
			GG_load_pal();
			GG_update_sound_timing(); // update fm tables incase of sound rate change
            
			if(gg_menu_options.sound_on) RunAudio();
			else RunNoAudio();
		}
#endif
	}
	
	free(current_state);
	free(temp_state);
	free(quick_state);
	free(RomData);
	gp_Reset();
	
	return(0);
}




