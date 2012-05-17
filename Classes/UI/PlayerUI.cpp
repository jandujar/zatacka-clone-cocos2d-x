#include "PlayerUI.h"

PlayerUI* PlayerUI::playerUI(int index)
{
	PlayerUI *pRet = new PlayerUI();
	pRet->init(index);
	
	pRet->autorelease();
	return pRet;
}

bool PlayerUI::init(int index)
{
	m_playerIndex=index;



	CCSize size = CCDirector::sharedDirector()->getWinSize();

	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();
	float buttonScrollerOverlap=24;

	m_powerupScroller=PowerupScroller::powerupScroller();
	m_buttonL = SteeringButton::steeringButtonFromFiles(GameState::fileNameFull("leftButton.png").c_str(), GameState::fileNameFull("leftButtonPressed.png").c_str(), false);
	m_buttonR = SteeringButton::steeringButtonFromFiles(GameState::fileNameFull("leftButton.png").c_str(), GameState::fileNameFull("leftButtonPressed.png").c_str(), true);	
	setContentSize(CCSizeMake(m_buttonL->getContentSize().width+m_buttonR->getContentSize().width+m_powerupScroller->getContentSize().width-uiScale*buttonScrollerOverlap*2, m_powerupScroller->getContentSize().height));	
	//small offset in the scroller
	m_powerupScroller->setPosition(getContentSize().width/2.0f, m_powerupScroller->getContentSize().height/2.0f-uiScale*5);
	m_powerupScroller->setAnchorPoint(ccp(0.5f, 0.5f));	
	m_powerupScroller->addTargetWithActionForControlEvents(this, menu_selector(PlayerUI::powerupPressedCallback), CCControlEventTouchUpInside|CCControlEventTouchUpOutside);	
	addChild(m_powerupScroller);

	m_buttonL->setAnchorPoint(ccp(1.0f, 0.0f));
	//32 there because there is an overlap
	
	m_buttonL->setPosition(ccp(getContentSize().width/2.0f-m_powerupScroller->getContentSize().width/2.0f+uiScale*buttonScrollerOverlap, 0));
	m_buttonL->setScale(spriteScale);
	m_buttonL->addTargetWithActionForControlEvents(this, menu_selector(PlayerUI::steeringButtonDownCallback), CCControlEventTouchDown);
	m_buttonL->addTargetWithActionForControlEvents(this, menu_selector(PlayerUI::steeringButtonUpCallback), CCControlEventTouchUpInside|CCControlEventTouchUpOutside);

	addChild(m_buttonL, 2);

	m_buttonR->setAnchorPoint(ccp(0.0f, 0.0f));
	m_buttonR->setPosition(ccp(getContentSize().width/2.0f+m_powerupScroller->getContentSize().width/2.0f-uiScale*24, 0));
	m_buttonR->setScale(spriteScale);
	m_buttonR->addTargetWithActionForControlEvents(this, menu_selector(PlayerUI::steeringButtonDownCallback), CCControlEventTouchDown);
	m_buttonR->addTargetWithActionForControlEvents(this, menu_selector(PlayerUI::steeringButtonUpCallback), CCControlEventTouchUpInside|CCControlEventTouchUpOutside);

	addChild(m_buttonR, 2);

	m_inputL=0.0f; m_inputR=0.0f;
	m_pfnAction=0;
	m_target=0;
	return true;
}

void PlayerUI::addOnPowerupButtonPressedCallback(CCObject* target, SEL_MenuHandler action)
{
	m_pfnAction=action;
	m_target=target;
}


void PlayerUI::steeringButtonDownCallback(CCObject* pSender)
{
	SteeringButton* button=(SteeringButton*)pSender;
	if (button==m_buttonL)
		m_inputL=1.0;
	else if (button==m_buttonR)
		m_inputR=1.0f;	
}

void PlayerUI::steeringButtonUpCallback(CCObject* pSender)
{
	SteeringButton* button=(SteeringButton*)pSender;
	if (button==m_buttonL)
		m_inputL=0.0;
	else if (button==m_buttonR)
		m_inputR=0.0f;	
}

void PlayerUI::powerupPressedCallback(CCObject* pSender)
{
	//pass through the event
	if (m_target && m_pfnAction)
	{
		(m_target->*m_pfnAction)(this);
	}
}


PowerupButton* PlayerUI::getCurrentPowerup()
{
	return m_powerupScroller->getCurrentPowerup();
}

void PlayerUI::addPowerup(PowerupType powerupType)
{
	m_powerupScroller->addPowerup(powerupType);
}

void PlayerUI::removeCurrentPowerup()
{
	m_powerupScroller->removeCurrentPowerup();
}



//CRGBA protocol
void PlayerUI::setColor(const ccColor3B& color)
{
	m_tColor=color;
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setColor(m_tColor);
		}
	}
}

const ccColor3B& PlayerUI::getColor(void)
{
	return m_tColor;
}


void PlayerUI::setOpacity(GLubyte opacity)
{
	m_cOpacity = opacity;
    
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setOpacity(opacity);
		}
	}

}

GLubyte PlayerUI::getOpacity()
{
	return m_cOpacity;
}


void PlayerUI::setIsOpacityModifyRGB(bool opacityModifyRGB)
{
	m_bIsOpacityModifyRGB=opacityModifyRGB;
		CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setIsOpacityModifyRGB(opacityModifyRGB);
		}
	}
}

bool PlayerUI::getIsOpacityModifyRGB()
{
	return m_bIsOpacityModifyRGB;
}