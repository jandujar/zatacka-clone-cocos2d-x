/****************************************************************************
Copyright (c) Sonic Zhao | weibo:http://weibo.com/transong
Objc version authro pmanna | links:http://www.cocos2d-iphone.org/wiki/doku.php/tips:slider_widget
*/

#ifndef __CCMENU_ITEM_SLIDER_H__
#define __CCMENU_ITEM_SLIDER_H__

#include "cocos2d.h"

using namespace cocos2d;

class CCMenuItemSlider : public CCMenuItem, public CCRGBAProtocol
{
	
public:
	//properties
	void setMinValue(float minV){ m_minValue = minV; }
	void setMaxValue(float maxV){ m_maxValue = maxV; }
	void setValue(float v);

	float getMinValue(){ return m_minValue; }
	float getMaxValue(){ return m_maxValue; }
	float getValue(){ return m_value; }
	CCNode* getKnobImage(){ return m_knobImage; }
    //
    
	void selected()
	{
		dynamic_cast<CCRGBAProtocol*>(m_knobImage)->setColor(ccc3(150,150,150));
	}
	void unselected()
	{
		dynamic_cast<CCRGBAProtocol*>(m_knobImage)->setColor(ccc3(255,255,255));
	}
	void setIsEnabled(bool bEnabled)
	{

		m_bIsEnabled = bEnabled;
		if(bEnabled)
			setColor(ccc3(255,255,255));
		else
			setColor(ccc3(200,200,200));
	}

	//RGBAProtocol

		// super methods
        virtual void setColor(const ccColor3B& color);
        virtual const ccColor3B& getColor();
        virtual void setOpacity(GLubyte opacity);
        virtual GLubyte getOpacity();

   		virtual void setIsOpacityModifyRGB(bool bValue) {CC_UNUSED_PARAM(bValue);}
	    virtual bool getIsOpacityModifyRGB(void) { return false;}
	
    //
    //the reason to use CCNode instead of string is it's efficient to generate a ccsprite from a spritesheet
	static CCMenuItemSlider* itemFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite);	
	static CCMenuItemSlider* itemFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite, CCObject* target, SEL_MenuHandler selector);	
	bool initFromNormalSprite(CCNode* trackSprite, CCNode* knobSprite, CCNode* progressSprite, CCObject* target, SEL_MenuHandler selector);

	void dragToPoint(CCPoint aPoint);
protected:
private:
	float			m_minValue;
	float			m_maxValue;
	float			m_value;

	bool			m_isVertical;
    
	CCNode*         m_trackImage;
	CCNode*         m_knobImage;
	CCNode*         m_progressImage;

	//you can remove it if you like
	//CCLabelTTF*     m_labelValue;

};

class CCMenuSlider : public CCMenu
{
public:
    
	static CCMenuSlider* menuWithItems(CCMenuItem* item, ...);
	static CCMenuSlider*menuWithItem(CCMenuItem* item);
    
	virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
	
protected:
private:
};

#endif //__CCMENU_ITEM_SLIDER_H__