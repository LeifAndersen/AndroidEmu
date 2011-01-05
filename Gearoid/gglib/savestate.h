#ifndef _SAVESTATE_H_
#define _SAVESTATE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct SMS_SAVESTATE
{
	unsigned int spareint0[0x10];
	unsigned int spareint1;
	unsigned int spareint2;
	unsigned int spareint3;
	unsigned int spareint4;
	unsigned int spareint5;
	unsigned int spareint6;
	unsigned int spareint7;
	unsigned int spareint8;
	unsigned int spareint9;
	unsigned int spareint10;
	unsigned int spareint11;
	unsigned int spareint12;                               // 0x14 
	unsigned int spareint13; 
	unsigned int spareint14; 
	unsigned int spareint15; 
	unsigned int spareint16; 
	unsigned short drsms_vdp_line;
	unsigned short drsms_lines_per_frame;
	unsigned short drsms_vdp_addr;
	unsigned short drsms_map_addr;
	unsigned short drsms_sprite_addr;
	unsigned short drsms_sprite_tiles;
	unsigned char drsms_vdp_reg0;
	unsigned char drsms_vdp_reg1;
	unsigned char drsms_vdp_reg2;
	unsigned char drsms_vdp_reg3;
	unsigned char drsms_vdp_reg4;
	unsigned char drsms_vdp_reg5;
	unsigned char drsms_vdp_reg6;
	unsigned char drsms_vdp_reg7;
	unsigned char drsms_vdp_reg8;
	unsigned char drsms_vdp_reg9;
	unsigned char drsms_vdp_regA;
	unsigned char drsms_vdp_regB;
	unsigned char drsms_vdp_regC;
	unsigned char drsms_vdp_regD;
	unsigned char drsms_vdp_regE;
	unsigned char drsms_vdp_regF;
	unsigned char drsms_vdp_status;
	unsigned char drsms_vdp_left;
	unsigned char drsms_cram_gg_latch;
	unsigned char drsms_vdp_pending;
	unsigned char drsms_vdp_type;
	unsigned char drsms_first_byte;
	unsigned char drsms_vdp_buffer;
	unsigned char drsms_max_rom_pages;
	unsigned char drsms_memory_reg_C;
	unsigned char drsms_memory_reg_D;
	unsigned char drsms_memory_reg_E;
	unsigned char drsms_memory_reg_F;
	unsigned char drsms_drsms_country;
	unsigned char drsms_sms_port0;
	unsigned char drsms_sms_port1;
	unsigned char drsms_sms_port5;
	unsigned char drsms_sms_port63;
	unsigned char drsms_gg_mode;
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
	unsigned int spareint17; 
	unsigned int spareint18; 
	unsigned int spareint19; 
	unsigned int spareint20; 
	unsigned int spareint21; 
	unsigned int spareint22; 
	unsigned int spareint23; 
	unsigned int spareint24; 
	unsigned int spareint25; 
	char work_ram[0x2000];
	char vram[0x4000];
	char cram[0x80];
	char sram[0x8000];
	struct PSG_CONTEXT PSG;
	unsigned char drz80_Z80_NMI;
};

void loadstate_mem(const struct SMS_SAVESTATE *s);
void savestate_mem(struct SMS_SAVESTATE *s);

#ifdef __cplusplus
} // End of extern "C"
#endif

#endif /*_SAVESTATE_H_*/
