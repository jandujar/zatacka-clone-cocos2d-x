#include "MenuScene.h"

USING_NS_CC;
CCScene* MenuScene::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	MenuScene *layer = MenuScene::node();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

MenuScene::MenuScene()
{
	//call the super
	CCLayer::CCLayer();
	//default settings
	headingText="Zatacka Mobile";
	LEFT_BUTTON=DISPLAY_BACK;
	RIGHT_BUTTON=DISPLAY_INFO;
}


// on "init" you need to initialize your instance
bool MenuScene::init()
{	
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}
	this->setIsKeypadEnabled(true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	
	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();

	//since this is the first screen, cache the textures here. Could have a loading screen, but for now it's simply overkill
	
	GameState::cacheTextures();

	initLowerMenu(size, LEFT_BUTTON, RIGHT_BUTTON);

	initHeading(size, headingText.c_str());
	initMenu(size);

	return true;
}

void MenuScene::initHeading(CCSize size, const char* headingText)
{
	headingPosition= ccp(size.width / 2, size.height) ;
	offscreenHeadingPosition= ccp(size.width / 2, size.height+HEADING_INITIAL_OFFSET*uiScale) ;
	CCScale9Sprite *backgroundHeading = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("Heading.png").c_str(), CCRect(39, 4, 50, 36));
#ifdef WIN32
	CCLabelTTF *titleLabel = CCLabelTTF::labelWithString(headingText, "Berlin Sans FB", 36*uiScale);	
#else
	CCLabelTTF *titleLabel = CCLabelTTF::labelWithString(headingText, "ButtonFont.ttf", 36*uiScale);	
#endif	
	
	CCControlButton *heading = CCControlButton::buttonWithLabelAndBackgroundSprite(titleLabel, backgroundHeading);
	heading->setMargins(40*uiScale, 5*uiScale);
	heading->setColor(ccc3(255, 255, 255));
	heading->setAnchorPoint(ccp(0.5f, 1.0f));
	heading->setPosition(headingPosition);
	this->addChild(heading, 1);
	pHeading=(CCNode*)heading;
	
}

