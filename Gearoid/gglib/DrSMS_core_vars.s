      
      drsms      .req r3

      ;@ memory map referenced by drsms pointer
      ;@ banks - used in memory banking emulation
      .equ bank0, 0
      .equ bank1, bank0+4
      .equ bank2, bank1+4
      .equ bank3, bank2+4
      .equ bank4, bank3+4
      .equ bank5, bank4+4
      .equ bank6, bank5+4
      .equ bank7, bank6+4
      .equ bank8, bank7+4
      .equ bank9, bank8+4
      .equ banka, bank9+4
      .equ bankb, banka+4
      .equ bankc, bankb+4
      .equ bankd, bankc+4
      .equ banke, bankd+4
      .equ bankf, banke+4
	  
      ;@ sms 
      
      .equ cart_rom, bankf+4
	  .equ zram, cart_rom+4
	  .equ sram, zram+4
	  .equ sram1, sram+4
      .equ sram2, sram1+4
      .equ vdp_memory, sram2+4      
      .equ vdp_cram, vdp_memory+4
      .equ tile_cache, vdp_cram+4
      .equ render_line,tile_cache+4
      .equ frame_buffer,render_line+4
	  .equ local_pal,frame_buffer+4
	  .equ z80_context,local_pal+4
	  .equ z80_run,z80_context+4
	  .equ z80_set_irq,z80_run+4
	  .equ z80_reset,z80_set_irq+4
	  .equ psg_write,z80_reset+4
      
      .equ vdp_line, psg_write+4
      .equ lines_per_frame, vdp_line+2
      .equ vdp_addr, lines_per_frame+2
      .equ map_addr, vdp_addr+2
      .equ sprite_addr, map_addr+2
      .equ sprite_tiles, sprite_addr+2
      
      .equ vdp_reg0, sprite_tiles+2
      .equ vdp_reg1, vdp_reg0+1
      .equ vdp_reg2, vdp_reg1+1
      .equ vdp_reg3, vdp_reg2+1
      .equ vdp_reg4, vdp_reg3+1
      .equ vdp_reg5, vdp_reg4+1
      .equ vdp_reg6, vdp_reg5+1
      .equ vdp_reg7, vdp_reg6+1
      .equ vdp_reg8, vdp_reg7+1
      .equ vdp_reg9, vdp_reg8+1
      .equ vdp_regA, vdp_reg9+1
      .equ vdp_regB, vdp_regA+1
      .equ vdp_regC, vdp_regB+1
      .equ vdp_regD, vdp_regC+1
      .equ vdp_regE, vdp_regD+1
      .equ vdp_regF, vdp_regE+1
      .equ vdp_status, vdp_regF+1
      .equ vdp_left, vdp_status+1
      .equ cram_gg_latch, vdp_left+1
      .equ vdp_pending, cram_gg_latch+1
      .equ vdp_type, vdp_pending+1
      .equ first_byte, vdp_type+1
      .equ vdp_buffer, first_byte+1
      .equ max_rom_pages, vdp_buffer+1
      .equ memory_reg_C, max_rom_pages+1
      .equ memory_reg_D, memory_reg_C+1
      .equ memory_reg_E, memory_reg_D+1
      .equ memory_reg_F, memory_reg_E+1
      .equ drsms_country, memory_reg_F+1
      .equ sms_port0, drsms_country+1
      .equ sms_port1, sms_port0+1
      .equ sms_port5, sms_port1+1
      .equ sms_port63, sms_port5+1
      .equ gg_mode, sms_port63+1
      .equ spare1, gg_mode+1
	  .equ spare2, spare1+1
	  .equ pad, spare2+1
      


