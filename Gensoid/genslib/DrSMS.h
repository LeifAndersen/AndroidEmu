// DrSMS - Header File

#ifdef __cplusplus
extern "C" {
#endif

struct DrSMS
{ 
  unsigned int banks[0x10];												// 0x0
  unsigned int cart_rom;													// 0x10
  unsigned int zram;															//
  unsigned int sram;															//
  unsigned int sram1;															//
  unsigned int sram2;															//
  unsigned int vdp_memory;													//
  unsigned int vdp_cram;													//
  unsigned int tile_cache;													//
  unsigned int render_line;												//
  unsigned int frame_buffer;												//
  unsigned int local_pal;													//
  unsigned int z80_context;                               				// 0x14 
  void (*z80_run)(struct DrZ80 *pcy,unsigned int cyc);    	// 0x18
  void (*z80_set_irq)(unsigned int irq);                  			// 0x1C
  void (*z80_reset)();														//
  unsigned int psg_write;													//
  
  unsigned short vdp_line;													//
  unsigned short lines_per_frame;										//
  unsigned short vdp_addr;													//
  unsigned short map_addr;													//
  unsigned short sprite_addr;											//
  unsigned short sprite_tiles;											//
  
  unsigned char vdp_reg0;													//
  unsigned char vdp_reg1;													//
  unsigned char vdp_reg2;													//
  unsigned char vdp_reg3;													//
  
  unsigned char vdp_reg4;													//
  unsigned char vdp_reg5;													//
  unsigned char vdp_reg6;													//
  unsigned char vdp_reg7;													//
  
  unsigned char vdp_reg8;													//
  unsigned char vdp_reg9;													//
  unsigned char vdp_regA;													//
  unsigned char vdp_regB;													//
  
  unsigned char vdp_regC;													//
  unsigned char vdp_regD;													//
  unsigned char vdp_regE;													//
  unsigned char vdp_regF;													//
  
  unsigned char vdp_status;												//
  unsigned char vdp_left;													//
  unsigned char cram_gg_latch;											//
  unsigned char vdp_pending;												//
  
  unsigned char vdp_type;													//
  unsigned char first_byte;												//
  unsigned char vdp_buffer;												//
  unsigned char max_rom_pages;											//
  
  unsigned char memory_reg_C;											//
  unsigned char memory_reg_D;											//
  unsigned char memory_reg_E;											//
  unsigned char memory_reg_F;											//
  
  unsigned char drsms_country;											//
  unsigned char sms_port0;													//
  unsigned char sms_port1;													//
  unsigned char sms_port5;													//
  
  unsigned char sms_port63;												//
  unsigned char gg_mode;													//
  unsigned char spare1;														//
  unsigned char spare2;														//
  unsigned int pad;															//
};  

extern void init_sms_pal(unsigned short *pal);
extern void update_tile_cache(void);
extern void DrSMSRun(unsigned int skip);
extern unsigned int DrSMS_Z80_Rebase_PC(unsigned int z80pc);
extern unsigned int DrSMS_Z80_Rebase_SP(unsigned int z80sp);
extern void DrSMS_Rebase_Banks(void);
extern void DrSMS_Z80_write_8(unsigned char d,unsigned short a);
extern void DrSMS_Z80_write_16(unsigned short d,unsigned short a);
extern unsigned char DrSMS_Z80_read_8(unsigned short a);
extern unsigned short DrSMS_Z80_read_16(unsigned short a);
extern unsigned char DrSMS_Z80_In(unsigned char p);
extern void DrSMS_Z80_Out(unsigned char p,unsigned char d);
extern void RenderSound(short *buffer, unsigned int length);

extern void sms_render_8(void);
extern void gg_render_8(void);

extern void sms_render_8_full(void);

#ifdef __cplusplus
} // End of extern "C"
#endif

