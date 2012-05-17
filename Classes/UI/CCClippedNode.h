#ifndef __CCCLIPPED_SPRITE_H__
#define __CCCLIPPED_SPRITE_H__

#include "CCGL.h"
#include "cocos2d.h"
USING_NS_CC;
//#define DEBUG_DRAW_CLIPPING

class CCClippedNode : public CCNode, public CCRGBAProtocol
{	
	//CCRGBAProtocol
	CC_PROPERTY(GLubyte, m_cOpacity, Opacity); 
	CC_PROPERTY_PASS_BY_REF(ccColor3B, m_tColor, Color);
	CC_PROPERTY(bool, m_bIsOpacityModifyRGB, IsOpacityModifyRGB);

protected:
    CCRect m_clippingRegion;
	CC_SYNTHESIZE_READONLY(CCRect, m_clippingRegionInNodeCoordinates, ClippingRegion);
	virtual void setClippingRegion(CCRect clippingRect);

public:
	static CCClippedNode* clippedNode(CCRect clippingRect);	
	virtual void visit();
	virtual void setScale(float newScale);
#ifdef DEBUG_DRAW_CLIPPING
	virtual void draw();
#endif
};

#endif
