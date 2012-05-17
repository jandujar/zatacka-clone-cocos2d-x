#include "PlayerChoiceScene.h"

USING_NS_CC;

CCScene* PlayerChoiceScene::scene()
{
	
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();

	// 'layer' is an autorelease object
	PlayerChoiceScene *layer = PlayerChoiceScene::node();
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void PlayerChoiceScene::registerWithTouchDispatcher()
{
	CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, -1, true);
}


// on "init" you need to initialize your instance
bool PlayerChoiceScene::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	//read settings from shared user defaults
	initSettings();

	//so that we can color the UI's correctly
	playerColors.push_back(ccMEDIUMRED);
	playerColors.push_back(ccVIOLET);
	playerColors.push_back(ccBISQUE);
	playerColors.push_back(ccAQUAMARINE);
	playerColors.push_back(ccLIGHTPINK);
	playerColors.push_back(ccLIGHTGREEN);
	playerColors.push_back(ccMEDIUMBLUE);
	playerColors.push_back(ccWHITE);



	CCSize size = CCDirector::sharedDirector()->getWinSize();

	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();

	this->setIsKeypadEnabled(true);	
	this->setIsTouchEnabled(true);

	float uiSpriteScaleY=spriteScale;

	

	numBlocksX=m_bTabletMode?15:10;
	gridBlockWidth=size.width/numBlocksX;
	numBlocksY=ceil(size.height/gridBlockWidth);
	gridBlockHeight=gridBlockWidth;
	gridSize=size;
	gridOrigin=ccp(0,0);
	initUI();

	if (m_bRenderBlocks)
		initBlocks();

	timeSinceLastTap=100;
	return true;
}

void PlayerChoiceScene::initSettings()
{	
	//game settings:
	numPlayers=0;
	numAI=0;
	
	//Graphics Options

	m_bRenderBlocks=CCUserDefault::sharedUserDefault()->getBoolForKey("bRenderBlocks", true);
	m_bTabletMode=CCUserDefault::sharedUserDefault()->getBoolForKey("bTabletMode", false);
	m_bRenderParticles=CCUserDefault::sharedUserDefault()->getBoolForKey("bRenderParticles", true);
	m_fUIOpacity=CCUserDefault::sharedUserDefault()->getFloatForKey("fUIOpaticy", 80);
    m_fUIScaleTablet=CCUserDefault::sharedUserDefault()->getFloatForKey("fUIScaleTablet", 0.7f);
}

