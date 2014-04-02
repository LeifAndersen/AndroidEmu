LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mupen64plus-rsp-hle
LOCAL_ARM_MODE := arm

SRCDIR := ../../src

LOCAL_SRC_FILES := \
	$(SRCDIR)/main.c \
	$(SRCDIR)/jpeg.c \
	$(SRCDIR)/ucode3.cpp \
	$(SRCDIR)/ucode2.cpp \
	$(SRCDIR)/ucode1.cpp \
	$(SRCDIR)/ucode3mp3.cpp

LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../mupen64plus-core/src/api

LOCAL_LDFLAGS += -Wl,-version-script,$(LOCAL_PATH)/$(SRCDIR)/rsp_api_export.ver

include $(BUILD_SHARED_LIBRARY)
