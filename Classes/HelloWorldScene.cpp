#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
	
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();

	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::node();
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void HelloWorld::registerWithTouchDispatcher()
{
	CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 2, true);
}


// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	//read settings from shared user defaults
	initSettings();

	LayoutSet layout=LayoutSet::LayoutSet3x5(480);

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();

	float aspectRatio=size.width/size.height;

	srand(time(NULL));

	this->setIsKeypadEnabled(true);	
	this->setIsTouchEnabled(true);

	//sprites should be scaled relative to 480px. TODO: add other options: select native res assests, etc


	float uiSpriteScaleY=spriteScale;

	numBlocksX=m_bTabletMode?15:10;
	gridBlockWidth=size.width/numBlocksX;
	numBlocksY=ceil(size.height/gridBlockWidth);
	gridBlockHeight=gridBlockWidth;
	gridSize=size;
	gridOrigin=ccp(0,0);
	//gridOrigin=ccp(0, size.height/2.0-gridSize.height/2.0f);
	initPlayers();
	initUI();

	//TODO: fix this
	safeBlockMin=0;
	safeBlockMax=numBlocksY;
	//safeBlockMin=(int)ceil(playerUISouth->getContentSize().height/gridBlockHeight);
	//safeBlockMax=(int)floor(gridSize.height/gridBlockHeight);
	//safeBlockMax=(int)floor((gridSize.height-(numPlayers>1?playerUINorth->getContentSize().height:0))/gridBlockHeight);

	if (m_bRenderBlocks)
		initBlocks();


	if (m_bRenderParticles)
		initParticles();
	

	powerupSpriteBatch=CCSpriteBatchNode::batchNodeWithFile(GameState::fileNameFull("gridHeart.png").c_str(), 10);

	scheduleUpdate();

	timeSinceLastTap=100;
	m_bGamePaused=false;




	return true;
}

void HelloWorld::initSettings()
{	
	//game settings:
	numPlayers=GameState::sharedGameState()->getNumPlayers();
	numAI=GameState::sharedGameState()->getNumAI();
	
	//Graphics Options

	m_bRenderBlocks=CCUserDefault::sharedUserDefault()->getBoolForKey("bRenderBlocks", true);
	m_bRenderParticles=CCUserDefault::sharedUserDefault()->getBoolForKey("bRenderParticles", true);	
	m_bTabletMode=CCUserDefault::sharedUserDefault()->getBoolForKey("bTabletMode", false);
	
    m_bFadeOnCollide=CCUserDefault::sharedUserDefault()->getBoolForKey("bFadeOnCollide", true);
    m_fUIOpacity=CCUserDefault::sharedUserDefault()->getFloatForKey("fUIOpaticy", 80);
    m_fUIScaleTablet=CCUserDefault::sharedUserDefault()->getFloatForKey("fUIScaleTablet", 0.7f);
    
	//this might seem like a bit of a messy way of doing it, but this will allow updates via the xml settings file (I think)
	int smoothness=(int)(CCUserDefault::sharedUserDefault()->getFloatForKey("fWormSmoothness", 2));

	vector<int> segmentLengths;
	segmentLengths.push_back(CCUserDefault::sharedUserDefault()->getIntegerForKey("iSegmentLengthLow", 15));
	segmentLengths.push_back(CCUserDefault::sharedUserDefault()->getIntegerForKey("iSegmentLengthMed", 10));
	segmentLengths.push_back(CCUserDefault::sharedUserDefault()->getIntegerForKey("iSegmentLengthHigh", 5));
	segmentLengths.push_back(CCUserDefault::sharedUserDefault()->getIntegerForKey("iSegmentLengthVeryHigh", 3));	
	m_iWormSegmentLength=segmentLengths[smoothness];
}

void HelloWorld::initPlayers()
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	map<UIPosition, int>::iterator it;
	int i=0;
	int maxColorIndex=0;
	for (it=GameState::sharedGameState()->playerUIColorMap.begin();it!=GameState::sharedGameState()->playerUIColorMap.end();it++)
	{
		int colorIndex=(*it).second;
		UIPosition position=(*it).first;
		if (colorIndex<0)
			continue;
		maxColorIndex=max(maxColorIndex, colorIndex);
		Player* temp=new Player(colorIndex, gridSize.width, gridSize.height, BORDER, BORDER);
		players.push_back(temp);

		PlayerUI* tempUI=PlayerUI::playerUI(i);
		tempUI->setAnchorPoint(ccp(0.5f, 0.0f));
		tempUI->setColor(ccc3(temp->color.r*0.5f+128, temp->color.g*0.5f+128,temp->color.b*0.5f+128));
		if (m_bTabletMode)
			tempUI->setScale(m_fUIScaleTablet);
		tempUI->setOpacity((GLubyte)(m_fUIOpacity));
		addChild(tempUI, 6);

		switch(position)
		{
		case SOUTH:
			tempUI->setPosition(size.width/2.0f, 0);
			break;
		case NORTH:
			tempUI->setPosition(size.width/2.0f, size.height);
			tempUI->setRotation(180);
			break;
		case EAST:
			tempUI->setPosition(size.width, size.height/2.0f);
			tempUI->setRotation(-90);
			break;
		case WEST:
			tempUI->setPosition(0, size.height/2.0f);
			tempUI->setRotation(90);
			break;
		}
		playerUI.push_back(tempUI);
		
		i++;
	}

	
	//Two AI players
	AIParameterSet* bestSet=new AIParameterSet();
	bestSet->cowardnessDiags=60;
	bestSet->cowardnessFront=97;
	bestSet->cowardnessSides=43;
	bestSet->startingSearchDistance=5;

	for (int i=0;i<numAI;i++)
	{
		//start colors from after the last player
		Player* temp=new Player(maxColorIndex+1+i, gridSize.width, gridSize.height, BORDER, BORDER);
		temp->setIsAI(true);		
		temp->aiParmSet=bestSet;
		players.push_back(temp);		
	}



	initLogicGrid();

	initWormStreaks();
}

