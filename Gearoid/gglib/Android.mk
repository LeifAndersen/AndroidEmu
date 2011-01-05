LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := user

LOCAL_ARM_MODE := arm

# This is the target being built.
LOCAL_MODULE := libgg

# All of the source files that we will compile.
LOCAL_SRC_FILES := \
	zip.c \
	unzip.c \
	ioapi.c \
	fileio.c \
	sn76496.c \
	savestate.c \
	RenderSound.c \
	DrSMS_core.s \
	gp2x_sms_render_8.s \
	gp2x_gg_render_8.s \
	giz_blit.s \
	giz_blit_rev.s \
	DrZ80/DrZ80.s

LOCAL_SRC_FILES += \
	android/drsms_main.c \
	android/ggengine.cpp

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
LOCAL_CFLAGS += -D__EMU_SMS__

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true. However,
# it's difficult to do this for applications that are not supplied as
# part of a system image.

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

