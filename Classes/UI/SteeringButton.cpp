#include "SteeringButton.h"


SteeringButton* SteeringButton::steeringButtonFromFiles(const char* normalBackgroundFile, const char* pressedBackgroundFile, bool flippedX, bool flippedY)
{
	SteeringButton *pRet = new SteeringButton();
	pRet->initFromFiles(normalBackgroundFile, pressedBackgroundFile, flippedX, flippedY);
	pRet->autorelease();
	return pRet;
}

bool SteeringButton::initFromFiles(const char* normalBackgroundFile, const char* pressedBackgroundFile, bool flippedX, bool flippedY)
{
	if (!CCControl::init())
		return false;

	m_normalBackground=CCSprite::spriteWithFile(normalBackgroundFile);	
	if (!m_normalBackground)
		return false;

	pressedBackground=CCSprite::spriteWithFile(pressedBackgroundFile);
	if (!pressedBackground)
		return false;

	setIsRelativeAnchorPoint(true);
	setIsTouchEnabled(true);
	m_pressed=false;
	setContentSize(m_normalBackground->getContentSize());
	ccBlendFunc blendFunc={GL_ONE, GL_ONE_MINUS_SRC_COLOR};
	m_normalBackground->setAnchorPoint(ccp(0.5f, 0.5f));
	m_normalBackground->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));	
	m_normalBackground->setFlipX(flippedX);
	m_normalBackground->setFlipY(flippedY);
	addChild(m_normalBackground);
	pressedBackground->setAnchorPoint(ccp(0.5f, 0.5f));
	pressedBackground->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));	
	pressedBackground->setFlipX(flippedX);
	pressedBackground->setFlipY(flippedY);
	pressedBackground->setBlendFunc(blendFunc);
	addChild(pressedBackground, 2);
	pressedBackground->setOpacity(0);
	pressedBackground->setIsVisible(false);
	return true;
}

void SteeringButton::setIsPressed(bool pressed)
{
	if (pressed!=m_pressed)
	{	
		float fadeDuration=0.5f;
		m_pressed=pressed;
		if (pressed)
		{
			pressedBackground->setIsVisible(true);
			pressedBackground->runAction(CCFadeTo::actionWithDuration(fadeDuration, 255));
		}
		else
			pressedBackground->runAction(CCFadeTo::actionWithDuration(fadeDuration, 0));			
	}
}

bool SteeringButton::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!isTouchInside(pTouch) || !getIsEnabled())
		return false;
    
	sendActionsForControlEvents(CCControlEventTouchDown);
	setIsPressed(true);
    return true;
}

void SteeringButton::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{	
    
}
void SteeringButton::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{ 
	setIsPressed(false);

	m_nState = CCControlStateNormal;
	setIsHighlighted(false);

    if (isTouchInside(pTouch))
    {
		sendActionsForControlEvents(CCControlEventTouchUpInside);        
    }
	else
    {
		sendActionsForControlEvents(CCControlEventTouchUpOutside);        
    }
}

void SteeringButton::registerWithTouchDispatcher()
{
	CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 1, true);
}