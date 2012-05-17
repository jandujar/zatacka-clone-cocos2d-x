#ifndef __USER_FADE_RIBBON_H__
#define __USER_FADE_RIBBON_H__

#include "cocos2d.h"

#include "CCRibbon.h"

USING_NS_CC;

class UserFadeRibbon: public CCRibbon
{
protected:
	bool fadingOut;
	ccTime startFadeTime;
	ccTime endFadeTime;
	ccTime currentFadeTime;
	SEL_CallFuncN m_callbackOnFadeOver;
	CCObject* m_target;
public:
	static UserFadeRibbon * ribbonWithWidth(float w, const char *path, float length, const ccColor4B& color, float fade);
	virtual void update(ccTime dt);
	virtual void draw();
	virtual void fadeOut(ccTime fadeTime, CCObject* target, SEL_CallFuncN m_callbackOnFadeOver);

protected:
	void disableFade();
};

#endif