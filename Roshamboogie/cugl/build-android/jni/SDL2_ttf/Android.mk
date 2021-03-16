###########################
#
# SDL2_ttf Prebuilt library
#
###########################
APP_SHORT_COMMANDS := true
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libSDL2_ttf.so

include $(PREBUILT_SHARED_LIBRARY)
