#ifndef __POWERUP_SCROLLER_H__
#define __POWERUP_SCROLLER_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "CCClippedNode.h"
#include "PowerupButton.h"
#include "SteeringButton.h"
#include "GameState.h"

#define SWIPE_MIN_DISTANCE 20
#define SWIPE_MAX_OFF_PATH 100
#define SWIPE_THRESHOLD_VELOCITY 20


class PowerupScroller : public CCControl
{
protected:
	CCSprite* m_backgroundScroller;
	CCSprite* m_overlay;
	CCSprite* m_overlayPressed;
	
	std::vector<PowerupButton*> powerupButtons;
	int selectedPowerup;
	CCPoint selectedPowerupPosition, deselectedPowerupPositionLower, deselectedPowerupPositionUpper;
	
	CCPoint flingOrigin;
	ccTime flingTime;
	bool flinging;
	float uiScale, spriteScale;	

public:
	static PowerupScroller* powerupScroller();
	virtual bool init();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	virtual void update(ccTime dt);
	virtual void visit();
	virtual void doFlingDown(float deltaY);
	virtual void doFlingUp(float deltaY);

	PowerupButton* getCurrentPowerup();
	void addPowerup(PowerupType powerupType);
	void removeCurrentPowerup();


	virtual void buttonPressedCallback(CCObject* sender);

protected:
	PowerupButton* standardPowerupButton(PowerupType powerupType);
	virtual void removePowerupFinishedCallback(CCNode* sender);
#ifdef DEBUG_DRAW_SCROLLER
	virtual void draw();
#endif

};
#endif