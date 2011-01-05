LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := user

LOCAL_ARM_MODE := arm

# This is the target being built.
LOCAL_MODULE := libgbc

# All of the source files that we will compile.
LOCAL_SRC_FILES := \
	cpu.c \
	debug.c \
	emu.c \
	events.c \
	exports.c \
	fastmem.c \
	hw.c \
	keytable.c \
	lcd.c \
	lcdc.c \
	loader.c \
	mem.c \
	palette.c \
	path.c \
	rccmds.c \
	rcfile.c \
	rckeys.c \
	rcvars.c \
	refresh.c \
	rtc.c \
	save.c \
	sound.c \
	cheats.cpp \
	split.c \
	ioapi.c \
	zip.c \
	unzip.c \
	fileio.c \

LOCAL_SRC_FILES += \
	android/gbc_main.c \
	android/gbcengine.cpp

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libz

# Static libraries.
LOCAL_STATIC_LIBRARIES := \
	libunz

# Also need the JNI headers.
LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../../common \
	external/zlib

# Compiler flags.
LOCAL_CFLAGS += -O3 -fvisibility=hidden

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true. However,
# it's difficult to do this for applications that are not supplied as
# part of a system image.

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