void HelloWorld::initWormStreaks()
{
	ccBlendFunc blendFunc = { GL_SRC_ALPHA, GL_ONE }; //additive
	//should be 1 for normal, 2 for retina
	float contentScaleFactor=CCDirector::sharedDirector()->getContentScaleFactor();
    m_fWormThickness=(m_bTabletMode?4:6)*uiScale;
	for (int i=0;i<players.size();i++)
	{

		CCMotionStreak* wormStreak=CCMotionStreak::streakWithFade(1000, m_iWormSegmentLength*uiScale*contentScaleFactor,"streak.png", m_fWormThickness*contentScaleFactor, 12*uiScale*contentScaleFactor, ccc4(players[i]->color.r,players[i]->color.g,players[i]->color.b,255));
		wormStreak->setAnchorPoint(ccp(0.5f, 0.5f));
		//wormStreak->setScale(spriteScale);
		//wormStreak->setBlendFunc(blendFunc);
		wormStreak->setPosition(ccpAdd(gridOrigin, players[i]->getPosition()));
		wormStreak->update(0);
		this->addChild(wormStreak, 3);
		wormStreaks.push_back(wormStreak);

		CCMotionStreak* wormStreakOverlay = CCMotionStreak::streakWithFade(3, m_iWormSegmentLength*uiScale*contentScaleFactor, "streak.png", 1.2f*m_fWormThickness*contentScaleFactor, 12*uiScale*contentScaleFactor, ccc4(players[i]->color.r,players[i]->color.g,players[i]->color.b,220) );		
		wormStreakOverlay->setAnchorPoint(ccp(0.5f, 0.5f));
		//wormStreakOverlay->setScale(spriteScale);
		wormStreakOverlay->setBlendFunc(blendFunc);
		wormStreakOverlay->setPosition(ccpAdd(players[i]->getPosition(), gridOrigin));
		wormStreakOverlay->update(0);
		this->addChild(wormStreakOverlay, 4);
		wormStreakOverlays.push_back(wormStreakOverlay);
	}
}

void HelloWorld::initUI()
{

	float uiSpriteScaleY=spriteScale;
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	CCScale9Sprite *border = CCScale9Sprite::scale9SpriteWithFile(GameState::fileNameFull("border.png").c_str(), CCRect(5, 5, 86, 54));
    border->setIsOutlineOnly(true);
	border->setContentSize(gridSize);
	border->setPosition(size.width/2.0f, size.height/2.0f);	
	border->setAnchorPoint(ccp(0.5f, 0.5f));
	addChild(border, 6);
	border->setOpacity(0);
	border->runAction(CCFadeTo::actionWithDuration(1.0f, 255));
	debugLabel = CCLabelTTF::labelWithString("0", "ButtonFont.ttf", 28*uiScale);
	debugLabel->setColor(ccWHITE);
	debugLabel->setAnchorPoint(ccp(0.5f, 0.0f));
	debugLabel->setPosition(ccp(size.width/2.0f, 0.0f));
	border->addChild(debugLabel, 4);
	debugLabel->setIsVisible(false);

}

void HelloWorld::initParticles()
{
	m_emitterBoost = CCParticleSystemQuad::particleWithFile("LavaFlow.plist");

	//adjust for scaling
	m_emitterBoost->setStartSize(m_emitterBoost->getStartSize()*uiScale);
	m_emitterBoost->setStartSizeVar(m_emitterBoost->getStartSizeVar()*uiScale);
	m_emitterBoost->setEndSize(m_emitterBoost->getEndSize()*uiScale);
	m_emitterBoost->setEndSizeVar(m_emitterBoost->getEndSizeVar()*uiScale);
	m_emitterBoost->setSpeed(m_emitterBoost->getSpeed()*uiScale);
	m_emitterBoost->setSpeedVar(m_emitterBoost->getSpeedVar()*uiScale);
	m_emitterBoost->setPosVar(ccpMult(m_emitterBoost->getPosVar(), uiScale));

	m_emitterBoost->setPosition(ccp(0.0f, 0.0f));
	m_emitterBoost->setSpeed(0);
	m_emitterBoost->stopSystem();
	this->addChild(m_emitterBoost, 3);

	m_emitterCollisions = CCParticleSystemQuad::particleWithFile("Collide.plist");

	//adjust for scaling
	m_emitterCollisions->setStartSize(m_emitterCollisions->getStartSize()*uiScale);
	m_emitterCollisions->setStartSizeVar(m_emitterCollisions->getStartSizeVar()*uiScale);
	m_emitterCollisions->setEndSize(m_emitterCollisions->getEndSize()*uiScale);
	m_emitterCollisions->setEndSizeVar(m_emitterCollisions->getEndSizeVar()*uiScale);
	m_emitterCollisions->setSpeed(m_emitterCollisions->getSpeed()*uiScale);
	m_emitterCollisions->setSpeedVar(m_emitterCollisions->getSpeedVar()*uiScale);
	m_emitterCollisions->setPosVar(ccpMult(m_emitterCollisions->getPosVar(), uiScale));

	m_emitterCollisions->setPosition(ccp(0.0f, 0.0f));
	m_emitterCollisions->setSpeed(0);	
	m_emitterCollisions->stopSystem();	
	this->addChild(m_emitterCollisions, 3);	
}