void MenuScene::initMenu(CCSize size)
{
	currentMenuPosition=ccp(10*uiScale, size.height/2);
	CCPoint offscreenMenuPosition=ccp(-size.width*2, size.height/2);
	scale9ButtonRect=CCRect(12, 12, 72, 40);

	//create some buttons for the main menu
	quickPlayButton=standardButtonWithTitle("Quick Play", menu_selector(MenuScene::quickPlayButtonCallback));
	quickPlayButton->setBackgroundSpriteForState(CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonGreenNormalSquare.png").c_str(), scale9ButtonRect), CCControlStateNormal);
	customGameButton=standardButtonWithTitle("Custom Game", menu_selector(MenuScene::customGameButtonCallback));
	optionsButton=standardButtonWithTitle("Options", menu_selector(MenuScene::optionsButtonCallback));
	
	//main menu
	mainMenu = CCMenuPassive::menuWithItems(quickPlayButton, customGameButton, optionsButton, NULL);
	mainMenu->setPosition(currentMenuPosition);
	mainMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);	
	mainMenu->setIsVisible(true);
	

	//create some buttons for the custom game menu
	playerColorsButton=standardButtonWithTitle("Player Colors", menu_selector(MenuScene::playerColorsButtonCallback));
	gameplaySettingsButton=standardButtonWithTitle("Customize Gameplay", menu_selector(MenuScene::gameplaySettingsButtonCallback));
	powerupSettingsButton=standardButtonWithTitle("Customize Powerups", menu_selector(MenuScene::powerupSettingsButtonCallback));
	startGameButton=standardButtonWithTitle("Start Game", menu_selector(MenuScene::startGameButtonCallback));
	startGameButton->setBackgroundSpriteForState(CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonGreenNormalSquare.png").c_str(), scale9ButtonRect), CCControlStateNormal);


	//custom game menu
	customGameMenu = CCMenuPassive::menuWithItems(playerColorsButton, gameplaySettingsButton, powerupSettingsButton, startGameButton, NULL);
	customGameMenu->setPosition(offscreenMenuPosition);
	customGameMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);

	//create some buttons for the options menu
	gameplayOptionsButton=standardButtonWithTitle("Default Gameplay Options", menu_selector(MenuScene::defaultGameplayOptionsButtonCallback));
	soundOptionsButton=standardButtonWithTitle("Sound Options", menu_selector(MenuScene::soundOptionsButtonCallback));
	graphicsOptionsButton=standardButtonWithTitle("Graphics Options", menu_selector(MenuScene::graphicsOptionsButtonCallback));
	
	tabletModeCheckBox=CCControlCheckBox::checkBoxFromFiles(GameState::fileNameFull("checkboxChecked.png").c_str(), GameState::fileNameFull("checkboxUnchecked.png").c_str(), GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect, "Tablet Mode (4 player)", "ButtonFont.ttf", 30*uiScale, "bTabletMode");
	tabletModeCheckBox->setAnchorPoint(ccp(0.0f, 0.5f));
	tabletModeCheckBox->setMargins(24*uiScale, 12*uiScale);

	//options menu
	optionsMenu = CCMenuPassive::menuWithItems(gameplayOptionsButton, soundOptionsButton, graphicsOptionsButton, tabletModeCheckBox, NULL);
	optionsMenu->setPosition(offscreenMenuPosition);
	optionsMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);


	//sound menu
	//align the label nicely with the 
	musicSetting=CCSetting::settingsControl("Music Volume", "fMusicVol", 0, 100, 100);
	soundSetting=CCSetting::settingsControl("Sound Effect Volume", "fSoundVol", 0, 100, 100);
	//musicSlider=standardSlider(0, 100, 75, menu_selector(MenuScene::musicSliderCallback));
	soundOptionsMenu = CCMenuPassive::menuWithItems(musicSetting, soundSetting, NULL);
	soundOptionsMenu->setPosition(offscreenMenuPosition);	
	soundOptionsMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);

	//default gameplay options menu
	std::vector<string> valueNamesSpeed;
	valueNamesSpeed.push_back("Very Slow");
	valueNamesSpeed.push_back("Slow");
	valueNamesSpeed.push_back("Normal");
	valueNamesSpeed.push_back("Fast");
	valueNamesSpeed.push_back("Very Fast");
	valueNamesSpeed.push_back("Extreme");
	speedSetting=CCSetting::settingsControl("Game Speed", "fGameSpeed", valueNamesSpeed, 1, 6, 3);

	std::vector<string> valueNamesGapLength;
	valueNamesGapLength.push_back("None");
	valueNamesGapLength.push_back("Small");
	valueNamesGapLength.push_back("Normal");
	valueNamesGapLength.push_back("Large");
	gapLengthSetting=CCSetting::settingsControl("Gap Length", "fGapLength", valueNamesGapLength, 0, 3, 2);

	std::vector<string> valueNamesPowerupFrequency;
	valueNamesPowerupFrequency.push_back("None");
	valueNamesPowerupFrequency.push_back("Few");
	valueNamesPowerupFrequency.push_back("Normal");
	valueNamesPowerupFrequency.push_back("Many");	
	powerupFrequencySetting=CCSetting::settingsControl("Powerup Frequency", "fPowerupFrequency", valueNamesPowerupFrequency, 0, 3, 2);

	turningSpeedSetting=CCSetting::settingsControl("Turning Speed", "fTurningSpeed", 0.5f, 1.5f, 1.0f);

	defaultGameplayOptionsMenu = CCMenuPassive::menuWithItems(speedSetting, gapLengthSetting, powerupFrequencySetting, turningSpeedSetting, NULL);
	defaultGameplayOptionsMenu->setPosition(offscreenMenuPosition);	
	defaultGameplayOptionsMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);


	blocksCheckBox=CCControlCheckBox::checkBoxFromFiles(GameState::fileNameFull("checkboxChecked.png").c_str(), GameState::fileNameFull("checkboxUnchecked.png").c_str(), GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect, "Enable Blocks", "ButtonFont.ttf", 30*uiScale, "bRenderBlocks");
	blocksCheckBox->setAnchorPoint(ccp(0.0f, 0.5f));
	blocksCheckBox->setMargins(24*uiScale, 12*uiScale);

	particlesCheckBox=CCControlCheckBox::checkBoxFromFiles(GameState::fileNameFull("checkboxChecked.png").c_str(), GameState::fileNameFull("checkboxUnchecked.png").c_str(), GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect, "Enable Particles", "ButtonFont.ttf", 30*uiScale, "bRenderParticles");
	particlesCheckBox->setAnchorPoint(ccp(0.0f, 0.5f));
	particlesCheckBox->setMargins(24*uiScale, 12*uiScale);

    fadeOnCollideCheckBox=CCControlCheckBox::checkBoxFromFiles(GameState::fileNameFull("checkboxChecked.png").c_str(), GameState::fileNameFull("checkboxUnchecked.png").c_str(), GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect, "Debug: Enable Fade", "ButtonFont.ttf", 30*uiScale, "bFadeOnCollide");
	fadeOnCollideCheckBox->setAnchorPoint(ccp(0.0f, 0.5f));
	fadeOnCollideCheckBox->setMargins(24*uiScale, 12*uiScale);    
    
   
    
	std::vector<string> valueNamesWormSmoothness;
	valueNamesWormSmoothness.push_back("Low");
	valueNamesWormSmoothness.push_back("Medium");
	valueNamesWormSmoothness.push_back("High");
	valueNamesWormSmoothness.push_back("Very High");	
	wormSmoothnessSetting=CCSetting::settingsControl("Worm Smoothness", "fWormSmoothness", valueNamesWormSmoothness, 0, 3, 2);


	graphicsOptionsMenu = CCMenuPassive::menuWithItems(blocksCheckBox, particlesCheckBox, wormSmoothnessSetting, fadeOnCollideCheckBox, NULL);
	graphicsOptionsMenu->setPosition(offscreenMenuPosition);	
	graphicsOptionsMenu->alignItemsVerticallyWithPadding(10.0f*uiScale);

	this->addChild(mainMenu, 1);
	this->addChild(customGameMenu, 2);
	this->addChild(optionsMenu, 2);
	this->addChild(soundOptionsMenu, 3);
	this->addChild(defaultGameplayOptionsMenu, 3);
	this->addChild(graphicsOptionsMenu, 3);


	menuState=0;
	pCurrentMenu=NULL;
	//animateMenuIn(optionsMenu, false, false, 0);
	switchToMenu(mainMenu, NULL);
}

