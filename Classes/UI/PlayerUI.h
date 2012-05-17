#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "CCClippedNode.h"
#include "PowerupButton.h"
#include "PowerupScroller.h"
#include "SteeringButton.h"
#include "GameState.h"

class PlayerUI: public CCNode, public CCRGBAProtocol
{
	//CCRGBAProtocol
	CC_PROPERTY(GLubyte, m_cOpacity, Opacity); 
	CC_PROPERTY_PASS_BY_REF(ccColor3B, m_tColor, Color);
	CC_PROPERTY(bool, m_bIsOpacityModifyRGB, IsOpacityModifyRGB);

	CC_SYNTHESIZE(int, m_playerIndex, PlayerIndex);
	CC_SYNTHESIZE_READONLY(float, m_inputL, InputL);
	CC_SYNTHESIZE_READONLY(float, m_inputR, InputR);



protected:
	PowerupScroller* m_powerupScroller;
	SteeringButton* m_buttonL, *m_buttonR;
	SEL_MenuHandler	m_pfnAction;
	CCObject* m_target;
	float spriteScale;
	float uiScale;

	virtual void steeringButtonDownCallback(CCObject* sender);
	virtual void steeringButtonUpCallback(CCObject* sender);
	virtual void powerupPressedCallback(CCObject* pSender);
public:
	static PlayerUI* playerUI(int playerIndex);
	virtual bool init(int playerIndex);
	void addOnPowerupButtonPressedCallback(CCObject* target, SEL_MenuHandler action);

	PowerupButton* getCurrentPowerup();
	void addPowerup(PowerupType powerupType);
	void removeCurrentPowerup();
};

#endif