void HelloWorld::initLogicGrid()
{
	//Logic blocks are now 3x3 pixels
	numLogicBlocksX=gridSize.width;
	numLogicBlocksY=gridSize.height;
	logicGrid=new int[numLogicBlocksX*numLogicBlocksY];
	tempLogicGrid=new int[numLogicBlocksX*numLogicBlocksY];
	resetLogicGrid();
}

void HelloWorld::resetLogicGrid()
{
	for (int i=0;i<numLogicBlocksX;i++)
	{
		for (int j=0;j<numLogicBlocksY;j++)
			logicGrid[i*numLogicBlocksY+j]=-1;
	}
	while (logicQueueX.size())
	{
		logicQueueX.pop();
		logicQueueY.pop();
		logicQueueIndex.pop();
	}
}
void HelloWorld::initBlocks()
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

void HelloWorld::update(ccTime dt)
{
	timeSinceLastTap+=dt;
	if (!m_bGamePaused)
	{

		//for (int z=0;z<10;z++)
		//{
		updateAI(dt);
		while (logicQueueX.size()>players.size()*5)
		{
			int x=logicQueueX.front();
			int y=logicQueueY.front();
			int index=logicQueueIndex.front();
			//logicGrid[x*numLogicBlocksY+y]=index;

			logicQueueX.pop();
			logicQueueY.pop();
			logicQueueIndex.pop();
		}

		//update inputs
		vector<PlayerUI*>::iterator it;
		for (it=playerUI.begin();it!=playerUI.end();it++)
		{
			int index=(*it)->getPlayerIndex();
			players[index]->setInputL((*it)->getInputL());
			players[index]->setInputR((*it)->getInputR());
		}
				
		for (int i=0;i<players.size();i++)
		{
			players[i]->update(dt);

			CCPoint wormPos=players[i]->getPosition();
			//logic
			int logicU=(int)(wormPos.x/(gridSize.width/numLogicBlocksX));
			int logicV=(int)(wormPos.y/(gridSize.height/numLogicBlocksY));	
			//clamp to grid
			logicU=min(max(logicU, 0), numLogicBlocksX-1);
			logicV=min(max(logicV, 0), numLogicBlocksY-1);
			logicGrid[logicU*numLogicBlocksY+logicV]=i;

			logicQueueX.push(logicU);
			logicQueueY.push(logicV);
			logicQueueIndex.push(i);
		}
		checkForCollisions(dt);
		//}


		for (int i=0;i<players.size();i++)
		{
			CCPoint wormPos=players[i]->getPosition();
			int u=(int)(wormPos.x/(gridBlockWidth));
			int v=(int)(wormPos.y/(gridBlockHeight));	
			//clamp to grid
			u=min(max(u, 0), numBlocksX-1);
			v=min(max(v, 0), numBlocksY-1);

			//logic
			int logicU=(int)(wormPos.x/(gridSize.width/numLogicBlocksX));
			int logicV=(int)(wormPos.y/(gridSize.height/numLogicBlocksY));	
			//clamp to grid
			logicU=min(max(logicU, 0), numLogicBlocksX-1);
			logicV=min(max(logicV, 0), numLogicBlocksY-1);

			logicQueueX.push(logicU);
			logicQueueY.push(logicV);
			logicQueueIndex.push(i);
			//logicGrid[logicU*numLogicBlocksY+logicV]=i;



			if (players[i]->getIsAlive())
			{
				CCPoint wormOldPos=players[i]->getOldPosition();
				int oldU=(int)(wormOldPos.x/(gridBlockWidth));
				int oldV=(int)(wormOldPos.y/(gridBlockHeight));
				//clamp to grid
				oldU=min(max(oldU, 0), numBlocksX-1);
				oldV=min(max(oldV, 0), numBlocksY-1);
				if (m_bRenderBlocks)
				{
					if (oldU!=u || oldV!=v)
					{
						//gridBlocksHighlighted[v*numBlocksX+u]->setColor(players[i]->color);
						float colorFraction=0.4f;
						ccColor3B tempColor=ccc3((GLubyte)(DEFAULT_COLOR.r*(1.0-colorFraction)+players[i]->color.r*colorFraction), (GLubyte)(DEFAULT_COLOR.g*(1.0-colorFraction)+players[i]->color.g*colorFraction), (GLubyte)(DEFAULT_COLOR.b*(1.0-colorFraction)+players[i]->color.b*colorFraction));
						gridBlocksHighlighted[u*numBlocksY+v]->runAction(CCTintTo::actionWithDuration(1.0f, tempColor.r, tempColor.g, tempColor.b));
					}
				}
				if (!m_bGamePaused)
				{
					wormStreaks[i]->setPosition(ccpAdd(players[i]->getPosition(), gridOrigin));
					wormStreakOverlays[i]->setPosition(ccpAdd(players[i]->getPosition(), gridOrigin));
				}
				//pick up powerups
				
				int index=u*numBlocksY+v;				
				if (powerupSprites.find(index) != powerupSprites.end())
				{
					CCSprite* temp=powerupSprites[index];
					PowerupType* tempType=(PowerupType*)(temp->getUserData());
					//only some players have a UI attatched!
					if (i<playerUI.size())
					{
						playerUI[i]->addPowerup(*tempType);
					}
					//fade out and scale down
					temp->runAction(CCScaleTo::actionWithDuration(0.5f, 0, 0));
					temp->runAction(CCSequence::actionOneTwo(CCFadeTo::actionWithDuration(0.5f, 0), CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::onNodeFadeoutComplete))));
					powerupSprites.erase(index);
				}

	


				if (players[i]->currentSpeed>DEFAULT_SPEED && m_bRenderParticles)
				{

					m_emitterBoost->setPosition(ccpAdd(players[i]->getPosition(), gridOrigin));
					m_emitterBoost->setAngle(90-players[i]->getAngle());
					m_emitterBoost->setAngleVar(10);
					float probability=0.99f;
					if (CCRANDOM_0_1()<probability)
						m_emitterBoost->addParticle();
				}	
			}
		}
		
		bool powerupCleanupNeeded=false;
		for (int i=0;i<activePowerups.size();i++)
		{
			activePowerups[i]->update(dt);
			if (!activePowerups[i]->getIsActive())
			{
				onPowerupDie(activePowerups[i]);
				powerupCleanupNeeded=true;
			}
		}

		if (powerupCleanupNeeded)
		{
			vector<Powerup*> temp;
			for (int i=0;i<activePowerups.size();i++)
			{			
				if (activePowerups[i]->getIsActive())
				{
					temp.push_back(activePowerups[i]);
				}
				else			
					CC_SAFE_DELETE(activePowerups[i]);
			}
			activePowerups.clear();
			activePowerups=temp;
		}

		
	}
	/*
	//This is interesting, but not THAT interesting
	if (index<=1 || totalDistance>400.0f)
	addingWave=false;

	if (addingWave)
	{
	for (int i=index;i>max(index-10, 1);i--)
	{
	float maxD=5.0f;
	float A=25.0f;
	if (d>maxD)
	{

	CCPoint wormPos=player1->positions[i];
	CCPoint wormPos2=player1->positions[i-1];
	CCPoint perp=ccpNormalize(ccpPerp(ccpSub(wormPos, wormPos2)));			
	m_emitter->setPosition(ccpAdd(wormPos, ccpMult(perp, A*exp(-totalDistance/200.0f)*sin(totalDistance))));
	m_emitter->setAngle(90-player1->getAngle());
	m_emitter->setAngleVar(10);
	m_emitter->addParticle();

	totalDistance+=d;
	d=0.0f;
	}
	d+=ccpDistance(player1->positions[i], player1->positions[i-1]);
	}

	index-=10;
	}*/


}

