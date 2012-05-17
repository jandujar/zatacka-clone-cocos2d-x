LOCAL_PATH := $(call my-dir)
LOCAL_CFLAGS := -O3
include $(CLEAR_VARS)
APP_ABI := armeabi armeabi-v7a 
subdirs := $(addprefix $(LOCAL_PATH)/../../../,$(addsuffix /Android.mk, \
           cocos2dx \
           CocosDenshion/android \
	))
subdirs += $(LOCAL_PATH)/../../Classes/Android.mk $(LOCAL_PATH)/helloworld/Android.mk

include $(subdirs)