void MenuScene::initLowerMenu(CCSize size, int leftButton, int rightButton)
{	
	const char* buttonNames[] = {"CancelButton.png", "BackButton.png", "HomeButton.png", "CancelButton.png", "AcceptButton.png", "InfoButton.png"};
	//TODO: this should be a flag thing. For now it's just hardcoded
	pLowerMenu=0;
	buttonLowerRight=0;
	buttonLowerLeft=0;
	string name=GameState::fileNameFull(buttonNames[leftButton]);
	if (leftButton>=0 && leftButton<=5)
		buttonLowerLeft = CCMenuItemImage::itemFromNormalImage(GameState::fileNameFull(buttonNames[leftButton]).c_str(), GameState::fileNameFull(buttonNames[leftButton]).c_str(), this, menu_selector(MenuScene::buttonLowerLeftCallback) );	
	else 
	{
		buttonLowerLeft=CCMenuItemImage::itemFromNormalImage(GameState::fileNameFull(buttonNames[0]).c_str(), GameState::fileNameFull(buttonNames[0]).c_str(), this, NULL);
		buttonLowerLeft->setIsVisible(false);
	}
	if (rightButton>=0 && rightButton<=5)
		buttonLowerRight = CCMenuItemImage::itemFromNormalImage(GameState::fileNameFull(buttonNames[rightButton]).c_str(), GameState::fileNameFull(buttonNames[rightButton]).c_str(), this, menu_selector(MenuScene::buttonLowerRightCallback) );
	else 
	{
		buttonLowerRight=CCMenuItemImage::itemFromNormalImage(GameState::fileNameFull(buttonNames[0]).c_str(), GameState::fileNameFull(buttonNames[0]).c_str(), this, NULL);
		buttonLowerRight->setIsVisible(false);
	}
	
	buttonLowerRight->setScale(spriteScale);
	buttonLowerLeft->setScale(spriteScale);
	CCPoint offscreenLowerMenuPosition;
	//padding size for buttons
	float padding=20.0f;
	
	CCSize buttonSize=buttonLowerLeft->getContentSize();
	pLowerMenu=CCMenu::menuWithItems(buttonLowerLeft, buttonLowerRight, NULL);
	//if they're both in there, we need to align horizontally and center
	lowerMenuPadding=size.width-(2*buttonSize.width*spriteScale+2*padding);
	pLowerMenu->alignItemsHorizontallyWithPadding(lowerMenuPadding);
	lowerMenuPosition=ccp(size.width / 2, buttonSize.height*spriteScale/2+padding);
	offscreenLowerMenuPosition=ccp(size.width / 2, -buttonSize.height*spriteScale/2-padding);
	pLowerMenu->setPosition(lowerMenuPosition);

		this->addChild(pLowerMenu, 0);
	

}