void HelloWorld::updateAI(ccTime dt)
{
	for (int i=0;i<players.size();i++)
	{
		if (!players[i]->getIsAI())
			continue;
		CCPoint pos=players[i]->getPosition();
		CCPoint dr=ccpSub(pos, players[i]->getOldPosition());
		float bearing=atan2(dr.y, dr.x);

		//logic
		int logicU=(int)(pos.x/(gridSize.width/numLogicBlocksX));
		int logicV=(int)(pos.y/(gridSize.height/numLogicBlocksY));	
		//clamp to grid
		logicU=min(max(logicU, 0), numLogicBlocksX-1);
		logicV=min(max(logicV, 0), numLogicBlocksY-1);
	
		int dir=pollAi(i, logicU, logicV, bearing, numLogicBlocksX, numLogicBlocksY);
		int R=-1, L=1, C=0;
		if (dir==R)
		{
			players[i]->setInputL(0);
			players[i]->setInputR(1);	
		}
		else if (dir==L)
		{
			players[i]->setInputL(1);
			players[i]->setInputR(0);	
		}
		else
		{
			//keep on truckin'
			players[i]->setInputL(0);
			players[i]->setInputR(0);		
		}
	}
}

int HelloWorld::pollAi(int index, int logicU, int logicV, float bearing, int numLogicBlocksX, int numLogicBlocksY)
{
	
	int COWARDNESS_FRONT = players[index]->aiParmSet->cowardnessFront;//*(CCRANDOM_0_1()/2.0f+0.5f);
	int COWARDNESS_SIDES = players[index]->aiParmSet->cowardnessSides;//*(CCRANDOM_0_1()/2.0f+0.5f);
	int COWARDNESS_DIAGS = players[index]->aiParmSet->cowardnessDiags;//*(CCRANDOM_0_1()/2.0f+0.5f);
	int extrapLength=20;
	memcpy(tempLogicGrid, logicGrid, sizeof(int)*numLogicBlocksX*numLogicBlocksY);
	//extrapolate some data 
	for (int i=0;i<players.size();i++)
	{
		if (i==index)
			continue;
		CCPoint wormPos=players[i]->getPosition();
		CCPoint wormDir=ccpNormalize(ccpSub(players[i]->getPosition(), players[i]->getOldPosition()));
		for (int j=0;j<extrapLength*2;j++)
		{
			wormPos=ccpAdd(wormPos, ccpMult(wormDir, 0.5f));
			//logic
			int logicU=(int)(wormPos.x/(gridSize.width/numLogicBlocksX));
			int logicV=(int)(wormPos.y/(gridSize.height/numLogicBlocksY));	
			//clamp to grid
			logicU=min(max(logicU, 0), numLogicBlocksX-1);
			logicV=min(max(logicV, 0), numLogicBlocksY-1);
			tempLogicGrid[logicU*numLogicBlocksY+logicV]=i;
		}
	}

	int R=-1, L=1, C=0;
    int i;
	for (i =players[index]->aiParmSet->startingSearchDistance; i < COWARDNESS_FRONT; ++i) {
        int checkx, checky, result;
        if (i < COWARDNESS_DIAGS) 
		{
            /* left diagonal */
            checkx = (int)(logicU + i * cos(bearing + PI / 4));
            checky = (int)(logicV + i * sin(bearing + PI / 4));
		if (checkx < BORDER || checky < BORDER || checkx >= numLogicBlocksX-BORDER || checky >= numLogicBlocksY-BORDER)
            {
				
				return R;				
			}
            if (tempLogicGrid[checkx*numLogicBlocksY+checky] != -1 || tempLogicGrid[(checkx-1)*numLogicBlocksY+checky] != -1 
				|| tempLogicGrid[(checkx+1)*numLogicBlocksY+checky] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky-1] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky+1] != -1)
                {
				
				return R;				
			}
            /* right diagonal */
            checkx = (int)(logicU + i * cos(bearing - PI / 4));
            checky = (int)(logicV + i * sin(bearing - PI / 4));
		if (checkx < BORDER || checky < BORDER || checkx >= numLogicBlocksX-BORDER || checky >= numLogicBlocksY-BORDER)
                {
				
				return L;				
			}
			//TODO: I think the above should be L...will test later
            if (tempLogicGrid[checkx*numLogicBlocksY+checky] != -1 || tempLogicGrid[(checkx-1)*numLogicBlocksY+checky] != -1 
				|| tempLogicGrid[(checkx+1)*numLogicBlocksY+checky] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky-1] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky+1] != -1)
               {
				
				return L;				
			}
        }
        if (i < COWARDNESS_SIDES) 
		{
            /* right side */
            checkx = (int)(logicU + i * cos(bearing - PI / 2));
            checky = (int)(logicV + i * sin(bearing - PI / 2));
		if (checkx < BORDER || checky < BORDER || checkx >= numLogicBlocksX-BORDER || checky >= numLogicBlocksY-BORDER)
               {
				
				return L;				
			}
			//TODO: I think the above should be L...will test later
           if (tempLogicGrid[checkx*numLogicBlocksY+checky] != -1 || tempLogicGrid[(checkx-1)*numLogicBlocksY+checky] != -1 
				|| tempLogicGrid[(checkx+1)*numLogicBlocksY+checky] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky-1] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky+1] != -1)
                {
				
				return L;
			}
            /* left side */
            checkx = (int)(logicU + i * cos(bearing + PI / 2));
            checky = (int)(logicV + i * sin(bearing + PI / 2));
		if (checkx < BORDER || checky < BORDER || checkx >= numLogicBlocksX-BORDER || checky >= numLogicBlocksY-BORDER)
                {
				
				return R;				
			}
            if (tempLogicGrid[checkx*numLogicBlocksY+checky] != -1 || tempLogicGrid[(checkx-1)*numLogicBlocksY+checky] != -1 
				|| tempLogicGrid[(checkx+1)*numLogicBlocksY+checky] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky-1] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky+1] != -1)
                {
				
				return R;				
			}
        }
        /* front */
		checkx = (int)(logicU + i * cos(bearing));
        checky = (int)(logicV + i * sin(bearing));
		if (checkx < BORDER || checky < BORDER || checkx >= numLogicBlocksX-BORDER || checky >= numLogicBlocksY-BORDER)
			{
				
				return R;				
			}
        result = tempLogicGrid[checkx*numLogicBlocksY+checky];
        if (tempLogicGrid[checkx*numLogicBlocksY+checky] != -1 || tempLogicGrid[(checkx-1)*numLogicBlocksY+checky] != -1 
				|| tempLogicGrid[(checkx+1)*numLogicBlocksY+checky] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky-1] != -1 || tempLogicGrid[checkx*numLogicBlocksY+checky+1] != -1)
		{
			//if (CCRANDOM_0_1()<0.5f)
            if (result == index)
                return L;
            else
                return R;
        }
    }
	
    return C;
}