void PlayerChoiceScene::initUI()
{

	

	CCSize size = CCDirector::sharedDirector()->getWinSize();
	scale9ButtonRect=CCRect(12, 12, 72, 40);

	playerUIMap[NORTH]=-1;
	playerUIMap[SOUTH]=-1;
	if (m_bTabletMode)
	{
		playerUIMap[EAST]=-1;
		playerUIMap[WEST]=-1;
	}

	//SOUTH	
	playerUI[SOUTH]=PlayerUI::playerUI(0);
	playerUI[SOUTH]->setPosition(size.width/2.0f, 0);
		
	//NORTH
	playerUI[NORTH]=PlayerUI::playerUI(1);
	playerUI[NORTH]->setPosition(size.width/2.0f, size.height);
	playerUI[NORTH]->setRotation(180);

	if (m_bTabletMode)
	{
		//EAST
		playerUI[EAST]=PlayerUI::playerUI(1);
		playerUI[EAST]->setPosition(size.width, size.height/2);
		playerUI[EAST]->setRotation(-90);
	
		//WEST
		playerUI[WEST]=PlayerUI::playerUI(1);
		playerUI[WEST]->setPosition(0, size.height/2);
		playerUI[WEST]->setRotation(90);
	}

	map<UIPosition, PlayerUI*>::iterator itUI;
	for (itUI=playerUI.begin();itUI!=playerUI.end();itUI++)
	{
		
		//Borders
		UIPosition position=(*itUI).first;
		PlayerUI* temp=(*itUI).second;

		if (m_bTabletMode)
			temp->setScale(m_fUIScaleTablet);
		temp->setAnchorPoint(ccp(0.5f, 0.0f));	
		temp->setOpacity((GLubyte)(m_fUIOpacity));
		temp->setIsVisible(false);
		addChild(temp, 3);

		playerUIBorder[position]= CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("border.png").c_str(), CCRect(10, 10, 81, 49));
		playerUIBorder[position]->setIsOutlineOnly(true);
		playerUIBorder[position]->setContentSize(temp->getContentSize());
		playerUIBorder[position]->setScale(temp->getScale());
		playerUIBorder[position]->setPosition(temp->getPosition());	
		playerUIBorder[position]->setAnchorPoint(temp->getAnchorPoint());
		playerUIBorder[position]->setRotation(temp->getRotation());
		addChild(playerUIBorder[position], 4);
	}
	
	
	//menus
	currentMenuPosition=ccp(size.width/2, size.height/2);
	offscreenMenuPosition=ccp(-size.width/2, size.height/2);

	stringstream ss;
	ss<<"Select the control position of each human \nplayer by touching the appropriate rectangle.";
	ss<<"\n\n You can select up to "<<(m_bTabletMode?4:2)<<" human players.";
	string labelString=ss.str();
	CCLabelTTF* labelHumanMenu=standardLabel(labelString.c_str());
	//human Menus
	contineButton=standardButtonWithTitle("Continue", menu_selector(PlayerChoiceScene::continueButtonCallback));
	contineButton->setIsVisible(false);
	contineButton->setIsEnabled(false);
	backButton=standardButtonWithTitle("Back", menu_selector(PlayerChoiceScene::backButtonCallback));

	menuHuman = CCMenuPassive::menuWithItems(labelHumanMenu, contineButton, backButton, NULL);
	menuHuman->setPosition(offscreenMenuPosition);
	menuHuman->alignItemsVerticallyWithPadding(10.0f*uiScale);	
	menuHuman->setIsVisible(true);
	menuHuman->setOpacity(0);
	addChild(menuHuman, 5);

	stringstream ss2;
	ss2<<"Select the number of AI opponents.";	
	labelString=ss2.str();
	CCLabelTTF* labelNumAIPlayers=standardLabel(labelString.c_str());

	noAIButton=standardButtonWithTitle("None", menu_selector(PlayerChoiceScene::aiNumButtonCallback));	
	oneAIButton=standardButtonWithTitle("One", menu_selector(PlayerChoiceScene::aiNumButtonCallback));
	twoAIButton=standardButtonWithTitle("Two", menu_selector(PlayerChoiceScene::aiNumButtonCallback));
	threeAIButton=standardButtonWithTitle("Three", menu_selector(PlayerChoiceScene::aiNumButtonCallback));
	aiButtonLayout=CCMenuPassive::menuWithItems(noAIButton, oneAIButton, twoAIButton, threeAIButton, NULL);
	aiButtonLayout->alignItemsHorizontallyWithPadding(0.0f*uiScale);
	
	backButtonAIMenu=standardButtonWithTitle("Back to Player Selection", menu_selector(PlayerChoiceScene::backButtonAIMenuCallback));
	menuAI = CCMenuPassive::menuWithItems(labelNumAIPlayers, aiButtonLayout, backButtonAIMenu, NULL);
	menuAI->setPosition(offscreenMenuPosition);
	menuAI->alignItemsVerticallyWithPadding(10.0f*uiScale);	
	menuAI->setOpacity(0);
	addChild(menuAI, 6);


	menuState=0;
	pCurrentMenu=NULL;
	//animateMenuIn(optionsMenu, false, false, 0);
	switchToMenu(menuHuman, NULL);

	debugLabel = CCLabelTTF::labelWithString("0", "ButtonFont.ttf", 28*uiScale);
	debugLabel->setColor(ccWHITE);
	debugLabel->setAnchorPoint(ccp(0.5f, 0.0f));
	debugLabel->setPosition(ccp(size.width/2.0f, 0.0f));
	debugLabel->setIsVisible(false);

}

void PlayerChoiceScene::initBlocks()
{	
	CCSpriteBatchNode* gridBlockBatch=CCSpriteBatchNode::batchNodeWithFile(GameState::fileNameFull(m_bTabletMode?"gridBlockSmall.png":"gridBlock.png").c_str(), numBlocksX*numBlocksY);
	ccBlendFunc noBlend={GL_ONE, GL_ZERO};
	gridBlocksHighlighted=new CCSprite*[numBlocksX*numBlocksY];
	for (int i=0;i<numBlocksX;i++)
	{
		for (int j=0;j<numBlocksY;j++)
		{			
			CCSprite* gridBlockHighlighted=CCSprite::spriteWithTexture(gridBlockBatch->getTexture());
			gridBlockHighlighted->setScale(spriteScale);
			
			gridBlockHighlighted->setAnchorPoint(ccp(0.5f, 0.5f));
			gridBlockHighlighted->setPosition(ccpAdd(gridOrigin, ccp(gridBlockWidth*(i+0.5f), gridBlockHeight*(j+0.5f) ) ) );
			gridBlockHighlighted->setColor(DEFAULT_COLOR);

			gridBlockHighlighted->setBlendFunc(noBlend) ;
			gridBlocksHighlighted[i*numBlocksY+j]=gridBlockHighlighted;
			gridBlockBatch->addChild(gridBlockHighlighted);
		}
	}
	this->addChild(gridBlockBatch, 1);
}

