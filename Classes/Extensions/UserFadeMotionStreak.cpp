#include "UserFadeMotionStreak.h"
#include "UserFadeRibbon.h"

UserFadeMotionStreak * UserFadeMotionStreak::streak(float seg, const char *imagePath, float width, float length, const ccColor4B& color)
{
	UserFadeMotionStreak *pRet = new UserFadeMotionStreak();
	if(pRet && pRet->init(seg, imagePath, width, length, color))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet)
	return NULL;
}

bool UserFadeMotionStreak::init(float seg, const char *imagePath, float width, float length, const ccColor4B& color)
{
	m_fSegThreshold = seg;
	m_fWidth = width;
	m_tLastLocation = CCPointZero;
	m_pRibbon = UserFadeRibbon::ribbonWithWidth(m_fWidth, imagePath, length, color, 2);
	this->addChild(m_pRibbon);

	// update ribbon position. Use schedule:interval and not scheduleUpdated. (cocos2d-iphone)issue #1075
	this->schedule(schedule_selector(CCMotionStreak::update), 0);
	return true;
}

void UserFadeMotionStreak::fadeOut(ccTime fadeTime, CCObject* target, SEL_CallFuncN callbackOnFadeOver)
{
	((UserFadeRibbon*)m_pRibbon)->fadeOut(fadeTime, target, callbackOnFadeOver);
}