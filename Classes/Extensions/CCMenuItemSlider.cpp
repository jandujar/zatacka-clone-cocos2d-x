#include "CCMenuItemSlider.h"
#include "CCPointExtension.h"


CCMenuItemSlider* CCMenuItemSlider::itemFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite)
{
   return CCMenuItemSlider::itemFromNormalSprite(trackSprite, knobSprite, progressSprite, NULL, NULL);
}

CCMenuItemSlider* CCMenuItemSlider::itemFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite, CCObject* target, SEL_MenuHandler selector)
{
	CCMenuItemSlider *pRet = new CCMenuItemSlider();
	pRet->initFromNormalSprite(trackSprite, knobSprite, progressSprite, target, selector); 
	pRet->autorelease();
	return pRet;
}

bool CCMenuItemSlider::initFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite, CCObject* target, SEL_MenuHandler selector)
{

	assert(trackSprite != NULL && knobSprite != NULL && progressSprite != NULL );

	if( CCMenuItem::initWithTarget(target, selector) ) 
	{
		m_trackImage    = trackSprite;
		m_knobImage		= knobSprite;
		m_progressImage		= progressSprite;
        
		m_trackImage->setAnchorPoint(ccp(0, 0));
		m_progressImage->setAnchorPoint(ccp(0, 0));

		m_isVertical = (m_trackImage->getContentSize().height > m_trackImage->getContentSize().width);

		if (m_isVertical)
		{
			float offset = (m_knobImage->getContentSize().width - m_trackImage->getContentSize().width)/2;
			m_trackImage->setPosition(ccp(offset, 0));
			m_progressImage->setPosition(ccp(offset, 0));

			setContentSize(CCSizeMake(m_knobImage->getContentSize().width, m_trackImage->getContentSize().height));
		}
		else
		{
			float offset = (m_knobImage->getContentSize().height - m_trackImage->getContentSize().height)/2;
			m_trackImage->setPosition(ccp(0, offset));
			m_progressImage->setPosition(ccp(0, offset));

			setContentSize(CCSizeMake(m_trackImage->getContentSize().width, m_knobImage->getContentSize().height));
		}
		
		addChild(trackSprite, 1);
		addChild(knobSprite, 3);
		addChild(progressSprite, 2);
		
		m_minValue	= 0.0f;
		m_maxValue	= 100.0f;

// 		char temp_char[16];
// 		sprintf(temp_char, "%.1f", m_minValue);
// 		CCLabelTTF* labelMin = CCLabelTTF::labelWithString(temp_char, "Arial", 10);
// 		labelMin->setPosition(ccp(0, 0));
//         addChild(labelMin, 3);
//        
// 		sprintf(temp_char, "%.1f", m_maxValue);
// 		CCLabelTTF* labelMax = CCLabelTTF::labelWithString(temp_char, "Arial", 10);
// 		labelMax->setPosition(ccp(m_trackImage->getContentSize().width, 0));
// 		addChild(labelMax);

		//remove it if you like
		//m_labelValue = CCLabelTTF::labelWithString("", "Arial", 10);
		//m_labelValue->setPosition(ccp(m_trackImage->getContentSize().width/2, 0));
		//addChild(m_labelValue, 3);
		//
		setValue(50.0f);
	}

	return true;
}

void CCMenuItemSlider::setValue(float aValue)
{
	float	valueRatio;

	if (m_isVertical)
		valueRatio	= (m_trackImage->getContentSize().height - m_knobImage->getContentSize().height) / (m_maxValue - m_minValue);
	else
		valueRatio	= (getContentSize().width - m_knobImage->getContentSize().width) / (m_maxValue - m_minValue);

	if (aValue < m_minValue)
		m_value	= m_minValue;
	else if (aValue > m_maxValue)
		m_value	= m_maxValue;
	else
		m_value	= aValue;

	if (m_isVertical)
		m_knobImage->setPosition(ccp(getContentSize().width / 2,
		(m_value - m_minValue) * valueRatio + m_knobImage->getContentSize().height / 2));
	else
		m_knobImage->setPosition(ccp((m_value - m_minValue) * valueRatio + m_knobImage->getContentSize().width / 2,
		getContentSize().height / 2));
    
	CCSprite* progressSprite=(CCSprite*)m_progressImage;

	//m_progressImage->
	float pos=(m_value - m_minValue) * valueRatio + m_knobImage->getContentSize().width / 2;
	CCRect textureRect          = progressSprite->getTextureRect();
    textureRect                 = CCRectMake(textureRect.origin.x, textureRect.origin.y, pos, textureRect.size.height);
	progressSprite->setTextureRect(textureRect);


	//remove it if you like
	//char temp_char[16];
	//sprintf(temp_char, "%.1f", m_value);
	//m_labelValue->setString(temp_char);
}