float HelloWorld::distanceToWall(float bearing, const CCPoint& pos)
{
	float distX=gridSize.width;
	float distY=gridSize.height;

	if (bearing>-PI/2.0f && bearing<PI/2.0f)	
		distX=fabs((gridSize.width-BORDER-pos.x)/cos(bearing));
	else if (bearing<-PI/2.0f || bearing>PI/2.0f)
		distX=fabs((pos.x-BORDER)/cos(bearing));

	if (bearing>0.0f && bearing<PI)	
		distY=fabs((gridSize.height-BORDER-pos.y)/sin(bearing));
	else if (bearing<0.0f || bearing>PI)
		distY=fabs((pos.y-BORDER)/sin(bearing));

	return min(distX, distY);
}


void HelloWorld::checkForCollisions(ccTime dt)
{

	for (int i=0;i<players.size();i++)
	{
		CCPoint pos=players[i]->getPosition();
		if (!players[i]->getIsAlive())
			continue;
		if (pos.x<BORDER || pos.x>=gridSize.width-BORDER || pos.y<BORDER || pos.y>=gridSize.height-BORDER)
		{
			//collide with -1 for the border
			collide(i, -1, dt);
			continue;			
		}
		for (int j=0;j<players.size();j++)
		{	
			//if there is a collision with another player, then player_i will die, so we can break out of this look and move on to the next player
			if (!players[i]->getIsAlive())
				break;
			CCPoint oldPos=players[i]->getOldPosition();

			//this is still using the crummy linear collision
			int numPositions=players[j]->positions.size();
			if (i==j)
			{
				//can't collide with the last few positions of your own worm
				int numSelfPositionsDisregarded=5;
				numPositions=max(0, numPositions-numSelfPositionsDisregarded);
			}
            
            //d_max^2=|pos-oldPos|^2+(w/2)^2
            float threshold2=ccpLengthSQ(ccpSub(pos, oldPos))+(m_fWormThickness/2.0f)*(m_fWormThickness/2.0f);
                        
            float f=m_fWormThickness/2.0f;
            //since there is alpha blending on the worm edges, we need to decrease the width used for logic.
            //Of course, this depends entirely on the texture used for the streak.
            f*=0.8f;
            CCPoint c1=ccpMult(ccpAdd(pos, oldPos), 0.5f);
            CCPoint u1=ccpSub(pos, oldPos);
            float e1=ccpLength(u1)/2.0f;
            u1=ccpNormalize(u1);
            
            CCPoint v1=ccpPerp(u1);
            
			for (int u=1; u<numPositions;u++)
			{
                CCPoint c0=ccpMult(ccpAdd(players[j]->positions[u-1], players[j]->positions[u]), 0.5f);
                CCPoint u0=ccpSub(players[j]->positions[u], players[j]->positions[u-1]);
                float e0=ccpLength(u0)/2.0f;
                u0=ccpNormalize(u0);
                
                CCPoint v0=ccpPerp(u0);
                
                
                CCPoint d=ccpSub(c1, c0);
                
                
                float absAdB00=fabsf(ccpDot(u0, u1));
                float absAdB01=fabsf(ccpDot(u0, v1));
                
                float absAdD=fabsf(ccpDot(u0, d));
                float rSum=e0+e1*absAdB00+f*absAdB01;
                if (absAdD>rSum)
                    continue;
                
                float absAdB10=fabsf(ccpDot(v0, u1));
                float absAdB11=fabsf(ccpDot(v0, v1));
                absAdD=fabsf(ccpDot(v0, d));
                rSum=f+e1*absAdB10+f*absAdB11;
                if (absAdD>rSum)
                    continue;
                
                absAdD=fabsf(ccpDot(u1, d));
                rSum=e1+e0*absAdB00+f*absAdB10;
                if (absAdD>rSum)
                    continue;
                
                absAdD=fabsf(ccpDot(v1, d));
                rSum=f+e0*absAdB01+f*absAdB11;
                if (absAdD>rSum)
                    continue;
                
                
                collide(i, j, dt);
                break;
                
                
                
			}
		}

	}
}