CCControlSlider* MenuScene::standardSlider(float min, float max, float value, SEL_MenuHandler callback, float snappingInterval)
{
	CCControlSlider* slider=CCControlSlider::sliderFromFiles(GameState::fileNameFull("Slider.png").c_str(), GameState::fileNameFull("SliderProgress.png").c_str(), GameState::fileNameFull("SliderKnob.png").c_str());
	slider->setAnchorPoint(ccp(0.0f, 0.5f));
	slider->setMinimumValue(min);
	slider->setMaximumValue(max);	
	slider->setSnappingInterval(snappingInterval);
	slider->setValue(value);
	slider->addTargetWithActionForControlEvents(this, callback, CCControlEventValueChanged);
	return slider;
}


CCControlButton* MenuScene::standardButtonWithTitle(const char* title, SEL_MenuHandler callback)
{	
	//Creates and return a button with a default background and title color. 
	CCScale9Sprite *backgroundButton = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect);
	CCScale9Sprite *backgroundHighlightedButton = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonSelectedSquare.png").c_str(), scale9ButtonRect);							 		
#ifdef WIN32
	CCLabelTTF *titleButton = CCLabelTTF::labelWithString(title, "Berlin Sans FB", 30*uiScale);
#else
	CCLabelTTF *titleButton = CCLabelTTF::labelWithString(title, "ButtonFont.ttf", 30*uiScale);
#endif		
	CCControlButton *button = CCControlButton::buttonWithLabelAndBackgroundSprite(titleButton, backgroundButton);
	button->setBackgroundSpriteForState(backgroundHighlightedButton, CCControlStateHighlighted);
	button->setTitleColorForState(ccBLACK, CCControlStateNormal);
	button->addTargetWithActionForControlEvents(this, callback, CCControlEventTouchDown);
	button->setAnchorPoint(ccp(0.0f, 0.5f));
	return button;
}

CCNode* MenuScene::labelWithSpacer(const char* labelText, float spacerValue)
{
#ifdef WIN32
	CCLabelTTF *label = CCLabelTTF::labelWithString(labelText, "Berlin Sans FB", 30*uiScale);
#else
	CCLabelTTF *label = CCLabelTTF::labelWithString(labelText, "ButtonFont.ttf", 30*uiScale);
#endif		
	CCSpacer* spacer=CCSpacer::horizontalSpacer(spacerValue);
	CCMenuPassive* layout=CCMenuPassive::menuWithItems(spacer, label, NULL);
	layout->leftAlignItemsHorizontally();
	label->setAnchorPoint(ccp(0.5f, 0.5f));
	return layout;
}

