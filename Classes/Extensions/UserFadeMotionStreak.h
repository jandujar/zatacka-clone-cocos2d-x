#ifndef __USER_FADE_MOTION_STREAK_H__
#define __USER_FADE_MOTION_STREAK_H__

#include "cocos2d.h"

USING_NS_CC;

class UserFadeMotionStreak: public CCMotionStreak
{
protected:
	
public:
	/** creates the a MotionStreak. The image will be loaded using the TextureMgr. */
	static UserFadeMotionStreak * streak(float seg, const char *imagePath, float width, float length, const ccColor4B& color);

	/** initializes a MotionStreak. The file will be loaded using the TextureMgr. */
	bool init(float seg, const char *imagePath, float width, float length, const ccColor4B& color);

	void fadeOut(ccTime fadeTime, CCObject* target, SEL_CallFuncN callbackOnFadeOver);
};

#endif