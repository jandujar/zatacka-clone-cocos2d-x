#include "UserFadeRibbon.h"

UserFadeRibbon * UserFadeRibbon::ribbonWithWidth(float w, const char *path, float length, const ccColor4B& color, float fade)
{
	UserFadeRibbon *pRet = new UserFadeRibbon();
	if(pRet && pRet->initWithWidth(w, path, length, color, fade))
	{
		pRet->autorelease();
		pRet->disableFade();
		return pRet;
	}
	CC_SAFE_DELETE(pRet)
	return NULL;
}

void UserFadeRibbon::disableFade()
{
	fadingOut=false;
	
}
void UserFadeRibbon::update(ccTime delta)
{
	
	m_fCurTime += delta;
	m_fDelta = delta;
	if (fadingOut)
	{
		currentFadeTime+=delta;
		if (currentFadeTime>endFadeTime)
		{

			fadingOut=false;
			if (m_callbackOnFadeOver) 
				(m_target->*m_callbackOnFadeOver)(this->m_pParent);
		}
	}
}

void UserFadeRibbon::draw()
{
	CCNode::draw();

	if (m_pSegments->count() > 0)
	{
		// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
		// Needed states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY
		// Unneeded states: GL_COLOR_ARRAY
		glDisableClientState(GL_COLOR_ARRAY);

		glBindTexture(GL_TEXTURE_2D, m_pTexture->getName());

        bool newBlend = ( m_tBlendFunc.src != CC_BLEND_SRC || m_tBlendFunc.dst != CC_BLEND_DST ) ? true : false;
        if( newBlend )
        {
            glBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
        }

		if(m_pSegments && m_pSegments->count() > 0)
		{
			CCRibbonSegment* seg;			
			CCMutableArray<CCRibbonSegment*>::CCMutableArrayIterator it;
			int segCount=0;
			for( it = m_pSegments->begin(); it != m_pSegments->end(); it++)
			{
				seg = (CCRibbonSegment*)*it;
				if (fadingOut)
					seg->draw(currentFadeTime/endFadeTime*startFadeTime, m_fFadeTime, m_tColor);
				else
					seg->draw(0, m_fFadeTime, m_tColor);
				segCount++;
			}
		}

		if( newBlend )
		{
			glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
		}

		// restore default GL state
		glEnableClientState( GL_COLOR_ARRAY );
	}
}

void UserFadeRibbon::fadeOut(ccTime fadeTime, CCObject* target, SEL_CallFuncN callbackOnFadeOver)
{
	startFadeTime=m_fCurTime;
	fadingOut=true;
	endFadeTime=fadeTime;
	currentFadeTime=0.0f;
	m_target=target;
	m_callbackOnFadeOver=callbackOnFadeOver;
}