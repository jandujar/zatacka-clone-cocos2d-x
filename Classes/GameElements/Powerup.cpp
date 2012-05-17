#include "Powerup.h"


Powerup::Powerup(PowerupType type, CCPoint pos)
{
	m_powerupType = type;
	switch (m_powerupType)
	{
	case LICKETY_SPEED:
		color = ccVIOLET;
		m_lifetime = 5;
		break;
	case FLIPSIDE:
		color = ccGREEN;
		m_lifetime = 5;
		break;                
	case CHANGE_PLACES:
		color = ccLIGHTBLUE;
		m_lifetime = 5;
		break;
	case HEAD_OVER_HEELS:
		color = ccORANGE;
		m_lifetime = 5;
		break;
	case STOP_THE_CLOCK:
		color = ccRED;
		m_lifetime = 5;
		break;
	}
	m_pos=pos;
	m_isActive=false;
}
	
void Powerup::activate(vector<Player*>& players, int ownerIndex)
{
	CC_ASSERT(ownerIndex<players.size());
	m_players=players;
	m_ownerIndex=ownerIndex;
	m_owner=m_players[m_ownerIndex];
	m_isActive = true;
	switch (m_powerupType)
	{
	case LICKETY_SPEED:
		m_owner->currentSpeed*=SPEEDUP_VALUE;
		//p.Sensitivity *= 1.25f;
		break;
	case STOP_THE_CLOCK:
		m_owner->currentSpeed=-fabs(m_owner->currentSpeed);
		break;                
	case HEAD_OVER_HEELS:
		//flip around
		/*
		if (p.positions.Count >= 2)
		{
		p.position = p.positions[0];
		p.oldPosition = p.positions[1];
		p.angle = (float)(atan2((double)(p.position.Y - p.oldPosition.Y), (double)(p.position.X - p.oldPosition.X)));
		p.lineList.Reverse();
		p.positions.Reverse();
		for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++)
		{
		p.positionsGrid[i, j].Reverse();
		}
		}*/
		break;
	}
}

void Powerup::update(ccTime dt)
{
	m_lifetime-=dt;
	if (m_lifetime<0)
		die();
}

void Powerup::die()
{
	switch (m_powerupType)
	{
		case LICKETY_SPEED:
			//p.Sensitivity /= 1.25f;			
			m_owner->currentSpeed/=SPEEDUP_VALUE;
			break;
		case STOP_THE_CLOCK:
			m_owner->currentSpeed=fabs(m_owner->currentSpeed);			
			break;
		default:
			break;
	}
	m_isActive = false;
}