void HelloWorld::collide(int player, int obstacle, ccTime dt)
{
	//this is here for later on, might want to change behaviour when colliding with the wall (for example particle effects)
	if (obstacle==-1)
	{

	}
	else
	{

	}
	
	players[player]->die();
	int numPlayersAlive=0;
	for (int i=0;i<players.size();i++)
	{		
		if (!players[i]->getIsAlive())
			continue;

		numPlayersAlive++;
		players[i]->score++;
		//TODO: put this back
		if (!players[i]->getIsAI())
			displayPopup(i, 0.5f);
	}
	//reached the end of the round
	if (numPlayersAlive<=1)
	{
		resetWorms(dt);
		resetGrid();
	}
	return;
}

void HelloWorld::resetGrid()
{
	if (m_bRenderBlocks)
	{
		for (int i=0;i<numBlocksX;i++)
		{
			for (int j=0;j<numBlocksY;j++)
			{
				gridBlocksHighlighted[j*numBlocksX+i]->stopAllActions();

				gridBlocksHighlighted[j*numBlocksX+i]->runAction(CCTintTo::actionWithDuration(1.0f, DEFAULT_COLOR.r, DEFAULT_COLOR.g, DEFAULT_COLOR.b));

			}
		}		
	}

	//get rid of the powerups scattered around
	map<int, CCSprite*>::iterator pos;
	for (pos = powerupSprites.begin(); pos != powerupSprites.end(); ++pos)
	{
		((*pos).second)->runAction(CCSequence::actionOneTwo(CCFadeTo::actionWithDuration(1.0f, 0),CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::onNodeFadeoutComplete))));
	}
	powerupSprites.clear();	
}

void HelloWorld::resetWorms(ccTime dt)
{
	if (m_bRenderParticles)
	{
		for (int i=0;i<players.size();i++)
		{
			float d=0;
			float totalDistance=0;
			m_emitterCollisions->setAngle(90-players[i]->getAngle());
			ccColor4F col=ccc4FFromccc3B(players[i]->color);
			col.a=0;
			m_emitterCollisions->setStartColor(col);
			col.a=255;
			m_emitterCollisions->setEndColor(col);
			m_emitterCollisions->setAngleVar(360);
			float initalLife=m_emitterCollisions->getLife();
			for (int j=players[i]->positions.size()-1;j>0;j--)
			{
				float maxD=2.0f*uiScale;
				float A=1.0f*uiScale;

				if (d>maxD)
				{
					CCPoint wormPos=players[i]->positions[j];
					CCPoint wormPos2=players[i]->positions[j-1];
					CCPoint perp=ccpNormalize(ccpPerp(ccpSub(wormPos, wormPos2)));			
					CCPoint pos=ccpAdd(wormPos, ccpMult(perp, A*exp(-totalDistance/200.0f)*sin(totalDistance)));

					m_emitterCollisions->setSourcePosition(ccpAdd(gridOrigin, pos));			
					m_emitterCollisions->setLife((3*initalLife-initalLife*totalDistance/players[i]->length)/3.0);
					m_emitterCollisions->addParticle();			

					totalDistance+=d;
					d=0.0f;
				}
				d+=ccpDistance(players[i]->positions[j], players[i]->positions[j-1]);
			}
			m_emitterCollisions->setLife(initalLife);

	
		}
	}

	for (int i=0;i<players.size();i++)
		players[i]->reset(dt);

	for (int i=0;i<activePowerups.size();i++)
	{			
		CC_SAFE_DELETE(activePowerups[i]);
	}
	activePowerups.clear();

	for (int i=0;i<players.size();i++)
	{

		//take a slightly longer fade if we're not rendering particles        
		if (wormStreaks.size())
		{
			if (m_bFadeOnCollide)
				wormStreaks[i]->getRibbon()->fadeOut(m_bRenderParticles?1.0f:2.0f);
			wormStreaks[i]->runAction(CCSequence::actionOneTwo(CCDelayTime::actionWithDuration(m_bRenderParticles?1.0f:2.0f),CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::onNodeFadeoutComplete))));
			if (m_bFadeOnCollide)
		        wormStreakOverlays[i]->getRibbon()->fadeOut(m_bRenderParticles?1.0f:2.0f);
			wormStreakOverlays[i]->runAction(CCSequence::actionOneTwo(CCDelayTime::actionWithDuration(m_bRenderParticles?1.0f:2.0f),CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::onNodeFadeoutComplete))));
		}
	}
	wormStreaks.clear();
	wormStreakOverlays.clear();

	resetLogicGrid();

	m_bGamePaused=true;	
	this->runAction(CCSequence::actionOneTwo(CCDelayTime::actionWithDuration(1.5f),CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::onResetWormsComplete))));



}



