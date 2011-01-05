#ifndef _SAVESTATE_H_
#define _SAVESTATE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct MD_SAVESTATE
{
	unsigned int spareint1;
	unsigned int spareint2;
	unsigned int spareint3;
	unsigned int spareint4;
	unsigned int spareint5;
	unsigned int spareint6;
	unsigned int spareint7;
	unsigned int spareint8;
	unsigned int spareint10;
	unsigned int spareint11;
	unsigned int spareint12;
	unsigned int spareint13;
	unsigned int spareint14;
	unsigned int spareint15;
	unsigned int spareint16;
	unsigned int spareint17;
	unsigned int spareint18;
	unsigned int spareint19;
	unsigned int spareint20;
	unsigned int spareint21;
	unsigned int spareint22;
	unsigned int spareint23;
	unsigned int spareint24;
	unsigned short spareshort1;
	unsigned short spareshort2;
	
	unsigned short drmd_lines_per_frame;
	unsigned short drmd_vdp_line;  
	// 0x62
	unsigned short drmd_vdp_status;                              // 0x64
	unsigned short drmd_vdp_addr;    
	// 0x66
	unsigned short drmd_vdp_addr_latch;                          // 0x68
	unsigned short drmd_gamma;      
	// 0x6A
	unsigned short drmd_cpl_m68k;                                // 0x6C
	unsigned short drmd_cpl_z80;   
	// 0x6E
	unsigned char drmd_spare70;                                      // 0x70
	unsigned char drmd_padselect;                                // 0x71
	unsigned char drmd_zbusreq;                                  // 0x72
	unsigned char drmd_zbusack;    
	// 0x73
	unsigned char drmd_zreset;                                   // 0x74
	unsigned char drmd_vdp_reg0;                                 // 0x75
	unsigned char drmd_vdp_reg1;                                 // 0x76
	unsigned char drmd_vdp_reg2;     
	// 0x77
	unsigned char drmd_vdp_reg3;                                 // 0x78
	unsigned char drmd_vdp_reg4;                                 // 0x79
	unsigned char drmd_vdp_reg5;                                 // 0x7A
	unsigned char drmd_vdp_reg6;   
	// 0x7B
	unsigned char drmd_vdp_reg7;                                 // 0x7C
	unsigned char drmd_vdp_reg8;                                 // 0x7D
	unsigned char drmd_vdp_reg9;                                 // 0x7E
	unsigned char drmd_vdp_reg10;   
	// 0x7F
	unsigned char drmd_vdp_reg11;                                // 0x80
	unsigned char drmd_vdp_reg12;                                // 0x81
	unsigned char drmd_vdp_reg13;                                // 0x82
	unsigned char drmd_vdp_reg14;  
	// 0x83
	unsigned char drmd_vdp_reg15;                                // 0x84
	unsigned char drmd_vdp_reg16;                                // 0x85
	unsigned char drmd_vdp_reg17;                                // 0x86
	unsigned char drmd_vdp_reg18;  
	// 0x87
	unsigned char drmd_vdp_reg19;                                // 0x88
	unsigned char drmd_vdp_reg20;                                // 0x89
	unsigned char drmd_vdp_reg21;                                // 0x8A
	unsigned char drmd_vdp_reg22;   
	// 0x8B
	unsigned char drmd_vdp_reg23;                                // 0x8C
	unsigned char drmd_vdp_reg24;                                // 0x8D
	unsigned char drmd_vdp_reg25;                                // 0x8E
	unsigned char drmd_vdp_reg26;  
	// 0x8F
	unsigned char drmd_vdp_reg27;                                // 0x90
	unsigned char drmd_vdp_reg28;                                // 0x91
	unsigned char drmd_vdp_reg29;                                // 0x92
	unsigned char drmd_vdp_reg30;   
	// 0x93
	unsigned char drmd_vdp_reg31;                                // 0x94
	unsigned char drmd_vdp_reg32;                                // 0x95
	unsigned char drmd_vdp_counter;                              // 0x96
	unsigned char drmd_hint_pending;  
	// 0x97
	unsigned char drmd_vint_pending;                             // 0x98
	unsigned char drmd_vdp_pending;                              // 0x99
	unsigned char drmd_vdp_code;                                 // 0x9A
	unsigned char drmd_vdp_dma_fill; 
	// 0x9B
	unsigned char drmd_pad_1_status;                             // 0x9C
	unsigned char drmd_pad_1_com;                                // 0x9D
	unsigned char drmd_pad_2_status;                             // 0x9E
	unsigned char drmd_pad_2_com;  
	// 0x9F
	unsigned int drmd_sram_start;                                // 0xA0
	unsigned int drmd_sram_end;                                  // 0xA4
	unsigned int drmd_sram;   
	// 0xA8
	unsigned char drmd_region;                                   // 0xAC
	unsigned char drmd_sram_flags;                               // 0xAD
	unsigned char drmd_spareAE;                                  // 0xAE
	unsigned char drmd_spareAF;  
	// 0xAF
	float drmd_cpl_fm;                                           // 0xB0
	unsigned char drmd_genesis_rom_banks[0x10];                  // 0xB4
	unsigned int drmd_pad; 
	// 0xC4
	unsigned char drmd_pad_1_counter;                                   // 0xC8
	unsigned char drmd_pad_1_delay;
	unsigned char drmd_pad_2_counter;
	unsigned char drmd_pad_2_delay; 
	
	unsigned int drmd_spareCC;                                   // 0xCC
	unsigned int drmd_spareD0;                                   // 0xD0
	unsigned int drmd_spareD4;                                   // 0xD4
	unsigned int drmd_spareD8;                                   // 0xD8
	unsigned int drmd_spareDC;                                   // 0xDC
	unsigned int drmd_spareE0;                                   // 0xE0
	unsigned int drmd_spareE4;                                   // 0xE4
	unsigned int drmd_spareE8;                                   // 0xE8
	unsigned int drmd_spareEC;                                   // 0xEC
	unsigned int drmd_spareF0;                                   // 0xF0
	unsigned int drmd_spareF4;                                   // 0xF4
	unsigned int drmd_spareF8;                                   // 0xF8
	unsigned int drmd_spareFC;                                   // 0xFC
	unsigned int drmd_spare100;                                  // 0x100
	unsigned int drz80_Z80PC;
	unsigned int drz80_Z80A;
	unsigned int drz80_Z80F;
	unsigned int drz80_Z80BC;
	unsigned int drz80_Z80DE;
	unsigned int drz80_Z80HL; 
	unsigned int drz80_Z80SP;
	unsigned int drz80_Z80PC_BASE;
	unsigned int drz80_Z80SP_BASE;
	unsigned int drz80_Z80IX;
	unsigned int drz80_Z80IY;
	unsigned int drz80_Z80I;
	unsigned int drz80_Z80A2;
	unsigned int drz80_Z80F2;
	unsigned int drz80_Z80BC2;
	unsigned int drz80_Z80DE2;
	unsigned int drz80_Z80HL2;   
	
	unsigned char drz80_Z80_IRQ;
	unsigned char drz80_Z80IF;
	unsigned char drz80_Z80IM;
	unsigned char drz80_Z80R;
	
	unsigned int drz80_z80irqvector;
	
	unsigned char drz80_Z80_NMI;
	unsigned char drz80_sparechar1;
	unsigned char drz80_sparechar2;
	unsigned char drz80_sparechar3;
	
	unsigned int drz80_spareint1;
	unsigned int drz80_spareint2;
	unsigned int drz80_spareint3;
	unsigned int drz80_spareint4;
	unsigned int drz80_spareint5;
	unsigned int drz80_spareint6;
	unsigned int drz80_spareint7;
	unsigned int drz80_spareint8;
	unsigned char cyclone[128];
	unsigned int drz80_spareint9;
	unsigned int drz80_spareint10;
	unsigned int drz80_spareint11;
	unsigned int drz80_spareint12;
	unsigned int drz80_spareint13;
	unsigned int drz80_spareint14;
	unsigned int drz80_spareint15;
	unsigned int drz80_spareint16;
	unsigned int drz80_spareint17;
	unsigned int drz80_spareint18;
	unsigned int drz80_spareint19;
	unsigned int drz80_spareint20;
	unsigned char work_ram[0x10000];
	unsigned short vram[0x8000];
	unsigned char zram[0x4000];
	unsigned char cram[0x80];
	unsigned char vsram[0x80];
	FM_3SLOT SL3;
	FM_ST	ST;
	FM_OPN OPN;
	FM_CH CH[6];
	int dacout;	
	int dacen;
	char OPN_pan[6*2]; 
	struct PSG_CONTEXT PSG;
	unsigned int YMOPN_ST_dt_tab;
	unsigned char sram[0x10000];
};

void loadstate_mem(const struct MD_SAVESTATE *s);
void savestate_mem(struct MD_SAVESTATE *s);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif /*_SAVESTATE_H_*/
