#include "CCControlCheckBox.h"


CCControlCheckBox* CCControlCheckBox::checkBoxFromSprites(CCSprite* checkedSprite, CCSprite* uncheckedSprite, CCScale9Sprite* backgroundSprite, CCLabelTTF* label, string settingKey)
{
	CCControlCheckBox *pRet = new CCControlCheckBox();
	pRet->initFromSprites(checkedSprite, uncheckedSprite, backgroundSprite, label, settingKey);
	pRet->autorelease();
	return pRet;
}
CCControlCheckBox* CCControlCheckBox::checkBoxFromFiles(const char* checkedIcon, const char* uncheckedIcon, const char* backgroundFile, CCRect rect, const char* text, const char* fontName, float fontSize, string settingKey)
{
	CCSprite *checkedSprite      = CCSprite::spriteWithFile(checkedIcon);	
    CCSprite *uncheckedSprite        = CCSprite::spriteWithFile(uncheckedIcon);
	CCLabelTTF* label = CCLabelTTF::labelWithString(text, fontName, fontSize);	
	CCScale9Sprite* backgroundSprite=CCScale9Sprite::scale9SpriteWithFile(backgroundFile, rect);
	return checkBoxFromSprites(checkedSprite, uncheckedSprite, backgroundSprite, label, settingKey);
}

bool CCControlCheckBox::initFromSprites(CCSprite* checkedSprite, CCSprite* uncheckedSprite, CCScale9Sprite* backgroundSprite, CCLabelTTF* label, string settingKey)
{
	if (!CCControl::init())
		return false;
	m_checkedSprite=checkedSprite;
	m_uncheckedSprite=uncheckedSprite;
	m_backgroundSprite=backgroundSprite;
	m_label=label;
	m_settingKey=settingKey;
	m_label->setColor(ccBLACK);
	this->addChild(m_checkedSprite, 1);
	this->addChild(m_uncheckedSprite, 1);
	this->addChild(m_label, 2);
	this->addChild(m_backgroundSprite, 0);

	// Set the default anchor point
	setIsRelativeAnchorPoint(true);
	setAnchorPoint(ccp(0.5f, 0.5f));

	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();

	m_checkedSprite->setScale(spriteScale);
	m_uncheckedSprite->setScale(spriteScale);

	//default margins
	m_marginH=16*uiScale;	
	m_marginV=12*uiScale;
	setIsTouchEnabled(true);

	setIsChecked(CCUserDefault::sharedUserDefault()->getBoolForKey(m_settingKey.c_str(), true));
	needsLayout();
	return true;
}

void CCControlCheckBox::setIsChecked(bool val)
{
	m_checked=val;
	if (m_checked)
	{
		m_checkedSprite->setIsVisible(true);
		m_uncheckedSprite->setIsVisible(false);
	}
	else
	{
		m_checkedSprite->setIsVisible(false);
		m_uncheckedSprite->setIsVisible(true);
	}
	sendActionsForControlEvents(CCControlEventValueChanged);        

}

void CCControlCheckBox::saveValue()
{
	CCUserDefault::sharedUserDefault()->setBoolForKey(m_settingKey.c_str(), m_checked); 
}



void CCControlCheckBox::setMargins(int marginH, int marginV)
{
	m_marginV=marginV;
	m_marginH=marginH;
	needsLayout();
}


void CCControlCheckBox::needsLayout()
{
	float padding=10;
	float width=m_marginH+m_checkedSprite->getContentSize().width*spriteScale+padding+m_label->getContentSize().width+m_marginH;
	float height=m_marginV+max(m_checkedSprite->getContentSize().height*spriteScale, m_label->getContentSize().height)+m_marginV;
	CCSize size=CCSizeMake(width, height);
	setContentSize(size);
	m_backgroundSprite->setContentSize(size);
	m_backgroundSprite->setPosition(width/2.0f, height/2.0f);
	m_checkedSprite->setPosition(ccp(m_marginH+m_checkedSprite->getContentSize().width*spriteScale/2.0f, height/2.0f));
	m_uncheckedSprite->setPosition(ccp(m_marginH+m_checkedSprite->getContentSize().width*spriteScale/2.0f, height/2.0f));
	m_label->setPosition(ccp(width-m_marginH-m_label->getContentSize().width/2.0f, height/2.0f));
}

bool CCControlCheckBox::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!isTouchInside(pTouch) || !getIsEnabled())
    {
		return false;
	}
    	
	sendActionsForControlEvents(CCControlEventTouchDown);
    return true;
}

void CCControlCheckBox::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{    
    if (isTouchInside(pTouch))
    {
		setIsChecked(!m_checked);
    }	
}