void HelloWorld::onNodeFadeoutComplete(CCNode* sender)
{
	this->removeChild(sender, true);


}

void HelloWorld::onResetWormsComplete()
{	

	//AI Stuff

	//stringstream ss;
	//ss<<"AI1 ("<<playerAI->aiParmSet->cowardnessFront<<","<<playerAI->aiParmSet->cowardnessDiags<<","<<playerAI->aiParmSet->cowardnessSides<<","<<playerAI->aiParmSet->startingSearchDistance<<")";
	//ss<<" vs AI2 ("<<playerAI2->aiParmSet->cowardnessFront<<","<<playerAI2->aiParmSet->cowardnessDiags<<","<<playerAI2->aiParmSet->cowardnessSides<<","<<playerAI2->aiParmSet->startingSearchDistance<<")";	
	//debugLabel->setString(ss.str().c_str());
	 
	/*for (int i=0;i<players.size();i++)
	{
		if (players[i]->score>=10)
		{
			//winner is AI1
			if (players[i]==playerAI)
			{
				delete playerAI2->aiParmSet;
				playerAI2->aiParmSet=playerAI->aiParmSet->clone();
				playerAI2->aiParmSet->cowardnessDiags+=(int)(CCRANDOM_MINUS1_1()*10.0f);
				playerAI2->aiParmSet->cowardnessDiags=max(0, playerAI2->aiParmSet->cowardnessDiags);
				playerAI2->aiParmSet->cowardnessFront+=(int)(CCRANDOM_MINUS1_1()*10.0f);				
				playerAI2->aiParmSet->cowardnessFront=max(0, playerAI2->aiParmSet->cowardnessFront);
				playerAI2->aiParmSet->cowardnessSides+=(int)(CCRANDOM_MINUS1_1()*10.0f);
				playerAI2->aiParmSet->cowardnessSides=max(0, playerAI2->aiParmSet->cowardnessSides);				
				//playerAI2->aiParmSet->startingSearchDistance+=(int)(CCRANDOM_MINUS1_1()*2.0f);
				//playerAI2->aiParmSet->startingSearchDistance=max(0, playerAI2->aiParmSet->startingSearchDistance);
			}
			//winner is AI2
			else if (players[i]==playerAI2)
			{
				delete playerAI->aiParmSet;
				playerAI->aiParmSet=playerAI2->aiParmSet->clone();
				playerAI->aiParmSet->cowardnessDiags+=(int)(CCRANDOM_MINUS1_1()*10.0f);
				playerAI->aiParmSet->cowardnessDiags=max(0, playerAI->aiParmSet->cowardnessDiags);
				playerAI->aiParmSet->cowardnessFront+=(int)(CCRANDOM_MINUS1_1()*10.0f);				
				playerAI->aiParmSet->cowardnessFront=max(0, playerAI->aiParmSet->cowardnessFront);
				playerAI->aiParmSet->cowardnessSides+=(int)(CCRANDOM_MINUS1_1()*10.0f);
				playerAI->aiParmSet->cowardnessSides=max(0, playerAI->aiParmSet->cowardnessSides);				
				//playerAI->aiParmSet->startingSearchDistance+=(int)(CCRANDOM_MINUS1_1()*2.0f);
				//playerAI->aiParmSet->startingSearchDistance=max(0, playerAI->aiParmSet->startingSearchDistance);
			}

			for (int i=0;i<players.size();i++)
			{
				players[i]->score=0;
				
			}
			break;
		}
	}*/



	for (int i=0;i<players.size();i++)
		players[i]->setIsAlive(true);
	m_bGamePaused=false;
	initWormStreaks();

	for (int i=0;i<players.size();i++)
	{
		CCPoint wormPos=players[i]->getPosition();
		//logic
		int logicU=(int)(wormPos.x/(gridSize.width/numLogicBlocksX));
		int logicV=(int)(wormPos.y/(gridSize.height/numLogicBlocksY));	
		//clamp to grid
		logicU=min(max(logicU, 0), numLogicBlocksX-1);
		logicV=min(max(logicV, 0), numLogicBlocksY-1);
		
		logicQueueX.push(logicU);
		logicQueueY.push(logicV);
		logicQueueIndex.push(i);
	}

	//fade first blocks in
	if (m_bRenderBlocks)
	{
		for (int i=0;i<players.size();i++)
		{
			CCPoint wormPos=players[i]->getPosition();
			int u=(int)(wormPos.x/(gridBlockWidth));
			int v=(int)(wormPos.y/(gridBlockHeight));
			gridBlocksHighlighted[v*numBlocksX+u]->stopAllActions();

			float colorFraction=0.4f;
			ccColor3B tempColor=ccc3((GLubyte)(DEFAULT_COLOR.r*(1.0-colorFraction)+players[i]->color.r*colorFraction), (GLubyte)(DEFAULT_COLOR.g*(1.0-colorFraction)+players[i]->color.g*colorFraction), (GLubyte)(DEFAULT_COLOR.b*(1.0-colorFraction)+players[i]->color.b*colorFraction));
			gridBlocksHighlighted[u*numBlocksY+v]->runAction(CCTintTo::actionWithDuration(1.0f, tempColor.r, tempColor.g, tempColor.b));
		}
	}

	//TODO: remove this
	//Testing: adds a powerup sprite to the grid on reset
	addRandomPowerup();
}