void PlayerChoiceScene::backButtonCallback(CCObject* sender)
{
	float fadeMenuDuration=1.0;	
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		if (((CCNode*)child)->getIsVisible())
			((CCNode*)child)->runAction(CCFadeTo::actionWithDuration(fadeMenuDuration, 0));	
	}
	//fade menu out as it's going
	this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(fadeMenuDuration), CCCallFunc::actionWithTarget(this, callfunc_selector(PlayerChoiceScene::backFinalCallback)), NULL));
}

void PlayerChoiceScene::backFinalCallback()
{
	CCDirector::sharedDirector()->replaceScene(MenuScene::scene());
}

//this could go in a function, but really, there's no point
void PlayerChoiceScene::continueButtonCallback(CCObject* sender)
{
	//can't select zero AI if there's only one human. Can only select up to 2 if 4 player (6 total players)
	if (numPlayers==1)
	{
		noAIButton->setScale(0);
		threeAIButton->setScale(1);
	}
	else if (numPlayers==4)
	{
		noAIButton->setScale(1);
		threeAIButton->setScale(0);
	}
	else
	{
		noAIButton->setScale(1);
		threeAIButton->setScale(1);
	}

	aiButtonLayout->alignItemsHorizontallyWithPadding(0.0f*uiScale);
	switchToMenu(menuAI, NULL);	
}

void PlayerChoiceScene::startFinalCallback()
{
	GameState::sharedGameState()->playerUIColorMap=playerUIMap;
	CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

void PlayerChoiceScene::aiNumButtonCallback(CCObject* sender)
{	
	if (sender==noAIButton)
		numAI=0;
	else if (sender==oneAIButton)
		numAI=1;
	else if (sender==twoAIButton)
		numAI=2;
	else if  (sender==threeAIButton)
		numAI=3;
	else
		return;

	GameState::sharedGameState()->setNumAI(numAI);
	GameState::sharedGameState()->setNumPlayers(numPlayers);


	float fadeMenuDuration=1.0;	
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{		
		if (((CCNode*)child)->getIsVisible())
			((CCNode*)child)->runAction(CCFadeTo::actionWithDuration(fadeMenuDuration, 0));	
	}
	//fade menu out as it's going
	this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(fadeMenuDuration), CCCallFunc::actionWithTarget(this, callfunc_selector(PlayerChoiceScene::startFinalCallback)), NULL));

	//stop the relevant UI's from fading
	map<UIPosition, PlayerUI*>::iterator itUI;
	for (itUI=playerUI.begin();itUI!=playerUI.end();itUI++)
	{
		(*itUI).second->stopAllActions();
	}
}
void PlayerChoiceScene::backButtonAIMenuCallback(CCObject* sender)
{
	switchToMenu(menuHuman, NULL);
}


bool PlayerChoiceScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

	CCPoint touchLocation=pTouch->locationInView(pTouch->view());
	touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);  // Convert the position to GL space
	touchLocation           = this->getParent()->convertToNodeSpace(touchLocation);
	
	//only need to overide this if the human menu is active
	if (pCurrentMenu!=menuHuman)
		return false;

	bool uiTouched=false;
	map<UIPosition, CCScale9Sprite*>::iterator itBorders;
	for (itBorders=playerUIBorder.begin();itBorders!=playerUIBorder.end();itBorders++)
	{
		
		if (CCRect::CCRectContainsPoint((*itBorders).second->boundingBox(), touchLocation))
		{
			bool alreadySelected=playerUI[(*itBorders).first]->getIsVisible();
			playerUI[(*itBorders).first]->setIsVisible(!alreadySelected);
			if (!alreadySelected)
			{
				int lowestAvailablePlayerIndex=0;
				map<UIPosition, int>::iterator it;
				for (it=playerUIMap.begin();it!=playerUIMap.end();)
				{
					int index=(*it).second;
					if (index>=0 && index==lowestAvailablePlayerIndex)
					{
						//start from the begining again;
						lowestAvailablePlayerIndex++;
						it=playerUIMap.begin();
						continue;
					}
					it++;
				}
				playerUIMap[(*itBorders).first]=lowestAvailablePlayerIndex;
				//mixture between player color and white
				playerUI[(*itBorders).first]->setColor(ccc3(playerColors[lowestAvailablePlayerIndex].r*0.5f+128, playerColors[lowestAvailablePlayerIndex].g*0.5f+128,playerColors[lowestAvailablePlayerIndex].b*0.5f+128));
				numPlayers++;
			}
			else
			{
				numPlayers--;
				playerUIMap[(*itBorders).first]=-1;
			}
			uiTouched=true;
			break;
		}
	}

	if (!uiTouched)
		return false;
	
	contineButton->setIsVisible(numPlayers);
	contineButton->setIsEnabled(numPlayers);

	return true;
}

