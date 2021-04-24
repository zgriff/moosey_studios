###########################
#
# SDL2 Prebuilt library
#
###########################
APP_SHORT_COMMANDS := true
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libSDL2.so

include $(PREBUILT_SHARED_LIBRARY)