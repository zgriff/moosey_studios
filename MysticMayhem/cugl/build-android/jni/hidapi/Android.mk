###########################
#
# hidapi Prebuilt library
#
###########################
APP_SHORT_COMMANDS := true
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hidapi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libhidapi.so

include $(PREBUILT_SHARED_LIBRARY)