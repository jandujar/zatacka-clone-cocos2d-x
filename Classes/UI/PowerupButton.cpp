#include "PowerupButton.h"

PowerupButton* PowerupButton::powerupButton(PowerupType powerupType)
{
	PowerupButton *pRet = new PowerupButton();
	pRet->init(powerupType);
	pRet->autorelease();
	return pRet;
}

bool PowerupButton::init(PowerupType powerupType)
{
	if (!CCControl::init())
		return false;	
	setIsRelativeAnchorPoint(true);
	CCSprite* buttonSprite;
	m_powerupType=powerupType;
	switch(powerupType)
	{
	case (LICKETY_SPEED):
		buttonSprite = CCSprite::spriteWithFile(GameState::fileNameFull("heart.png").c_str());
		break;
	case (HEAD_OVER_HEELS):
		buttonSprite=CCSprite::spriteWithFile(GameState::fileNameFull("circleArrow.png").c_str());
		break;
	default:
		return false;
		break;
	}	
	
	setContentSize(buttonSprite->getContentSize());
	buttonSprite->setAnchorPoint(ccp(0.5f, 0.5f));
	buttonSprite->setOpacity(50);
	buttonSprite->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));		
	addChild(buttonSprite);
	return true;
}