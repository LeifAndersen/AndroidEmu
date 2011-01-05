// DrMD - Header File

#ifdef __cplusplus
extern "C" {
#endif

struct DrMD
{ 
  // total mem = 76+8+60 = 
  // word variables 19x4 = 76
  unsigned int m68k_context;                              // 0x00 pointer to M68K emulator
  void (*m68k_run)(struct Cyclone *pcy);                  // 0x04 pointer to Mk68k run functoin
  int (*m68k_set_irq)(int irq);                           // 0x08
  unsigned int m68k_aim;                                  // 0x0C
  unsigned int m68k_total;                                // 0x10
  unsigned int z80_context;                               // 0x14 
  void (*z80_run)(struct DrZ80 *pcy,unsigned int cyc);    // 0x18
  void (*z80_set_irq)(unsigned int irq);                  // 0x1C
  void (*z80_reset)();                                    // 0x20
  void (*render_line)();                                  // 0x24
  unsigned int frame_buffer;                              // 0x28
  void (*fm_write)(int a, UINT8 v);                       // 0x2C
  int (*fm_read)(int address);                            // 0x30
  void (*psg_write)(int data);                            // 0x34
  unsigned int cart_rom;                                  // 0x38
  unsigned int work_ram;                                  // 0x3C
  unsigned int zram;                                      // 0x40
  unsigned int vram;                                      // 0x44
  unsigned int cram;                                      // 0x48
  unsigned int gp32_pal;                                  // 0x4C
  unsigned int vsram;                                     // 0x50
  unsigned int zbank;                                     // 0x54
  unsigned int romsize;                                   // 0x58
  unsigned short current_sample;                          // 0x5C
  unsigned short sample_count;                            // 0x5E
  unsigned short lines_per_frame;                         // 0x60
  unsigned short vdp_line;                                // 0x62
  unsigned short vdp_status;                              // 0x64
  unsigned short vdp_addr;                                // 0x66
  unsigned short vdp_addr_latch;                          // 0x68
  unsigned short gamma;                                   // 0x6A
  unsigned short cpl_m68k;                                // 0x6C
  unsigned short cpl_z80;                                 // 0x6E
  // byte variables  60 0x9B
  unsigned char spare70;                                      // 0x70
  unsigned char padselect;                                // 0x71
  unsigned char zbusreq;                                  // 0x72
  unsigned char zbusack;                                  // 0x73
  unsigned char zreset;                                   // 0x74
  unsigned char vdp_reg0;                                 // 0x75
  unsigned char vdp_reg1;                                 // 0x76
  unsigned char vdp_reg2;                                 // 0x77
  unsigned char vdp_reg3;                                 // 0x78
  unsigned char vdp_reg4;                                 // 0x79
  unsigned char vdp_reg5;                                 // 0x7A
  unsigned char vdp_reg6;                                 // 0x7B
  unsigned char vdp_reg7;                                 // 0x7C
  unsigned char vdp_reg8;                                 // 0x7D
  unsigned char vdp_reg9;                                 // 0x7E
  unsigned char vdp_reg10;                                // 0x7F
  unsigned char vdp_reg11;                                // 0x80
  unsigned char vdp_reg12;                                // 0x81
  unsigned char vdp_reg13;                                // 0x82
  unsigned char vdp_reg14;                                // 0x83
  unsigned char vdp_reg15;                                // 0x84
  unsigned char vdp_reg16;                                // 0x85
  unsigned char vdp_reg17;                                // 0x86
  unsigned char vdp_reg18;                                // 0x87
  unsigned char vdp_reg19;                                // 0x88
  unsigned char vdp_reg20;                                // 0x89
  unsigned char vdp_reg21;                                // 0x8A
  unsigned char vdp_reg22;                                // 0x8B
  unsigned char vdp_reg23;                                // 0x8C
  unsigned char vdp_reg24;                                // 0x8D
  unsigned char vdp_reg25;                                // 0x8E
  unsigned char vdp_reg26;                                // 0x8F
  unsigned char vdp_reg27;                                // 0x90
  unsigned char vdp_reg28;                                // 0x91
  unsigned char vdp_reg29;                                // 0x92
  unsigned char vdp_reg30;                                // 0x93
  unsigned char vdp_reg31;                                // 0x94
  unsigned char vdp_reg32;                                // 0x95
  unsigned char vdp_counter;                              // 0x96
  unsigned char hint_pending;                             // 0x97
  unsigned char vint_pending;                             // 0x98
  unsigned char vdp_pending;                              // 0x99
  unsigned char vdp_code;                                 // 0x9A
  unsigned char vdp_dma_fill;                             // 0x9B
  unsigned char pad_1_status;                             // 0x9C
  unsigned char pad_1_com;                                // 0x9D
  unsigned char pad_2_status;                             // 0x9E
  unsigned char pad_2_com;                                // 0x9F
  unsigned int sram_start;                                // 0xA0
  unsigned int sram_end;                                  // 0xA4
  unsigned int sram;                                      // 0xA8
  unsigned char region;                                   // 0xAC
  unsigned char sram_flags;                               // 0xAD
  unsigned char pad_1_type;                                  // 0xAE
  unsigned char pad_2_type;                                  // 0xAF
  float cpl_fm;                                           // 0xB0
  unsigned char genesis_rom_banks[0x10];                  // 0xB4
  unsigned int pad;                                   // 0xC4
  unsigned char pad_1_counter;                                   // 0xC8
  unsigned char pad_1_delay;
  unsigned char pad_2_counter;
  unsigned char pad_2_delay; 
  unsigned int spareCC;                                   // 0xCC
  unsigned int spareD0;                                   // 0xD0
  unsigned int spareD4;                                   // 0xD4
  unsigned int spareD8;                                   // 0xD8
  unsigned int spareDC;                                   // 0xDC
  unsigned int spareE0;                                   // 0xE0
  unsigned int spareE4;                                   // 0xE4
  unsigned int spareE8;                                   // 0xE8
  unsigned int spareEC;                                   // 0xEC
  unsigned int spareF0;                                   // 0xF0
  unsigned int spareF4;                                   // 0xF4
  unsigned int spareF8;                                   // 0xF8
  unsigned int spareFC;                                   // 0xFC
  unsigned int spare100;                                  // 0x100
};  
extern void update_md_pal(void);
extern void m68k_irq_callback(void);
extern void genesis_rom_banks_reset(void);
extern void genesis_rom_bank_set(unsigned char bank_target,unsigned char bank_source);

extern void DrMDRun(unsigned int skip);
extern unsigned int m68k_read_memory_8(unsigned int address);
extern unsigned int m68k_read_memory_16(unsigned int address);
extern unsigned int m68k_read_memory_32(unsigned int address);
extern void m68k_write_memory_8(unsigned int address, unsigned int value);
extern void m68k_write_memory_16(unsigned int address, unsigned int value);
extern void m68k_write_memory_32(unsigned int address, unsigned int value);
extern unsigned int m68k_read_immediate_8(unsigned int address);
extern unsigned int m68k_read_immediate_16(unsigned int address);
extern unsigned int m68k_read_immediate_32(unsigned int address);
extern unsigned int  m68k_read_pcrelative_8(unsigned int address);
extern unsigned int  m68k_read_pcrelative_16(unsigned int address);
extern unsigned int  m68k_read_pcrelative_32(unsigned int address);
extern void DrMD_Z80_write_8(unsigned char d,unsigned short a);
extern void DrMD_Z80_write_16(unsigned short d,unsigned short a);
extern unsigned char DrMD_Z80_read_8(unsigned short a);
extern unsigned short DrMD_Z80_read_16(unsigned short a);
extern unsigned char DrMD_Z80_In(unsigned char p);
extern void DrMD_Z80_Out(unsigned char p,unsigned char d);
extern unsigned int DrMD_Z80_Rebase_PC(unsigned short a);
extern unsigned int DrMD_Z80_Rebase_SP(unsigned short a);

//render8.s
extern void md_render_8(void);
//render16.s
extern void md_render_16(void);
//render16_small.s
extern void md_render_16_small(void);

#ifdef __cplusplus
} // End of extern "C"
#endif

