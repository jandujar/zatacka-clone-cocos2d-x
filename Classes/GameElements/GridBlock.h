#ifndef  _PLAYER_H_
#define  _PLAYER_H_

#include "Powerup.h"
#include "Player.h"
#include "cocos2d.h"

class GridBlock
{
	CC_SYNTHESIZE_READONLY(ccColor3B, m_blockColor, BlockColor);
	
protected:
	ccColor3B m_destinationColor, m_sourceColor;
	int m_ownerIndex;
	CCPoint m_position;
	float m_scale;
	float m_transition;


public:
	static const ccColor3B defaultColor;
	static const float transitionTime=1000.0f;

	GridBlock(CCPoint position, float scale);
	void FadeOut();
	void FadeToPlayer(ccColor3B playerColor);
	void Update(ccTime dt);
	bool isFading(){return (m_transition<transitionTime);}

};

#endif