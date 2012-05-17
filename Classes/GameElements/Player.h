#ifndef  _PLAYER_H_
#define  _PLAYER_H_

#include "cocos2d.h"
#include "Powerup.h"
#include "CustomColors.h"
#include "GameState.h"
#include "AIParameter.h"
#define DEFAULT_SPEED 100.50f
#define DEFAULT_SENSITIVITY 3.1f
#define MAX_MOVE_DISTANCE 2.2f
#define MAX_DISTANCE_BETWEEN_KEYFRAMES 5.0
#define PI 3.141593f

USING_NS_CC;

class Powerup;

class Player
{
	CC_SYNTHESIZE(float, m_angle, Angle);
	CC_SYNTHESIZE_READONLY(int, m_index, Index);
	CC_SYNTHESIZE(bool, m_isAlive, IsAlive);
	CC_SYNTHESIZE(bool, m_isAI, IsAI);
	CC_SYNTHESIZE_READONLY(CCPoint, m_oldPos, OldPosition);
	CC_SYNTHESIZE_READONLY(CCPoint, m_pos, Position);
	CC_SYNTHESIZE(float, m_inputValueL, InputL);
	CC_SYNTHESIZE(float, m_inputValueR, InputR);

public:
	float currentSpeed;
	ccColor3B color;
	AIParameterSet* aiParmSet;
protected:
    int m_borderX, m_borderY;
	std::vector<ccColor3B> colors;
    //TODO: what the hell is this count??
	int count;
	int gapLength;
	float m_currentWormLength;
	static const int maxCount=10000;
	int m_width, m_height;
	float m_speed;

public:
	//saves all the positions of the worm, in a linear fashion
	std::vector<CCPoint> positions;
	int score;
	float length;
	float sensitivity;

	Player(int index, int W, int H, int BX, int BY);
	void reset(ccTime dt);
	void die();
	bool hasMoved();

	bool isPenDown(){return (true);}
	void update(ccTime dt);

protected:
	void move(ccTime dt);

};

#endif