CCControlButton* PlayerChoiceScene::standardButtonWithTitle(const char* title, SEL_MenuHandler callback)
{	
	//Creates and return a button with a default background and title color. 
	CCScale9Sprite *backgroundButton = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonNormalSquare.png").c_str(), scale9ButtonRect);
	CCScale9Sprite *backgroundHighlightedButton = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("ButtonSelectedSquare.png").c_str(), scale9ButtonRect);							 		
#ifdef WIN32
	CCLabelTTF *titleButton = CCLabelTTF::labelWithString(title, "Berlin Sans FB", (m_bTabletMode?24:30)*uiScale);
#else
	CCLabelTTF *titleButton = CCLabelTTF::labelWithString(title, "ButtonFont.ttf", (m_bTabletMode?24:30)*uiScale);
#endif		
	CCControlButton *button = CCControlButton::buttonWithLabelAndBackgroundSprite(titleButton, backgroundButton);
	button->setMargins(12*uiScale, 12*uiScale);
	button->setBackgroundSpriteForState(backgroundHighlightedButton, CCControlStateHighlighted);
	button->setTitleColorForState(ccBLACK, CCControlStateNormal);
	button->addTargetWithActionForControlEvents(this, callback, CCControlEventTouchDown);
	button->setAnchorPoint(ccp(0.5f, 0.5f));
	return button;
}

CCLabelTTF* PlayerChoiceScene::standardLabel(const char* labelText)
{
#ifdef WIN32
	CCLabelTTF *label = CCLabelTTF::labelWithString(labelText, "Berlin Sans FB", (m_bTabletMode?16:24)*uiScale);
#else
	CCLabelTTF *label = CCLabelTTF::labelWithString(labelText, "ButtonFont.ttf", (m_bTabletMode?16:24)*uiScale);
#endif			
	label->setAnchorPoint(ccp(0.5f, 0.5f));
	return label;
}

//returns the total duration of the menu animation
float PlayerChoiceScene::animateMenuIn(CCMenuPassive* menu, float initialDelay)
{
	reorderChild(menu, 6);
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
		pNode->setPosition(ccp(-tempSize.width*1.5-menu->getPosition().x, finalPosition.y));
		pNode->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(duration),easeAction, NULL));
		i++;
	}
	duration=initialDelay+easeDuration+(MENU_INITIAL_DELAY_MILLIS+i*MENU_DELAY_BETWEEN_ITEMS_MILLIS)/1000.0f;
	//fade menu in as it's arriving
	float fadeMenuDuration=1.0;
	menu->runAction(CCFadeIn::actionWithDuration(fadeMenuDuration));

	menu->setIsVisible(true);
	return duration;
}
float PlayerChoiceScene::animateMenuOut(CCMenuPassive* menu, float initialDelay, SEL_CallFunc selector)
{	
	reorderChild(menu, 5);
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
		finalPosition.x=-tempSize.width*1.5-menu->getPosition().x;
		CCAction* easeAction=CCEaseIn::actionWithAction(CCMoveTo::actionWithDuration(0.5f, finalPosition), 2.0f);
		CCAction* actionSequence;
		
		actionSequence=CCSequence::actions(CCDelayTime::actionWithDuration(duration),easeAction, NULL);
		pNode->runAction(actionSequence);		
	}

	//fade menu out as it's going
	float fadeMenuDuration=1.0;
	menu->runAction(CCFadeOut::actionWithDuration(fadeMenuDuration));
	
	//add the callback action to the layer
	if (selector)
		this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(duration+fadeMenuDuration), CCCallFunc::actionWithTarget(this, selector), NULL));
	
	menu->setIsVisible(true);

	return duration+fadeMenuDuration;
}

void PlayerChoiceScene::switchToMenu(CCMenuPassive* menu, SEL_CallFunc selector)
{	
	//ignore switches to the same menu!
	if(menu==pCurrentMenu)
		return;
	float delay=0;
	//if there's an existing menu, then replace it(and call the selector)
	if (pCurrentMenu)
		delay=animateMenuOut(pCurrentMenu, 0, selector);
	
		delay=animateMenuIn(menu, 0);
	pPreviousMenu=pCurrentMenu;
	pCurrentMenu=menu;

	//add the callback action to the layer
	this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(delay), CCCallFunc::actionWithTarget(this, callfunc_selector(PlayerChoiceScene::switchMenuCompleteCallback)), NULL));
	menuState|=ANIMATING;
}

void PlayerChoiceScene::switchMenuCompleteCallback()
{
		menuState&=~ANIMATING;
}