void HelloWorld::displayPopup(int index, float duration)
{
	CCPoint initialPosition, finalPosition;
	if (index==0)
	{
		initialPosition=ccp(gridSize.width+100.0f*uiScale, gridOrigin.y+50*uiScale);
//		initialPosition=ccpAdd(ccp(0, gridOrigin.y-50*uiScale), powerupScrollerPlayer1->getPosition());
		//finalPosition=powerupScrollerPlayer1->getPosition();
		finalPosition=ccp(gridSize.width-50.0f*uiScale, gridOrigin.y+50*uiScale);
	}
	else if (index==1)
	{
		initialPosition=ccp(-100.0f*uiScale, gridOrigin.y+gridSize.height-50*uiScale);
		//initialPosition=ccpAdd(ccp(0, 50*uiScale), powerupScrollerPlayer2->getPosition());
		//finalPosition=powerupScrollerPlayer2->getPosition();
		finalPosition=ccp(50.0f*uiScale, gridOrigin.y+gridSize.height-50*uiScale);
	}
	else
		return;

	CCSprite* popupSprite=CCSprite::spriteWithFile(GameState::fileNameFull("plusOne.png").c_str());
	//button->addTargetWithActionForControlEvents(this, callback, CCControlEventTouchDown);
	popupSprite->setAnchorPoint(ccp(0.5f, 0.5f));
	
	popupSprite->setPosition(initialPosition);
	float fadeDuration=0.6f;
	popupSprite->setOpacity(0);
	popupSprite->setScale(spriteScale);
	if (index==1)
		popupSprite->setRotation(180);
	
	popupSprite->runAction(CCEaseBackInOut::actionWithAction(CCMoveTo::actionWithDuration(fadeDuration+duration/2.0f, finalPosition)));
	popupSprite->runAction(CCSequence::actions( CCFadeIn::actionWithDuration(fadeDuration), CCDelayTime::actionWithDuration(duration), CCFadeOut::actionWithDuration(fadeDuration), CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::onNodeFadeoutComplete)), NULL));
	//right in front
	this->addChild(popupSprite, 10);

	
}

void HelloWorld::addRandomPowerup()
{	
	int u=(int)(CCRANDOM_0_1()*numBlocksX);
	//just to make sure it's safe
	int v=(int)(CCRANDOM_0_1()*(safeBlockMax-safeBlockMin)+safeBlockMin);
	int index=u*numBlocksY+v;
	//for now, if the random spot that is picked is already taken by a powerup, then just return
	if (powerupSprites.find(index) != powerupSprites.end())
		return;	
	
	CCSprite* powerupSprite=CCSprite::spriteWithFile(GameState::fileNameFull("gridHeart.png").c_str());
	powerupSprite->setPosition(ccpAdd(gridOrigin, ccp(gridBlockWidth*(u+0.5f), gridBlockHeight*(v+0.5f) ) ) );
	powerupSprite->setScale(0);
	powerupSprite->setOpacity(0);
	//fade in and scale up
	powerupSprite->runAction(CCEaseBounceOut::actionWithAction(CCScaleTo::actionWithDuration(0.5f, spriteScale, spriteScale)));
	powerupSprite->runAction(CCFadeTo::actionWithDuration(0.5f, 128));
	
	
	PowerupType* powerupType=new PowerupType;
	(*powerupType)=LICKETY_SPEED;
	powerupSprite->setUserData(powerupType);
	powerupSprites[index]=powerupSprite;
	this->addChild(powerupSprite, 2);

}


void HelloWorld::onPowerupDie(Powerup* powerup)
{

}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

	CCPoint touchLocation=pTouch->locationInView(pTouch->view());
	touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);  // Convert the position to GL space
	touchLocation           = this->getParent()->convertToNodeSpace(touchLocation);

	if (CCRect::CCRectContainsPoint(CCRectMake(gridOrigin.x, gridOrigin.y, gridSize.width, gridSize.height), touchLocation))
	{
		if (timeSinceLastTap>0 && timeSinceLastTap<0.4f)
		{
			keyBackClicked();
		}
		 timeSinceLastTap=0;
	}
	
	return true;

}

void HelloWorld::powerupPressedCallback(CCObject* pSender)
{
	PlayerUI* pUI= dynamic_cast<PlayerUI*>(pSender);		
	if (!pUI)
		return;

	PowerupButton* pressedButton=pUI->getCurrentPowerup();
	if (pressedButton)
	{
		PowerupType powerupType=pressedButton->getPowerupType();
		//TODO: this should actually get the powerup typefrom the button!

		Powerup* p=new Powerup(LICKETY_SPEED, ccp(0.0f, 0.0f));
		p->activate(players, pUI->getPlayerIndex());

		activePowerups.push_back(p);
		pUI->removeCurrentPowerup();

	}
}

void HelloWorld::keyBackClicked()
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
	this->runAction(CCSequence::actions(CCDelayTime::actionWithDuration(fadeMenuDuration), CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::endSceneFinalCallback)), NULL));

}

void HelloWorld::endSceneFinalCallback()
{
	CCDirector::sharedDirector()->replaceScene(MenuScene::scene());
}

