#ifndef __STEERING_BUTTON_H__
#define __STEERING_BUTTON_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "Player.h"

USING_NS_CC;

class SteeringButton : public CCControl
{
	CCSprite* m_normalBackground, *pressedBackground;
	CC_SYNTHESIZE_READONLY(bool, m_pressed, IsPressed);
	virtual void setIsPressed(bool pressed);
public:
	static SteeringButton* steeringButtonFromFiles(const char* normalBackgroundFile, const char* pressedBackgroundFile, bool flippedX=false, bool flippedY=false);
	virtual bool initFromFiles(const char* normalBackgroundFile, const char* pressedBackgroundFile, bool flippedX, bool flippedY);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void registerWithTouchDispatcher();
};

#endif