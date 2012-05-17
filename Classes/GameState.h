#ifndef  _GAME_STATE_H_
#define  _GAME_STATE_H_

#include "cocos2d.h"
#include <sstream>
#include <fstream>
#include <map>
USING_NS_CC;

enum
{
	NORTH=0,
	SOUTH=1,
	EAST=2,
	WEST=3
};
typedef unsigned int UIPosition;


class GameState: public CCObject
{
	CC_SYNTHESIZE_READONLY(float, m_uiScale, UIScale);
	CC_SYNTHESIZE_READONLY(float, m_spriteScale, SpriteScale);
	//this is basically the size of the sprites used, relative to the 480px standard. Used for Scale9Sprites
	CC_SYNTHESIZE_READONLY(float, m_spriteSetFactor, SpriteSetFactor);
	//this is simply SpriteSetFactor*480
	CC_SYNTHESIZE_READONLY(int, m_spriteSize, SpriteSize);
	
	CC_SYNTHESIZE(int, m_numPlayers, NumPlayers);
	CC_SYNTHESIZE(int, m_numAI, NumAI);
	//maps UI positions to color index

public:
	map<UIPosition, int> playerUIColorMap;

	static GameState* sharedGameState();
	static std::string fileNameFull(const char* fileName);	
	
	static void cacheTextures();

protected:
	bool init ();
	
	
};

#endif