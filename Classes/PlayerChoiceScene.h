#ifndef __PLAYER_CHOICE_SCENE_H__
#define __PLAYER_CHOICE_SCENE_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "PowerupScroller.h"
#include "PlayerUI.h"
#include "LayoutSet.h"
#include "MenuScene.h"
#include "GameState.h"
#include "Player.h"
#include "CCRibbon.h"
#include <queue>
USING_NS_CC;

#define DEFAULT_COLOR ccc3(40, 40, 40)




typedef unsigned int MenuState;


class PlayerChoiceScene : public cocos2d::CCLayer
{
	static const int MENU_INITIAL_OFFSET=-200.;
	static const int HEADING_INITIAL_OFFSET=100.;
	static const int MENU_INITIAL_DELAY_MILLIS=0;
	static const int MENU_DELAY_BETWEEN_ITEMS_MILLIS=200;

protected:
	//UI
	map<UIPosition, PlayerUI*> playerUI;
	map<UIPosition, CCScale9Sprite*> playerUIBorder;

	//PlayerUI* playerUISouth, *playerUINorth, *playerUIEast, *playerUIWest;
	//CCScale9Sprite *playerUISouthBorder, *playerUINorthBorder, *playerUIEastBorder, *playerUIWestBorder;

	float spriteScale;
	float uiScale;

	CCRect scale9ButtonRect;

	//Grid
	int numBlocksX, numBlocksY;
	float gridBlockWidth, gridBlockHeight;
	CCSprite** gridBlocksHighlighted;

	CCPoint gridOrigin;
	CCSize gridSize;
	
	int numPlayers, numAI;
	map<UIPosition, int> playerUIMap;
	std::vector<ccColor3B> playerColors;
	
    
	CCMenuPassive* menuHuman, *menuAI;	
	CCControlButton* contineButton, *backButton;
	CCMenuPassive* aiButtonLayout;
	CCControlButton *noAIButton, *oneAIButton, *twoAIButton, *threeAIButton, *backButtonAIMenu;
	
	CCPoint currentMenuPosition;
	CCPoint offscreenMenuPosition;
	CCMenuPassive* pCurrentMenu, *pPreviousMenu;
	MenuState menuState;
	//Some settings
	bool m_bRenderBlocks, m_bRenderParticles;
    bool m_bTabletMode;
	float m_fUIScaleTablet, m_fUIOpacity;

	//DEBUG
	CCLabelTTF *debugLabel;
    

	//for detecting double-tap
	ccTime timeSinceLastTap;

public:
	virtual bool init();  
	
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();
	virtual void registerWithTouchDispatcher();

protected:
	void initSettings();
	void initBlocks();
	void initUI();
	void resetGrid();
	
	virtual float animateMenuIn(CCMenuPassive* menu, float initialDelay);
	virtual float animateMenuOut(CCMenuPassive* menu, float initialDelay, SEL_CallFunc selector);
	virtual void switchToMenu(CCMenuPassive* menu, SEL_CallFunc selector);
	virtual void switchMenuCompleteCallback();

	CCControlButton* standardButtonWithTitle(const char* title, SEL_MenuHandler callback);
	CCLabelTTF* standardLabel(const char* labelText);
	
	virtual void backButtonCallback(CCObject* sender);
	virtual void continueButtonCallback(CCObject* sender);
	virtual void aiNumButtonCallback(CCObject* sender);
	virtual void backButtonAIMenuCallback(CCObject* sender);
	virtual void backFinalCallback();
	virtual void startFinalCallback();

	// implement the "static node()" method manually
	LAYER_NODE_FUNC(PlayerChoiceScene);	
};

#endif // __PlayerChoiceScene_SCENE_H__