//returns the total duration of the menu animation
float MenuScene::animateMenuIn(CCMenuPassive* menu, bool animateHeading, bool animateLowerMenu, float initialDelay)
{
	reorderChild(menu, 2);
	menu->setIsVisible(false);
	menu->setPosition(currentMenuPosition);
	menu->alignItemsVerticallyWithPadding(10.0f*uiScale);
	
	//animate the individual menu items
	CCArray* children=menu->getChildren();
	CCObject* child;
	int i=0;
	float easeDuration=1.1f;
	float duration=initialDelay;

	CCARRAY_FOREACH(children, child)
	{		
		CCNode* pNode = dynamic_cast<CCNode*>(child);
		if (!pNode)
			continue;

		duration=initialDelay+(MENU_INITIAL_DELAY_MILLIS+i*MENU_DELAY_BETWEEN_ITEMS_MILLIS)/1000.0f;
		CCPoint finalPosition=pNode->getPosition();
		CCAction* easeAction=CCEaseBackInOut::actionWithAction(CCMoveTo::actionWithDuration(easeDuration, finalPosition));
		CCSize tempSize=pNode->getContentSize();
		pNode->setPosition(ccp(-tempSize.width*1.5, finalPosition.y));
		pNode->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(duration),easeAction, NULL));
		i++;
	}
	duration=initialDelay+easeDuration+(MENU_INITIAL_DELAY_MILLIS+i*MENU_DELAY_BETWEEN_ITEMS_MILLIS)/1000.0f;
	//fade menu in as it's arriving
	float fadeMenuDuration=1.0;
	menu->runAction(CCFadeIn::actionWithDuration(fadeMenuDuration));

	//animate the heading
	if (animateHeading)
	{
		duration=max(duration, initialDelay+MENU_INITIAL_DELAY_MILLIS/1000.0f+0.5f);
		CCAction* easeAction=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(0.5f, headingPosition), 2.0f);
		pHeading->setPosition(offscreenHeadingPosition);
		pHeading->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(MENU_INITIAL_DELAY_MILLIS/1000.0f),easeAction, NULL));
	}

	//animate the lower menu
	if (animateLowerMenu && pLowerMenu)
	{
		pLowerMenu->alignItemsHorizontallyWithPadding(lowerMenuPadding);	
		duration=max(duration, initialDelay+MENU_INITIAL_DELAY_MILLIS/1000.0f+0.5f);		
		CCAction* easeActionLowerMenu=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(0.5f, lowerMenuPosition), 1.0f);
		pLowerMenu->setPosition(offscreenLowerMenuPosition);
		pLowerMenu->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(MENU_INITIAL_DELAY_MILLIS/1000.0f),easeActionLowerMenu, NULL));
	}
	menu->setIsVisible(true);
	return duration;
}
float MenuScene::animateMenuOut(CCMenuPassive* menu, bool animateHeading, bool animateLowerMenu, float initialDelay, SEL_CallFunc selector)
{	
	reorderChild(menu, 1);
	menu->setIsVisible(false);
	//animate the individual menu items
	CCArray* children=menu->getChildren();
	CCObject* child;
	int i=0;
	float duration=initialDelay;
	CCARRAY_FOREACH(children, child)
	{		
		CCNode* pNode = dynamic_cast<CCNode*>(child);		
		if (!pNode)
			continue;
		duration=initialDelay+(i*MENU_DELAY_BETWEEN_ITEMS_MILLIS)/1000.0f;
		CCSize tempSize=pNode->getContentSize();
		CCPoint finalPosition=pNode->getPosition();
		finalPosition.x=-tempSize.width*1.5;
		CCAction* easeAction=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(0.5f, finalPosition), 2.0f);
		CCAction* actionSequence;
		
		actionSequence=CCSequence::actions(CCDelayTime::actionWithDuration(duration),easeAction, NULL);
		pNode->runAction(actionSequence);		
	}

	//fade menu out as it's going
	float fadeMenuDuration=1.0;
	menu->runAction(CCFadeOut::actionWithDuration(fadeMenuDuration));

	//animate the heading
	if (animateHeading)
	{
		
		CCAction* easeAction=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(1.0f, offscreenHeadingPosition), 2.0f);	
		pHeading->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(MENU_INITIAL_DELAY_MILLIS/1000.0f),easeAction, NULL));
	}

	if (animateLowerMenu && pLowerMenu)
	{
		pLowerMenu->alignItemsHorizontallyWithPadding(lowerMenuPadding);	
		CCPoint finalPositionLowerMenu=pLowerMenu->getPosition();
		finalPositionLowerMenu.y-=HEADING_INITIAL_OFFSET;
		CCAction* easeActionLowerMenu=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(1.0f, finalPositionLowerMenu), 2.0f);
		pLowerMenu->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(MENU_INITIAL_DELAY_MILLIS/1000.0f),easeActionLowerMenu, NULL));
	}

	//add the callback action to the layer
	if (selector)
		this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(duration+fadeMenuDuration), CCCallFunc::actionWithTarget(this, selector), NULL));
	
	menu->setIsVisible(true);

	return duration+fadeMenuDuration;
}

