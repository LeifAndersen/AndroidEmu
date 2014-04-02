LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

MY_LOCAL_PATH := $(LOCAL_PATH)

include $(MY_LOCAL_PATH)/mupen64plus-core/projects/android/Android.mk
include $(MY_LOCAL_PATH)/mupen64plus-rsp-hle/projects/android/Android.mk
