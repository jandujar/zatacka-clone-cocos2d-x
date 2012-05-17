#include "CCClippedNode.h"

CCClippedNode* CCClippedNode::clippedNode(CCRect clippingRect)
{
	CCClippedNode *pRet = new CCClippedNode();
	pRet->autorelease();
	pRet->setClippingRegion(clippingRect);
	pRet->setIsRelativeAnchorPoint(true);
	pRet->setAnchorPoint(ccp(0.5f, 0.5f));
	return pRet;
}

void CCClippedNode::visit()
{
	glPushMatrix();    
	glEnable(GL_SCISSOR_TEST);
	CCPoint origin=this->convertToWorldSpace(m_clippingRegion.origin);
	CCRect clippingRect=CCRectMake(m_clippingRegion.origin.x + getPositionInPixels().x, m_clippingRegion.origin.y + getPositionInPixels().y,
            m_clippingRegion.size.width, m_clippingRegion.size.height);
    glScissor(m_clippingRegion.origin.x + getPositionInPixels().x, m_clippingRegion.origin.y + getPositionInPixels().y,
            m_clippingRegion.size.width, m_clippingRegion.size.height);
	CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
    glPopMatrix();
}

void CCClippedNode::setClippingRegion(CCRect clippingRegion)
{
	// keep the original region coordinates in case the user wants them back unchanged
    m_clippingRegionInNodeCoordinates = clippingRegion;
	//setPosition(m_clippingRegionInNodeCoordinates.origin);
	//setContentSize(m_clippingRegionInNodeCoordinates.size);
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();	
	
    // glScissor requires the coordinates to be rotated to portrait mode
	switch (CCDirector::sharedDirector()->getDeviceOrientation())
    {
        default:
        case kCCDeviceOrientationPortrait:
            // do nothing, coords are already correct
            break;
            
        case kCCDeviceOrientationPortraitUpsideDown:
            clippingRegion.origin.x = screenSize.width - clippingRegion.size.width - clippingRegion.origin.x;
            clippingRegion.origin.y = screenSize.height - clippingRegion.size.height - clippingRegion.origin.y;
            break;
            
        case kCCDeviceOrientationLandscapeLeft:
            clippingRegion.origin = CCPointMake(clippingRegion.origin.y, screenSize.width - clippingRegion.size.width - clippingRegion.origin.x);
            clippingRegion.size = CCSizeMake(clippingRegion.size.height, clippingRegion.size.width);
            break;
            
        case kCCDeviceOrientationLandscapeRight:
            clippingRegion.origin = CCPointMake(screenSize.height - clippingRegion.size.height - clippingRegion.origin.y, clippingRegion.origin.x);
            clippingRegion.size = CCSizeMake(clippingRegion.size.height, clippingRegion.size.width);
            break;
    }
    
    // convert to retina coordinates if needed
    clippingRegion = CC_RECT_POINTS_TO_PIXELS(clippingRegion);
    
    // respect scaling
	float s=getScaleX();
    m_clippingRegion = CCRectMake(clippingRegion.origin.x * getScaleX(), clippingRegion.origin.y * getScaleY(), 
                            clippingRegion.size.width * getScaleX(), clippingRegion.size.height * getScaleY());
}

void CCClippedNode::setScale(float newScale)
{
	CCNode::setScale(newScale);
    // re-adjust the clipping region according to the current scale factor
	setClippingRegion(m_clippingRegionInNodeCoordinates);
}

//CRGBA protocol
void CCClippedNode::setColor(const ccColor3B& color)
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

const ccColor3B& CCClippedNode::getColor(void)
{
	return m_tColor;
}


void CCClippedNode::setOpacity(GLubyte opacity)
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

GLubyte CCClippedNode::getOpacity()
{
	return m_cOpacity;
}


void CCClippedNode::setIsOpacityModifyRGB(bool opacityModifyRGB)
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

bool CCClippedNode::getIsOpacityModifyRGB()
{
	return m_bIsOpacityModifyRGB;
}

#ifdef DEBUG_DRAW_CLIPPING
void CCClippedNode::draw()
{
	CCNode::draw();
	    // draw bounding box
	//m_clippingRegion.origin.x

	CCSize s = m_clippingRegion.size;
    CCPoint vertices[4] = {
		ccp(m_clippingRegion.origin.x,m_clippingRegion.origin.y), ccp(m_clippingRegion.origin.x+s.width,m_clippingRegion.origin.y),
		ccp(m_clippingRegion.origin.x+s.width,m_clippingRegion.origin.y+s.height), ccp(m_clippingRegion.origin.x,m_clippingRegion.origin.y+s.height)
    };
    ccDrawPoly(vertices, 4, true);
}
#endif