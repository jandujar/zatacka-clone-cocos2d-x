#ifndef CCScale9Sprite_h__
#define CCScale9Sprite_h__

#include "cocos2d.h"
#include "GameState.h"
//  Copyright Eli Yukelzon, base on code by Jose Antonio Andújar Clavell 
//
//  License http://creativecommons.org/licenses/by/3.0/

USING_NS_CC;

class CCScale9Sprite : 	public CCNode, public CCRGBAProtocol
{

	CCSpriteBatchNode *scale9Image;
	CCSprite *topLeft;
	CCSprite *top;
	CCSprite *topRight;
	CCSprite *left;
	CCSprite *centre;
	CCSprite *right;
	CCSprite *bottomLeft;
	CCSprite *bottom;
	CCSprite *bottomRight;
	CCSize baseSize;
	CCRect resizableRegion;

	//if the preferredSize component is given as -1, it is ignored
	CC_SYNTHESIZE(CCSize, preferredSize, PreferredSize); 

	// texture RGBA


	CC_PROPERTY(GLubyte, m_cOpacity, Opacity); 
	CC_PROPERTY_PASS_BY_REF(ccColor3B, m_tColor, Color);
	CC_PROPERTY(bool, m_bIsOpacityModifyRGB, IsOpacityModifyRGB);
    
    //for outlines
    virtual void setIsOutlineOnly(bool value);
protected:
	// opacity and RGB protocol
	ccColor3B m_sColorUnmodified;

	CCScale9Sprite();
	virtual bool init();

public:
	static CCScale9Sprite* scale9SpriteWithFile(const char* file, const CCRect & centreRegion);
	CCScale9Sprite(const char* file, const CCRect & centreRegion);
	virtual void draw();
	void setContentSize(const CCSize &size);


	
	//TODO: this might break things
	static CCScale9Sprite* node()
	{ 
		CCScale9Sprite *pRet = new CCScale9Sprite();
		if (pRet && pRet->init())
	{ 
		pRet->autorelease(); 
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
	}; 

	//LAYER_NODE_FUNC(CCScale9Sprite);
};
#endif // CCScale9Sprite_h__
