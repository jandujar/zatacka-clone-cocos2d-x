#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "PowerupScroller.h"
#include "PlayerUI.h"
#include "LayoutSet.h"
#include "MenuScene.h"
#include "GameState.h"
#include "Player.h"
#include "PlayerChoiceScene.h"
#include "CCRibbon.h"
#include <queue>
USING_NS_CC;

#define DEFAULT_COLOR ccc3(40, 40, 40)
#define BORDER 2*uiScale

typedef unsigned int UIPosition;


class HelloWorld : public cocos2d::CCLayer
{
protected:
	//UI
	
	
	//maps UI position to UI
	vector<PlayerUI*> playerUI;
	
	float spriteScale;
	float uiScale;
	//Grid
	int numBlocksX, numBlocksY;
	float gridBlockWidth, gridBlockHeight;
	//defines the max y index of "safe blocks": blocks that are never blocked by the UI. Safe blocks are used for 
	//powerups and starting positions
	int safeBlockMin, safeBlockMax;
	CCPoint gridOrigin;
	CCSize gridSize;
	CCSprite** gridBlocksHighlighted;

	//Logic Grid
	int* logicGrid;
	int* tempLogicGrid;
	int numLogicBlocksX, numLogicBlocksY;
	queue<int> logicQueueX, logicQueueY, logicQueueIndex;
		
	unsigned char bestCowardnessFront;
	unsigned char bestCowardnessSides;
	unsigned char bestCowardnessDiags;
	unsigned char bestStartingSearchDistance;
	//Players
	
	vector<Player*> players;
	vector<CCMotionStreak*> wormStreaks;
	vector<CCMotionStreak*> wormStreakOverlays;

	int numPlayers, numAI;

	//Powerups
	vector<Powerup*> activePowerups;
	//map of sprites (index of (u,v) is u*numBlocksY+v)
	CCSpriteBatchNode* powerupSpriteBatch;
	map<int, CCSprite*> powerupSprites;
	//particles
	CCParticleSystem*	m_emitterBoost;
	CCParticleSystem*	m_emitterCollisions;

	bool m_bGamePaused;
	bool m_bRenderBlocks, m_bRenderParticles, m_bTabletMode;
    
	int m_iWormSegmentLength; 
    float m_fWormThickness;
	float m_fUIScaleTablet, m_fUIOpacity;

	//DEBUG
	CCLabelTTF *debugLabel;
    //Some debug settings
    bool m_bFadeOnCollide;

	//for detecting double-tap
	ccTime timeSinceLastTap;

public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  
	virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();
	virtual void update(ccTime dt);
	virtual void registerWithTouchDispatcher();

protected:
	void initSettings();
	void initBlocks();
	void initPlayers();
	void initWormStreaks();
	void initUI();
	void initParticles();
	void resetWorms(ccTime dt);
	void resetGrid();
	void displayPopup(int index, float duration);
	
	void addRandomPowerup();
	
	void onPowerupDie(Powerup* powerup);
	void checkForCollisions(ccTime dt);
	void collide(int player, int obstacle, ccTime dt);

	void initLogicGrid();
	void resetLogicGrid();
	void updateAI(ccTime dt);
	int pollAi(int index, int logicU, int logicV, float bearing, int numLogicBlocksX, int numLogicBlocksY);
	float distanceToWall(float bearing, const CCPoint& pos);

	virtual void endSceneFinalCallback();
	void onNodeFadeoutComplete(CCNode* sender);
	void onResetWormsComplete();

	virtual void powerupPressedCallback(CCObject* pSender);
	// implement the "static node()" method manually
	LAYER_NODE_FUNC(HelloWorld);
	


};

#endif // __HELLOWORLD_SCENE_H__
