LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mupen64plus-core
LOCAL_ARM_MODE := arm

SRCDIR := ../../src

LOCAL_SRC_FILES := \
	$(SRCDIR)/api/callbacks.c \
	$(SRCDIR)/api/common.c \
	$(SRCDIR)/api/config.c \
	$(SRCDIR)/api/frontend.c \
	$(SRCDIR)/main/main.c \
	$(SRCDIR)/main/ticks.c \
	$(SRCDIR)/main/util.c \
	$(SRCDIR)/main/cheat.c \
	$(SRCDIR)/main/md5.c \
	$(SRCDIR)/main/rom.c \
	$(SRCDIR)/main/ini_reader.c \
	$(SRCDIR)/main/savestates.c \
	$(SRCDIR)/main/adler32.c \
	$(SRCDIR)/main/zip/ioapi.c \
	$(SRCDIR)/main/zip/zip.c \
	$(SRCDIR)/main/zip/unzip.c \
	$(SRCDIR)/memory/dma.c \
	$(SRCDIR)/memory/flashram.c \
	$(SRCDIR)/memory/memory.c \
    $(SRCDIR)/memory/n64_cic_nus_6105.c \
	$(SRCDIR)/memory/pif.c \
	$(SRCDIR)/memory/tlb.c \
	$(SRCDIR)/osal/dynamiclib_unix.c \
	$(SRCDIR)/osal/files_unix.c \
	$(SRCDIR)/plugin/plugin.c \
	$(SRCDIR)/plugin/dummy_video.c \
	$(SRCDIR)/plugin/dummy_audio.c \
	$(SRCDIR)/plugin/dummy_input.c \
	$(SRCDIR)/plugin/dummy_rsp.c \
	$(SRCDIR)/r4300/r4300.c \
	$(SRCDIR)/r4300/bc.c \
	$(SRCDIR)/r4300/cop0.c \
	$(SRCDIR)/r4300/cop1.c \
	$(SRCDIR)/r4300/cop1_d.c \
	$(SRCDIR)/r4300/cop1_l.c \
	$(SRCDIR)/r4300/cop1_s.c \
	$(SRCDIR)/r4300/cop1_w.c \
	$(SRCDIR)/r4300/exception.c \
	$(SRCDIR)/r4300/interupt.c \
	$(SRCDIR)/r4300/profile.c \
	$(SRCDIR)/r4300/pure_interp.c \
	$(SRCDIR)/r4300/recomp.c \
	$(SRCDIR)/r4300/special.c \
	$(SRCDIR)/r4300/regimm.c \
	$(SRCDIR)/r4300/tlb.c

LOCAL_SRC_FILES += \
	$(SRCDIR)/r4300/empty_dynarec.c \
	$(SRCDIR)/r4300/new_dynarec/new_dynarec.c \
	$(SRCDIR)/r4300/new_dynarec/fpu.c \
	$(SRCDIR)/r4300/new_dynarec/linkage_arm.s

ifeq ($(TARGET_ARCH_ABI), armeabi)
	LOCAL_CFLAGS += -DARMv5_ONLY
endif

LOCAL_CFLAGS += -DDYNAREC
LOCAL_CFLAGS += -I$(LOCAL_PATH)/$(SRCDIR)

LOCAL_LDFLAGS += -Wl,-version-script,$(LOCAL_PATH)/$(SRCDIR)/api/api_export.ver
LOCAL_LDLIBS := -llog -lz

include $(BUILD_SHARED_LIBRARY)
