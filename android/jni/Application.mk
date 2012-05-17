APP_STL := gnustl_static
APP_CPPFLAGS += -frtti
APP_ABI := armeabi armeabi-v7a 
NDK_DEBUG=0
LOCAL_CFLAGS := -O3
APP_CFLAGS := -O3
APP_MODULES := cocos2dx_static cocosdenshion_shared game_logic_static helloworld_shared