// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef __VBA_GB_GBCHEATS_H
#define __VBA_GB_GBCHEATS_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned char BOOL;

#define FALSE	0
#define TRUE	1

struct gbXxCheat {
  char cheatDesc[100];
  char cheatCode[20];
};

struct gbCheat {
  char cheatCode[20];
  char cheatDesc[32];
  u16 address;
  int code;
  u8 compare;
  u8 value;
  BOOL enabled;
};

#ifdef __cplusplus
extern "C" {
#endif

BOOL gbAddGsCheat(const char *, const char*);
BOOL gbAddGgCheat(const char *, const char*);
int gbCheatFind(const char *code);
void gbCheatRemove(int);
void gbCheatRemoveAll();
void gbCheatEnable(int);
void gbCheatDisable(int);
u8 gbCheatRead(u16);

#ifdef __cplusplus
}
#endif

extern BOOL gbCheatMap[0x10000];
#endif

