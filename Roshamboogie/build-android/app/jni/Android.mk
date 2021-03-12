########################
# 
# Linker Makefile
#
########################
CACHE_PATH := $(call my-dir)
LOCAL_PATH := $(CACHE_PATH)
PROJ_PATH  := $(CACHE_PATH)
CUGL_PATH  := $(CACHE_PATH)/../../../cugl/build-android
LOCAL_SHORT_COMMANDS := true

include $(CUGL_PATH)/jni/Android.mk
include $(PROJ_PATH)/source/Android.mk
