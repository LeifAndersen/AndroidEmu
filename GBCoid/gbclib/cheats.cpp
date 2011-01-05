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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern "C" {
#include "fastmem.h"
}
#include "cheats.h"

static gbCheat gbCheatList[100];
static int gbCheatNumber = 0;
BOOL gbCheatMap[0x10000];

#define GBCHEAT_IS_HEX(a) ( ((a)>='A' && (a) <='F') || ((a) >='0' && (a) <= '9'))
#define GBCHEAT_HEX_VALUE(a) ( (a) >= 'A' ? (a) - 'A' + 10 : (a) - '0')

static void gbCheatUpdateMap()
{
  memset(gbCheatMap, 0, 0x10000);

  for(int i = 0; i < gbCheatNumber; i++) {
    if(gbCheatList[i].enabled)
      gbCheatMap[gbCheatList[i].address] = TRUE;
  }
}

BOOL gbVerifyGsCode(const char *code)
{
  int len = strlen(code);

  if(len != 8)
    return FALSE;

  for(int i = 0; i < 8; i++)
    if(!GBCHEAT_IS_HEX(code[i]))
      return FALSE;

  int address = GBCHEAT_HEX_VALUE(code[6]) << 12 |
    GBCHEAT_HEX_VALUE(code[7]) << 8 |
    GBCHEAT_HEX_VALUE(code[4]) << 4 |
    GBCHEAT_HEX_VALUE(code[5]);

  if(address < 0xa000 ||
     address > 0xdfff)
    return FALSE;

  return TRUE;
}

BOOL gbAddGsCheat(const char *code, const char *desc)
{
  if(gbCheatNumber > 99) {
//    systemMessage(MSG_MAXIMUM_NUMBER_OF_CHEATS,
//                  N_("Maximum number of cheats reached."));
    return FALSE;
  }

  if(!gbVerifyGsCode(code)) {
//    systemMessage(MSG_INVALID_GAMESHARK_CODE,
//                  N_("Invalid GameShark code: %s"), code);
    return FALSE;
  }
  
  int i = gbCheatNumber;

  strcpy(gbCheatList[i].cheatCode, code);
  strcpy(gbCheatList[i].cheatDesc, desc);
  
  gbCheatList[i].code = GBCHEAT_HEX_VALUE(code[0]) << 4 |
    GBCHEAT_HEX_VALUE(code[1]);

  gbCheatList[i].value = GBCHEAT_HEX_VALUE(code[2]) << 4 |
    GBCHEAT_HEX_VALUE(code[3]);

  gbCheatList[i].address = GBCHEAT_HEX_VALUE(code[6]) << 12 |
    GBCHEAT_HEX_VALUE(code[7]) << 8 |
    GBCHEAT_HEX_VALUE(code[4]) << 4 |
    GBCHEAT_HEX_VALUE(code[5]);

  gbCheatList[i].compare = 0;

  gbCheatList[i].enabled = TRUE;
  
  gbCheatMap[gbCheatList[i].address] = TRUE;
  
  gbCheatNumber++;
  return TRUE;
}

BOOL gbVerifyGgCode(const char *code)
{
  int len = strlen(code);

  if(len != 11 &&
     len != 7 &&
     len != 6)
    return FALSE;
  
  if(!GBCHEAT_IS_HEX(code[0]))
    return FALSE;
  if(!GBCHEAT_IS_HEX(code[1]))
    return FALSE;
  if(!GBCHEAT_IS_HEX(code[2]))
    return FALSE;
  if(code[3] != '-')
    return FALSE;
  if(!GBCHEAT_IS_HEX(code[4]))
    return FALSE;
  if(!GBCHEAT_IS_HEX(code[5]))
    return FALSE;
  if(!GBCHEAT_IS_HEX(code[6]))
    return FALSE;
  if(code[7] != 0) {
    if(code[7] != '-')
      return FALSE;
    if(code[8] != 0) {
      if(!GBCHEAT_IS_HEX(code[8]))
        return FALSE;
      if(!GBCHEAT_IS_HEX(code[9]))
        return FALSE;
      if(!GBCHEAT_IS_HEX(code[10]))
        return FALSE;
    }
  }

  //  int replace = (GBCHEAT_HEX_VALUE(code[0]) << 4) +
  //    GBCHEAT_HEX_VALUE(code[1]);

  int address = (GBCHEAT_HEX_VALUE(code[2]) << 8) +
    (GBCHEAT_HEX_VALUE(code[4]) << 4) +
    (GBCHEAT_HEX_VALUE(code[5])) +
    ((GBCHEAT_HEX_VALUE(code[6]) ^ 0x0f) << 12);

  if(address >= 0x8000 && address <= 0x9fff)
    return FALSE;

  if(address >= 0xc000)
    return FALSE;

  if(code[7] == 0 || code[8] == '0')
    return TRUE;

  int compare = (GBCHEAT_HEX_VALUE(code[8]) << 4) +
    (GBCHEAT_HEX_VALUE(code[10]));
  compare = compare ^ 0xff;
  compare = (compare >> 2) | ( (compare << 6) & 0xc0);
  compare ^= 0x45;

  int cloak = (GBCHEAT_HEX_VALUE(code[8])) ^ (GBCHEAT_HEX_VALUE(code[9]));
  
  if(cloak >=1 && cloak <= 7)
    return FALSE;

  return TRUE;
}

