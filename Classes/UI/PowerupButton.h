#ifndef __POWERUP_BUTTON_H__
#define __POWERUP_BUTTON_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "Powerup.h"
#include "Player.h"

USING_NS_CC;




class PowerupButton : public CCControl
{
	CC_SYNTHESIZE_READONLY(PowerupType, m_powerupType, PowerupType);

public:
	static PowerupButton* powerupButton(PowerupType powerupType);
	virtual bool init(PowerupType powerupType);
};

#endif