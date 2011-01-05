# This Makefile is very easy to use !
# Only edit PRG and OBJS 
# Dont touch anything else

CC = arm-wince-pe-gcc
AR = arm-wince-pe-ar

OBJS = m68kcpu.o m68kops.o m68kopac.o m68kopdm.o m68kopnz.o

CFLAGS     = -mcpu=arm920 -mtune=arm920t -O2 -msoft-float -ffast-math -fstrict-aliasing -mstructure-size-boundary=8 \
					-fexpensive-optimizations -fweb -frename-registers -fomit-frame-pointer -falign-functions -finline -finline-functions \
					-fno-builtin -fno-common

m68k.a:	$(OBJS)
	      $(AR) -rs $@ $(OBJS)

%.o:	%.c
	$(CC) $(ALL_INCLUDES) $(CFLAGS) -c $< -o $@

%.o:	%.s
	$(CC) $(ALL_INCLUDES) $(SFLAGS) -c $< -o $@
