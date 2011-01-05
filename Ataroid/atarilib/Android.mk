LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := user

LOCAL_ARM_MODE := arm

# This is the target being built.
LOCAL_MODULE := libatari

LOCAL_CPP_EXTENSION = .cxx

# All of the source files that we will compile.
LOCAL_SRC_FILES := \
	common/SoundNull.cxx \
	emucore/AtariVox.cxx \
	emucore/Booster.cxx \
	emucore/Cart2K.cxx \
	emucore/Cart3F.cxx \
	emucore/Cart3E.cxx \
	emucore/Cart4A50.cxx \
	emucore/Cart4K.cxx \
	emucore/CartAR.cxx \
	emucore/CartCV.cxx \
	emucore/Cart.cxx \
	emucore/CartDPC.cxx \
	emucore/CartDPCPlus.cxx \
	emucore/CartE0.cxx \
	emucore/CartE7.cxx \
	emucore/CartEF.cxx \
	emucore/CartEFSC.cxx \
	emucore/CartF0.cxx \
	emucore/CartF4.cxx \
	emucore/CartF4SC.cxx \
	emucore/CartF6.cxx \
	emucore/CartF6SC.cxx \
	emucore/CartF8.cxx \
	emucore/CartF8SC.cxx \
	emucore/CartFA.cxx \
	emucore/CartFE.cxx \
	emucore/CartMC.cxx \
	emucore/CartSB.cxx \
	emucore/CartUA.cxx \
	emucore/Cart0840.cxx \
	emucore/CartX07.cxx \
	emucore/Console.cxx \
	emucore/Control.cxx \
	emucore/Device.cxx \
	emucore/Driving.cxx \
	emucore/Event.cxx \
	emucore/EventHandler.cxx \
	emucore/FrameBuffer.cxx \
	emucore/FSNode.cxx \
	emucore/Genesis.cxx \
	emucore/Joystick.cxx \
	emucore/Keyboard.cxx \
	emucore/KidVid.cxx \
	emucore/M6502.cxx \
	emucore/M6532.cxx \
	emucore/MT24LC256.cxx \
	emucore/NullDev.cxx \
	emucore/MD5.cxx \
	emucore/MediaFactory.cxx \
	emucore/OSystem.cxx \
	emucore/Paddles.cxx \
	emucore/Props.cxx \
	emucore/PropsSet.cxx \
	emucore/Random.cxx \
	emucore/SaveKey.cxx \
	emucore/Serializer.cxx \
	emucore/Settings.cxx \
	emucore/Switches.cxx \
	emucore/StateManager.cxx \
	emucore/System.cxx \
	emucore/TIA.cxx \
	emucore/TIASnd.cxx \
	emucore/TIATables.cxx \
	emucore/TrackBall.cxx \
	emucore/unzip.c \
	unix/FSNodePOSIX.cxx \
	unix/OSystemUNIX.cxx \
	unix/SettingsUNIX.cxx

LOCAL_SRC_FILES += \
	android/giz_blit.s \
	android/giz_blit_rev.s \
	android/FrameBufferAndroid.cxx \
	android/SoundAndroid.cxx \
	android/atariengine.cxx

# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libz

# Static libraries.
LOCAL_STATIC_LIBRARIES := \
	libunz \
	libstlport

# Also need the JNI headers.
LOCAL_C_INCLUDES := \
	bionic \
	external/zlib \
	$(LOCAL_PATH)/common \
	$(LOCAL_PATH)/emucore \
	$(LOCAL_PATH)/gui \
	$(LOCAL_PATH)/unix \
	$(LOCAL_PATH)/android \
	$(LOCAL_PATH)/../../common \
	$(JNI_H_INCLUDE) \
	$(LOCAL_C_INCLUDES)

# Special compiler flags.
LOCAL_CFLAGS += -O3 -fexceptions -fvisibility=hidden

LOCAL_CFLAGS += \
	-DUNIX \
	-DBSPF_UNIX \
	-DHAVE_GETTIMEOFDAY \
	-DHAVE_INTTYPES \
	-DSOUND_SUPPORT

# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true. However,
# it's difficult to do this for applications that are not supplied as
# part of a system image.

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
