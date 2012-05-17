#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "CCScale9Sprite.h"
#include "CCControlExtensions.h"
#include "PowerupScroller.h"

#include "HelloWorldScene.h"
#include "PlayerChoiceScene.h"
#include "GameState.h"

USING_NS_CC;

enum 
{
   	DISPLAY_NONE=-1,	
	DISPLAY_EXIT=0,
	DISPLAY_BACK=1,
	DISPLAY_HOME=2,
	DISPLAY_CANCEL=3,
	DISPLAY_ACCEPT=4,
	DISPLAY_INFO=5
};
typedef int GenericButtonDisplay;

enum {
    STYLE1 =   1,
    STYLE2 =   2,
    STYLE3 =   4,
    STYLE4 =   8,
    STYLE5 =  16,
    STYLE6 =  32,
    STYLE7 =  64,
    STYLE8 = 128
};


enum 
{
   	ANIMATING=1,
	EXITING=2,
	LOADING=4
};
typedef unsigned int MenuState;





class MenuScene: public CCLayer
{
	static const int MENU_INITIAL_OFFSET=-200.;
	static const int HEADING_INITIAL_OFFSET=100.;
	static const int MENU_INITIAL_DELAY_MILLIS=0;
	static const int MENU_DELAY_BETWEEN_ITEMS_MILLIS=200;

protected:
	//keeps track of which buttons to render
	GenericButtonDisplay LEFT_BUTTON;
	GenericButtonDisplay RIGHT_BUTTON;	
	string headingText;
	MenuState menuState;

	//this part should go in the subclass, probably
	CCMenuPassive* mainMenu;
	CCControlButton *quickPlayButton, *customGameButton, *optionsButton;

	CCMenuPassive* customGameMenu;
	CCControlButton *playerColorsButton, *gameplaySettingsButton, *powerupSettingsButton, *startGameButton;

	CCMenuPassive* optionsMenu;
	CCControlButton *gameplayOptionsButton, *soundOptionsButton, *graphicsOptionsButton;
	CCControlCheckBox* tabletModeCheckBox;

	CCMenuPassive* soundOptionsMenu;
	CCSetting *musicSetting, *soundSetting;

	CCMenuPassive* defaultGameplayOptionsMenu;
	CCSetting *speedSetting, *gapLengthSetting, *powerupFrequencySetting,  *turningSpeedSetting;

	CCMenuPassive* graphicsOptionsMenu;
	CCControlCheckBox* blocksCheckBox;
	CCControlCheckBox* particlesCheckBox;
    CCControlCheckBox* fadeOnCollideCheckBox;
	CCSetting *wormSmoothnessSetting;

	CCPoint currentMenuPosition;
	CCPoint offscreenMenuPosition;
	CCPoint headingPosition;
	CCPoint offscreenHeadingPosition;
	float lowerMenuPadding;
	CCPoint lowerMenuPosition;
	CCPoint offscreenLowerMenuPosition;
	float uiScale, spriteScale;

	//Rect for stretching Scale9Sprites
	CCRect scale9ButtonRect;

	//main menu
	CCMenuPassive* pCurrentMenu;
	CCMenuPassive* pPreviousMenu;

	//heading (could be a simple label, a sprite, etc)
	CCNode* pHeading;
	//generic buttons: can be used for: exit, back, accept, cancel, home, info
	CCMenu* pLowerMenu;
	//lower buttons
	CCMenuItem* buttonLowerLeft;
	CCMenuItem* buttonLowerRight;

	virtual float animateMenuIn(CCMenuPassive* menu, bool animateHeading, bool animateLowerMenu, float initialDelay);
	virtual float animateMenuOut(CCMenuPassive* menu, bool animateHeading, bool animateLowerMenu, float initialDelay, SEL_CallFunc selector);
	virtual void switchToMenu(CCMenuPassive* menu, SEL_CallFunc selector);

public:
	MenuScene();
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  
	virtual void keyBackClicked();
	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();
	
	virtual CCControlButton* standardButtonWithTitle(const char* title, SEL_MenuHandler callback);
	virtual CCControlSlider* standardSlider(float min, float max, float value, SEL_MenuHandler callback, float snappingInterval=-1.0f);
	virtual CCNode* labelWithSpacer(const char* labelText, float spacer);

	//this needs to be cleaned up. there should be one callback for all menu items

protected:
	//initialises the heading. By default it takes the heading string and simply creates a label
	virtual void initHeading(CCSize size, const char* heading);
	//initialises the generic lower menu
	virtual void initLowerMenu(CCSize size, int leftButton, int rightButton);
	//init the menu
	virtual void initMenu(CCSize size);
	//callbacks for the generic buttons. these should be overridden
	virtual void buttonLowerLeftCallback(CCObject* pSender);
	virtual void buttonLowerRightCallback(CCObject* pSender);
	
	void finalExitCallback();	
	void finalPlayCallback();
	void switchMenuCompleteCallback();

	//menu callbacks
	void quickPlayButtonCallback(CCObject* sender);
	void customGameButtonCallback(CCObject* sender);
	void optionsButtonCallback(CCObject* sender);
	void playerColorsButtonCallback(CCObject* sender);
	void gameplaySettingsButtonCallback(CCObject* sender);
	void powerupSettingsButtonCallback(CCObject* sender);
	void startGameButtonCallback(CCObject* sender);
	void defaultGameplayOptionsButtonCallback(CCObject* sender);
	void soundOptionsButtonCallback(CCObject* sender);
	void graphicsOptionsButtonCallback(CCObject* sender);


	void musicSliderCallback(CCObject* sender);
	void soundEffectsSliderCallback(CCObject* sender);

	void speedSliderCallback(CCObject* sender);
	void gapLengthSliderCallback(CCObject* sender);
	void powerupFrequencySliderCallback(CCObject* sender);
	void turningSpeedSliderCallback(CCObject* sender);
	
	// implement the "static node()" method manually
	LAYER_NODE_FUNC(MenuScene);
};

#endif