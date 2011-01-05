LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := user

LOCAL_ARM_MODE := arm

# This is the target being built.
LOCAL_MODULE := libemu

# All of the source files that we will compile.
LOCAL_SRC_FILES := \
	ticks.c \
	main.cpp \
	emumedia.cpp \
	emulator.cpp

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libdl \
	libnativehelper \
	libutils

# Static libraries.
LOCAL_STATIC_LIBRARIES :=

# Also need the JNI headers.
LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE)

# Special compiler flags.
LOCAL_CFLAGS += -O3 -fvisibility=hidden

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true. However,
# it's difficult to do this for applications that are not supplied as
# part of a system image.

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