void CCMenuItemSlider::setOpacity(GLubyte opacity)
    {
        dynamic_cast<CCRGBAProtocol*>(m_knobImage)->setOpacity(opacity);
		dynamic_cast<CCRGBAProtocol*>(m_trackImage)->setOpacity(opacity);
		dynamic_cast<CCRGBAProtocol*>(m_progressImage)->setOpacity(opacity);
    }
    void CCMenuItemSlider::setColor(const ccColor3B& color)
    {
        dynamic_cast<CCRGBAProtocol*>(m_knobImage)->setColor(color);
        dynamic_cast<CCRGBAProtocol*>(m_trackImage)->setColor(color);
        dynamic_cast<CCRGBAProtocol*>(m_progressImage)->setColor(color);
	 
    }
    GLubyte CCMenuItemSlider::getOpacity()
    {
        return dynamic_cast<CCRGBAProtocol*>(m_knobImage)->getOpacity();
    }
    const ccColor3B& CCMenuItemSlider::getColor()
    {
        return dynamic_cast<CCRGBAProtocol*>(m_knobImage)->getColor();
    }


void CCMenuItemSlider::dragToPoint(CCPoint aPoint)
{
	float	valueRatio;
	float	absValue;

	if (m_isVertical) {
		valueRatio	= (m_maxValue - m_minValue) / (getContentSize().height - m_knobImage->getContentSize().height);
		absValue	= aPoint.y - m_knobImage->getContentSize().height / 2;
	} else {
		valueRatio	= (m_maxValue - m_minValue) / (getContentSize().width - m_knobImage->getContentSize().width);
		absValue	= aPoint.x - m_knobImage->getContentSize().width / 2;
	}

	setValue(m_minValue + absValue * valueRatio);

	activate();

}



//Menu
CCMenuSlider* CCMenuSlider::menuWithItems(CCMenuItem* item, ...)
{
	va_list args;
	va_start(args,item);
	CCMenuSlider *pRet = new CCMenuSlider();
	if (pRet && pRet->initWithItems(item, args))
	{
		pRet->autorelease();
		va_end(args);
		return pRet;
	}
	va_end(args);
	CC_SAFE_DELETE(pRet)
		return NULL;
}

CCMenuSlider* CCMenuSlider::menuWithItem(CCMenuItem* item)
{
	return menuWithItems(item, NULL);
}

void CCMenuSlider::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	//CCMenuItemSlider *currentItem = (CCMenuItemSlider*)itemForTouch(touch);
	
	CCPoint touchLocation = m_pSelectedItem->convertTouchToNodeSpace(touch);
	CCMenuItemSlider* sliderItem = dynamic_cast<CCMenuItemSlider*>(m_pSelectedItem);
	if (sliderItem)
		((CCMenuItemSlider*)m_pSelectedItem)->dragToPoint(touchLocation);
	
}

bool CCMenuSlider::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	CC_UNUSED_PARAM(event);
	if (m_eState != kCCMenuStateWaiting || ! m_bIsVisible)
	{
		return false;
	}

	for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
	{
		if (c->getIsVisible() == false)
		{
			return false;
		}
	}

	CCMenuItem *currentItem = itemForTouch(touch);
	if (currentItem)
	{   
		CCMenuItemSlider* sliderItem = dynamic_cast<CCMenuItemSlider*>(currentItem);
		if (!sliderItem)
			return CCMenu::ccTouchBegan(touch, event);
		m_pSelectedItem = currentItem;
		m_eState = kCCMenuStateTrackingTouch;
		currentItem->selected();		

		CCPoint touchLocation = currentItem->convertTouchToNodeSpace(touch);

		sliderItem->dragToPoint(touchLocation);

		return true;
	}
	return false;
}