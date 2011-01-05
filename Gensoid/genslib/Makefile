# This Makefile is very easy to use !
# Only edit PRG and OBJS 
# Dont touch anything else

CC = arm-elf-gcc
LD = arm-elf-gcc
AS = arm-elf-as
AR = arm-elf-ar

PRG  = DrMD
OBJS = Main.o Menu.o Input.o fm.o DrMD_core.o \
       gp32_menu_header.o gp32_highlightbar.o gp32_menutile.o \
       sn76496.o gp32_snd.o unzip.o zip_old.o \
       ioapi.o fileio.o gp32_md_render_8.o gp32_md_render_16_small.o\
       mdrom_good.o rom_crc.o ../DrZ80/DrZ80.o ../cyclone/Cyclone.o \
	   savestate.o

SDKDIR   = C:/gp32_MrMirko/gp32_SDK
LIBS      = -L$(SDKDIR)/lib -lmirkoSDK -lm
CRT0      = $(SDKDIR)/lib/crt0.S
LNKSCRIPT = $(SDKDIR)/lib/lnkscript
INCLUDES  = -I$(SDKDIR)/lib.src/include -I$(SDKDIR)/lib.src/zlib/include
# CFLAGS    = $(INCLUDES) -D__GP32__ -D__EMU_MD__ -O2 -s -msoft-float -mtune=arm9tdmi
CFLAGS    = $(INCLUDES) -D__GP32__ -D__EMU_MD__ -O2 -s -msoft-float -mtune=arm9tdmi \
		-Wall -fomit-frame-pointer -ffast-math
# CFLAGS    = $(INCLUDES) -D__GP32__ -D__EMU_MD__ -Wall -O2 -mtune=arm9tdmi\
# 			-fomit-frame-pointer\
#			-ffast-math

all:	$(OBJS)
	$(CC) -c -o crt0.o $(CRT0)
	$(LD) -nostartfiles -s -Wall -Wl,-Map,Test.map  -T $(LNKSCRIPT) crt0.o -o $(PRG).elf $(OBJS)  $(LIBS)
	arm-elf-objcopy -O binary $(PRG).elf $(PRG).bin
	b2fxec -a Reesy -t "DrMD v5 beta 8 By Reesy" -b DrMD_Sonic.bmp $(PRG).bin $(PRG).fxe

%.o:	%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

%.o:	%.s
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

install:
	gplink put $(PRG).fxe gpmm

clean:
	rm -f *.o *~ Test.map *.bin *.elf