void MenuScene::switchToMenu(CCMenuPassive* menu, SEL_CallFunc selector)
{	
	//ignore switches to the same menu!
	if(menu==pCurrentMenu)
		return;
	float delay=0;
	//if there's an existing menu, then replace it(and call the selector)
	if (pCurrentMenu)
		delay=animateMenuOut(pCurrentMenu, false, false, 0, selector);
	
		delay=animateMenuIn(menu, false, false, 0);
	pPreviousMenu=pCurrentMenu;
	pCurrentMenu=menu;

	//add the callback action to the layer
	this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delay), CCCallFunc::actionWithTarget(this, callfunc_selector(MenuScene::switchMenuCompleteCallback)), NULL));
	menuState|=ANIMATING;
}

void MenuScene::quickPlayButtonCallback(CCObject* sender)
{
		animateMenuOut(mainMenu, true, true, 0, callfunc_selector(MenuScene::finalPlayCallback));

}

void MenuScene::customGameButtonCallback(CCObject* sender)
{
	switchToMenu(customGameMenu, NULL);
}

void MenuScene::optionsButtonCallback(CCObject* sender)
{
	switchToMenu(optionsMenu, NULL);
}

void MenuScene::playerColorsButtonCallback(CCObject* sender)
{

}

void MenuScene::gameplaySettingsButtonCallback(CCObject* sender)
{

}

void MenuScene::powerupSettingsButtonCallback(CCObject* sender)
{

}

void MenuScene::startGameButtonCallback(CCObject* sender)
{

}

void MenuScene::defaultGameplayOptionsButtonCallback(CCObject* sender)
{
	switchToMenu(defaultGameplayOptionsMenu, NULL);

}

void MenuScene::soundOptionsButtonCallback(CCObject* sender)
{
		switchToMenu(soundOptionsMenu, NULL);
}

void MenuScene::graphicsOptionsButtonCallback(CCObject* sender)
{
		switchToMenu(graphicsOptionsMenu, NULL);

}


void MenuScene::musicSliderCallback(CCObject* sender)
{

}

void MenuScene::soundEffectsSliderCallback(CCObject* sender)
{

}

void MenuScene:: speedSliderCallback(CCObject* sender)
{

}

void MenuScene:: gapLengthSliderCallback(CCObject* sender)
{

}

void MenuScene:: powerupFrequencySliderCallback(CCObject* sender)
{

}

void MenuScene:: turningSpeedSliderCallback(CCObject* sender)
{

}

void MenuScene::keyBackClicked()
{
	buttonLowerLeftCallback(NULL);
}
//callbacks for generic buttons
void MenuScene::buttonLowerLeftCallback(CCObject* pSender)
{
	if (pCurrentMenu==soundOptionsMenu)
	{
		musicSetting->saveValue();
		soundSetting->saveValue();
		switchToMenu(optionsMenu, NULL);
	}
	else if (pCurrentMenu==defaultGameplayOptionsMenu)
	{
		speedSetting->saveValue();
		gapLengthSetting->saveValue();
		powerupFrequencySetting->saveValue();
		turningSpeedSetting->saveValue();
		switchToMenu(optionsMenu, NULL);
	}
	else if (pCurrentMenu==graphicsOptionsMenu)
	{
        blocksCheckBox->saveValue();
        particlesCheckBox->saveValue();
        wormSmoothnessSetting->saveValue();
        fadeOnCollideCheckBox->saveValue();
        
		switchToMenu(optionsMenu, NULL);
	}
	else if (pCurrentMenu==optionsMenu)
	{
        tabletModeCheckBox->saveValue();
		switchToMenu(mainMenu, NULL);
	}
	else if (pCurrentMenu==customGameMenu)
		switchToMenu(mainMenu, NULL);
	else if (pCurrentMenu==mainMenu)
		animateMenuOut(mainMenu, true, true, 0, callfunc_selector(MenuScene::finalExitCallback));
}

void MenuScene::buttonLowerRightCallback(CCObject* pSender)
{	
	CCDirector::sharedDirector()->replaceScene(MenuScene::scene());
}

void MenuScene::switchMenuCompleteCallback()
{
		menuState&=~ANIMATING;
}

void MenuScene::finalPlayCallback()
{

	CCDirector::sharedDirector()->replaceScene(PlayerChoiceScene::scene());		
}

void MenuScene::finalExitCallback()
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}


