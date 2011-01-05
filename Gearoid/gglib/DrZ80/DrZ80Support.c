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
 
#include DrZ80.h

struct DrZ80 drz80;

//example of how to run DrZ80
static RunCPUExample()
{
	// Simple call DrZ80Run, passing the address of the drz80 context and the number of cycles you want to run
	DrZ80Run(&drz80, 100);
}

void DrZ80_Init()
{
  // save pointers to all required memory functions
  drz80.z80_write8=Z80_write_8;
  drz80.z80_write16=Z80_write_16;
  drz80.z80_in=Z80_In;
  drz80.z80_out=Z80_Out;
  drz80.z80_read8=Z80_read_8;
  drz80.z80_read16=Z80_read_16;
  drz80.z80_rebasePC=Z80_Rebase_PC;
  drz80.z80_rebaseSP=Z80_Rebase_SP;
  drz80.z80_irq_callback=DrZ80_irq_callback;
}

void DrZ80_Reset()
{
  drz80.Z80A = 0x00		<<24;
  drz80.Z80F = (1<<2)	<<24;  // set ZFlag
  drz80.Z80BC = 0x0000	<<16;
  drz80.Z80DE = 0x0000	<<16;
  drz80.Z80HL = 0x0000	<<16;
  drz80.Z80A2 = 0x00	<<24;
  drz80.Z80F2 = 1<<2	<<24;  // set ZFlag
  drz80.Z80BC2 = 0x0000	<<16;
  drz80.Z80DE2 = 0x0000	<<16;
  drz80.Z80HL2 = 0x0000	<<16;
  drz80.Z80IX = 0xFFFF	<<16;
  drz80.Z80IY = 0xFFFF	<<16;
  
  drz80.Z80I = 0x00;
  drz80.Z80IM = 0x00;
  drz80.Z80_IRQ = 0x00;
  drz80.Z80IF = 0x00;
  
  drz80.Z80PC=DrZ80_Rebase_PC(0);
  drz80.Z80SP=DrZ80_Rebase_SP(0);
}

void DrZ80_Set_Irq(unsigned int irq)
{
    drz80.z80irqvector = 0xFF;  // default IRQ vector RST opcode
	drz80.Z80_IRQ = irq;
}

void DrZ80_irq_callback(void)
{
    drz80.Z80_IRQ = 0x00;  // lower irq when in accepted
}

unsigned int DrZ80_Rebase_PC(unsigned short address)
{
	if (address>0xC000)
	{
		//PC in memory - rebase PC into memory
		drz80.Z80PC_BASE = (unsigned int)&Z80_Ram-0xC000;
		return (drz80.Z80PC_BASE + address);
	}
	else
	{
		//PC in rom - rebase PC into rom
		drz80.Z80PC_BASE = (unsigned int)&Z80_Rom;
		return (drz80.Z80PC_BASE + address);
	}
}

unsigned int DrZ80_Rebase_SP(unsigned short address)
{
	if (address>0xC000)
	{
		//PC in memory - rebase PC into memory
		drz80.Z80SP_BASE = (unsigned int)&Z80_Ram-0xC000;
		return (drz80.Z80SP_BASE + address);
	}
	else
	{
		//PC in rom - rebase PC into rom
		drz80.Z80SP_BASE = (unsigned int)&Z80_Rom;
		return (drz80.Z80SP_BASE + address);
	}
}

