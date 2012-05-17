LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_logic_static

LOCAL_MODULE_FILENAME := libgame_logic

LOCAL_SRC_FILES := AppDelegate.cpp \
                   HelloWorldScene.cpp \
				   PlayerChoiceScene.cpp \
				   GameState.cpp \
				   CCControlExtension/CCControl.cpp \
				   CCControlExtension/CCControlButton.cpp \
				   CCControlExtension/CCInvocation.cpp \
				   CCControlExtension/CCControlSlider.cpp \
				   CCControlExtension/CCControlHuePicker.cpp \
				   CCControlExtension/CCControlSaturationBrightnessPicker.cpp \
				   CCControlExtension/CCControlColourPicker.cpp \
				   CCControlExtension/CCControlCheckBox.cpp \
				   CCControlExtension/CCMenuPassive.cpp \
				   CCControlExtension/CCSpacer.cpp \
				   CCControlExtension/CCSetting.cpp \
				   CCControlExtension/Utils/CCControlUtils.cpp \
				   Extensions/CCMenuItemDynamic.cpp \
				   Extensions/CCMenuItemSlider.cpp \
				   Extensions/CCScale9Sprite.cpp \
				   Menus/MenuScene.cpp \
				   UI/CCClippedNode.cpp \
				   UI/PowerupScroller.cpp \
				   UI/SteeringButton.cpp \
				   UI/PowerupButton.cpp \
				   UI/PlayerUI.cpp \
				   UI/LayoutSet.cpp \
				   GameElements/Player.cpp \
				   GameElements/Powerup.cpp \
				   GameElements/AIParameter.cpp
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/CCControlExtension \
					$(LOCAL_PATH)/CCControlExtension/Utils \
					$(LOCAL_PATH)/Extensions \
					$(LOCAL_PATH)/Menus \
					$(LOCAL_PATH)/UI \
					$(LOCAL_PATH)/GameElements
APP_OPTIM := release
APP_ABI := armeabi
LOCAL_LDLIBS := -lGLESv1_CM     
                   
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

LOCAL_STATIC_LIBRARIES := png_static_prebuilt
LOCAL_STATIC_LIBRARIES += xml2_static_prebuilt
LOCAL_STATIC_LIBRARIES += jpeg_static_prebuilt
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
                
LOCAL_SHARED_LIBRARIES := cocosdenshion_shared
            
include $(BUILD_STATIC_LIBRARY)

$(call import-module,cocos2dx/platform/third_party/android/modules/libpng)
$(call import-module,cocos2dx/platform/third_party/android/modules/libxml2)
$(call import-module,cocos2dx/platform/third_party/android/modules/libjpeg)
