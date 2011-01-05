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

#ifndef _DRZ80SUPPORT_H_
#define _DRZ80SUPPORT_H_

extern struct DrZ80 drz80;

void DrZ80_Reset();
void DrZ80_Init();
void DrZ80_Set_Irq(unsigned int irq);
void DrZ80_irq_callback(void);
void z80_push(int reg);
unsigned int DrZ80_Rebase_SP(unsigned int newsp);
unsigned int DrZ80_Rebase_PC(unsigned int newpc);



#endif /* _DRZ80SUPPORT_H_ */

