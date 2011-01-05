/*
 * DrZ80 Version 1.0
 * Z80 Emulator by Reesy
 * Copyright 2005 Reesy
 * 
 * This file is part of DrZ80.
 * 
 *     DrZ80 is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     DrZ80 is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with DrZ80; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

extern int DrZ80Ver; // Version number of library

struct DrZ80
{ 
  unsigned int Z80PC;
  unsigned int Z80A;
  unsigned int Z80F;
  unsigned int Z80BC;
  unsigned int Z80DE;
  unsigned int Z80HL; 
  unsigned int Z80SP;
  unsigned int Z80PC_BASE;
  unsigned int Z80SP_BASE;
  unsigned int Z80IX;
  unsigned int Z80IY;
  unsigned int Z80I;
  unsigned int Z80A2;
  unsigned int Z80F2;
  unsigned int Z80BC2;
  unsigned int Z80DE2;
  unsigned int Z80HL2;   
  unsigned char Z80_IRQ;
  unsigned char Z80IF;
  unsigned char Z80IM;
  unsigned char Z80R;
  unsigned int z80irqvector;
  void (*z80_irq_callback )(void);
  void (*z80_write8 )(unsigned char d,unsigned short a); 
  void (*z80_write16 )(unsigned short d,unsigned short a); 
  unsigned char (*z80_in)(unsigned short p);
  void (*z80_out )(unsigned short p,unsigned char d);
  unsigned char (*z80_read8)(unsigned short a);
  unsigned short (*z80_read16)(unsigned short a);
  unsigned int (*z80_rebaseSP)(unsigned short new_sp);
  unsigned int (*z80_rebasePC)(unsigned short new_pc);
  unsigned char Z80_NMI;
};

extern void DrZ80Run(struct DrZ80 *pcy,unsigned int cyc);

#ifdef __cplusplus
} // End of extern "C"
#endif