BOOL gbAddGgCheat(const char *code, const char *desc)
{
  if(gbCheatNumber > 99) {
//    systemMessage(MSG_MAXIMUM_NUMBER_OF_CHEATS,
//                  N_("Maximum number of cheats reached."));
    return FALSE;
  }

  if(!gbVerifyGgCode(code)) {
//    systemMessage(MSG_INVALID_GAMEGENIE_CODE,
//                  N_("Invalid GameGenie code: %s"), code);
    return FALSE;
  }
  
  int i = gbCheatNumber;

  int len = strlen(code);
  
  strcpy(gbCheatList[i].cheatCode, code);
  strcpy(gbCheatList[i].cheatDesc, desc);

  gbCheatList[i].code = 1;
  gbCheatList[i].value = (GBCHEAT_HEX_VALUE(code[0]) << 4) +
    GBCHEAT_HEX_VALUE(code[1]);
  
  gbCheatList[i].address = (GBCHEAT_HEX_VALUE(code[2]) << 8) +
    (GBCHEAT_HEX_VALUE(code[4]) << 4) +
    (GBCHEAT_HEX_VALUE(code[5])) +
    ((GBCHEAT_HEX_VALUE(code[6]) ^ 0x0f) << 12);

  gbCheatList[i].compare = 0;
  
  if(len != 7 && len != 8) {
    
    int compare = (GBCHEAT_HEX_VALUE(code[8]) << 4) +
      (GBCHEAT_HEX_VALUE(code[10]));
    compare = compare ^ 0xff;
    compare = (compare >> 2) | ( (compare << 6) & 0xc0);
    compare ^= 0x45;

    gbCheatList[i].compare = compare;
    gbCheatList[i].code = 0;
  }

  gbCheatList[i].enabled = TRUE;
  
  gbCheatMap[gbCheatList[i].address] = TRUE;
  
  gbCheatNumber++;
  return TRUE;
}

int gbCheatFind(const char *code)
{
	int i;
	for (i = 0; i < gbCheatNumber; i++) {
		if (strcmp(gbCheatList[i].cheatCode, code) == 0)
			return i;
	}
	return -1;
}

void gbCheatRemove(int i)
{
  if(i < 0 || i >= gbCheatNumber) {
//    systemMessage(MSG_INVALID_CHEAT_TO_REMOVE,
//                  N_("Invalid cheat to remove %d"), i);
    return;
  }
  
  if((i+1) <  gbCheatNumber) {
    memcpy(&gbCheatList[i], &gbCheatList[i+1], sizeof(gbCheat)*
           (gbCheatNumber-i-1));
  }
  
  gbCheatNumber--;

  gbCheatUpdateMap();
}

void gbCheatRemoveAll()
{
  gbCheatNumber = 0;
  gbCheatUpdateMap();
}

void gbCheatEnable(int i)
{
  if(i >=0 && i < gbCheatNumber) {
    if(!gbCheatList[i].enabled) {
      gbCheatList[i].enabled = TRUE;
      gbCheatUpdateMap();
    }
  }
}

void gbCheatDisable(int i)
{
  if(i >=0 && i < gbCheatNumber) {
    if(gbCheatList[i].enabled) {
      gbCheatList[i].enabled = FALSE;
      gbCheatUpdateMap();
    }
  }
}

u8 gbCheatRead(u16 address)
{
  for(int i = 0; i < gbCheatNumber; i++) {
    if(gbCheatList[i].enabled && gbCheatList[i].address == address) {
      switch(gbCheatList[i].code) {
      case 0x100: // GameGenie support
        if(readb_internal(address) == gbCheatList[i].compare)
          return gbCheatList[i].value;
        break;
      case 0x00:
      case 0x01:
      case 0x80:
        return gbCheatList[i].value;
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
        if(address >= 0xd000 && address < 0xe000) {
			break;
        } else
          return gbCheatList[i].value;
      }
    }
  }
  return readb_internal(address);
}
