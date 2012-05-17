#ifndef  _POWERUP_H_
#define  _POWERUP_H_

#include "cocos2d.h"
#include "Player.h"
#include "CustomColors.h"

#define SPEEDUP_VALUE 1.5f

USING_NS_CC;

class Player;
enum PowerupType
{
	DISABLED_POWERUP,
	CHANGE_PLACES,
    LICKETY_SPEED,
    FLIPSIDE,
    HEAD_OVER_HEELS,
    STOP_THE_CLOCK
};


class Powerup
{
	CC_SYNTHESIZE_READONLY(PowerupType, m_powerupType, PowerupTpe);
	CC_SYNTHESIZE_READONLY(CCPoint, m_pos, Position);
	CC_SYNTHESIZE(bool, m_isActive, IsActive);

	ccColor3B color;
	
protected:
	float m_lifetime;
	vector<Player*> m_players;
	Player* m_owner;
	int m_ownerIndex;
public:
	Powerup(PowerupType type, CCPoint pos);
	void activate(vector<Player*>& players, int ownerIndex);
	void update(ccTime dt);
	void die();
};


#endif