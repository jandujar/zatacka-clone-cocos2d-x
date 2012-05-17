#include "PowerupScroller.h"

bool PowerupScroller::init()
{
	if (CCControl::init())
	{
		setIsTouchEnabled(true);
		// Set the default anchor point
		// Set the default anchor point
		setIsRelativeAnchorPoint(true);
		setAnchorPoint(ccp(0.5f, 0.5f));		
		
		CCSize size = CCDirector::sharedDirector()->getWinSize();	
		
		uiScale=GameState::sharedGameState()->getUIScale();
		spriteScale=GameState::sharedGameState()->getSpriteScale();



		flinging=false;
		flingTime=0.0f;

		CCScheduler::sharedScheduler()->scheduleUpdateForTarget(this, 10, false);
		m_backgroundScroller = CCSprite::spriteWithFile(GameState::fileNameFull("powerupScrollerBack.png").c_str());
		m_backgroundScroller->setScale(spriteScale);
		if (!m_backgroundScroller)
			return false;		
		//TODO: dynamic layout. what is the 180 for? for swiping purposes
		setContentSize(CCSizeMake(m_backgroundScroller->getContentSize().width*spriteScale, m_backgroundScroller->getContentSize().height*spriteScale));

		m_backgroundScroller->setAnchorPoint(ccp(0.5f, 0.5f));
		m_backgroundScroller->setPosition(ccp(getContentSize().width/2, spriteScale*m_backgroundScroller->getContentSize().height/2));

		addChild(m_backgroundScroller, 1);

		m_overlay = CCSprite::spriteWithFile(GameState::fileNameFull("powerupScrollerOverlay.png").c_str());
		if (!m_overlay)
			return false;
		m_overlay->setAnchorPoint(ccp(0.5f, 0.5f));
		m_overlay->setPosition(ccp(getContentSize().width/2, spriteScale*m_backgroundScroller->getContentSize().height/2));
		m_overlay->setScale(spriteScale);
		addChild(m_overlay, 3);

		m_overlayPressed = CCSprite::spriteWithFile(GameState::fileNameFull("powerupScrollerOverlayPressed.png").c_str());
		if (!m_overlayPressed)
			return false;
		m_overlayPressed->setAnchorPoint(ccp(0.5f, 0.5f));
		m_overlayPressed->setPosition(ccp(getContentSize().width/2, spriteScale*m_backgroundScroller->getContentSize().height/2));
		addChild(m_overlayPressed, 4);
		m_overlayPressed->setOpacity(0);
		m_overlayPressed->setIsVisible(false);
		m_overlayPressed->setScale(spriteScale);

		//TODO: this should get read from some sort of layout file
		selectedPowerupPosition.x=getContentSize().width/2;
		selectedPowerupPosition.y=spriteScale*m_backgroundScroller->getContentSize().height/2;
		deselectedPowerupPositionLower.x=selectedPowerupPosition.x;
		deselectedPowerupPositionLower.y=selectedPowerupPosition.y-getContentSize().height/3.5f;
		deselectedPowerupPositionUpper.x=selectedPowerupPosition.x;
		deselectedPowerupPositionUpper.y=selectedPowerupPosition.y+getContentSize().height/3.5f;

		return true;
	}
	return false;
}

PowerupScroller* PowerupScroller::powerupScroller()
{
	PowerupScroller *pRet = new PowerupScroller();
	pRet->init();
	pRet->autorelease();
	return pRet;
}

PowerupButton* PowerupScroller::standardPowerupButton(PowerupType powerupType)
{
	PowerupButton* powerupButton = PowerupButton::powerupButton(powerupType);
	if (!powerupButton)
		return NULL;
	powerupButton->setAnchorPoint(ccp(0.5f, 0.5f));
	powerupButton->setPosition(deselectedPowerupPositionLower);
	powerupButton->setIsEnabled(false);
	powerupButton->addTargetWithActionForControlEvents(this, menu_selector(PowerupScroller::buttonPressedCallback), CCControlEventTouchDown);
	return powerupButton;
}

void PowerupScroller::update(ccTime dt)
{
	CCControl::update(dt);
	if (flinging)
		flingTime+=dt;
}

bool PowerupScroller::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!isTouchInside(pTouch) || !getIsEnabled())
    {
		return false;
	}
    
	sendActionsForControlEvents(CCControlEventTouchDown);
	flingOrigin=getTouchLocation(pTouch);
	flingTime=0.0f;
	flinging=true;
    return true;
}

void PowerupScroller::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{	
    if (!m_bEnabled || m_bSelected)
    {
        if (m_bHighlighted)
        {
			setIsHighlighted(false);
        }
        return;
    }
    
	bool isTouchMoveInside = isTouchInside(pTouch);
    if (isTouchMoveInside && !m_bHighlighted)
    {
        m_nState = CCControlStateHighlighted;
		setIsHighlighted(true);
		sendActionsForControlEvents(CCControlEventTouchDragEnter);
    }
	else if (isTouchMoveInside && m_bHighlighted)
    {
		sendActionsForControlEvents(CCControlEventTouchDragInside);

		CCPoint currentDragPoint=getTouchLocation(pTouch);
		float deltaX=currentDragPoint.x-flingOrigin.x;
		float deltaY=currentDragPoint.y-flingOrigin.y;
		if (flinging&& fabs(deltaX)<SWIPE_MAX_OFF_PATH && fabs(deltaY)>SWIPE_MIN_DISTANCE)
		{
			if (flingTime>0.01f)
			{
				float velocityY=deltaY/flingTime;
				if (velocityY>SWIPE_THRESHOLD_VELOCITY)
				{
					//flinging up occured
					flinging=false;
					doFlingUp(deltaY);
				}
				else if (velocityY<-SWIPE_THRESHOLD_VELOCITY)
				{
					//flinging down occured
					flinging=false;
					doFlingDown(deltaY);
				}
			}
		}
    }
	else if (!isTouchMoveInside && m_bHighlighted)
    {
        m_nState = CCControlStateNormal;
			setIsHighlighted(false);
        
		sendActionsForControlEvents(CCControlEventTouchDragExit);        
    }
	else if (!isTouchMoveInside && !m_bHighlighted)
    {
		sendActionsForControlEvents(CCControlEventTouchDragOutside);        
    }
}
void PowerupScroller::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{ 
	if (flinging && powerupButtons.size()>0)
	{
		float fadeDuration=0.4;
		m_overlayPressed->setIsVisible(true);	
		m_overlayPressed->stopAllActions();
		m_overlayPressed->runAction(CCSequence::actions(CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 255), 2), CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 0), 2), NULL));	
	}
	//TODO: this is just stupid
    if (isTouchInside(pTouch) && flinging)
    {
		sendActionsForControlEvents(CCControlEventTouchUpInside);
    }
	
	flinging=false;
	m_nState = CCControlStateNormal;
	setIsHighlighted(false);
	
}

void PowerupScroller::doFlingDown(float deltaY)
{	
	//can't scroll to anything else
	if (powerupButtons.size()<=1)
		return;
	float fadeDuration=0.4;
	m_overlayPressed->stopAllActions();
	m_overlayPressed->setIsVisible(true);	
	m_overlayPressed->runAction(CCSequence::actions(CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 50), 2), CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 0), 2), NULL));
	float easeDuration=0.3f;

	PowerupButton* currentPowerup=powerupButtons[selectedPowerup];
	currentPowerup->setPosition(selectedPowerupPosition);
	currentPowerup->setIsEnabled(false);
	selectedPowerup++;
	selectedPowerup%=powerupButtons.size();
	PowerupButton* nextPowerup=powerupButtons[selectedPowerup];
	currentPowerup->stopAllActions();	
	currentPowerup->runAction(CCEaseBackOut::actionWithAction(CCMoveTo::actionWithDuration(easeDuration, deselectedPowerupPositionLower)));
	currentPowerup->runAction(CCFadeTo::actionWithDuration(easeDuration/2, 0));

	nextPowerup->setPosition(deselectedPowerupPositionUpper);
	nextPowerup->setIsEnabled(true);
	nextPowerup->setIsVisible(true);
	nextPowerup->stopAllActions();	
	nextPowerup->runAction(CCEaseBackOut::actionWithAction(CCMoveTo::actionWithDuration(easeDuration, selectedPowerupPosition)));
	nextPowerup->setOpacity(0);
	nextPowerup->runAction(CCFadeTo::actionWithDuration(easeDuration*2, 50));
}

void PowerupScroller::doFlingUp(float deltaY)
{
	//can't scroll to anything else
	if (powerupButtons.size()<=1)
		return;
	float fadeDuration=0.4;
	m_overlayPressed->stopAllActions();
	m_overlayPressed->setIsVisible(true);	
	m_overlayPressed->runAction(CCSequence::actions(CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 50), 2), CCEaseInOut::actionWithAction(CCFadeTo::actionWithDuration(fadeDuration, 0), 2), NULL));
	float easeDuration=0.3f;
	PowerupButton* currentPowerup=powerupButtons[selectedPowerup];
	currentPowerup->setPosition(selectedPowerupPosition);
	currentPowerup->setIsEnabled(false);
	selectedPowerup--;
	selectedPowerup+=powerupButtons.size();
	selectedPowerup%=powerupButtons.size();
	
	currentPowerup->stopAllActions();	
	currentPowerup->runAction(CCEaseBackOut::actionWithAction(CCMoveTo::actionWithDuration(easeDuration, deselectedPowerupPositionUpper)));
	currentPowerup->runAction(CCFadeTo::actionWithDuration(easeDuration/2, 0));

	PowerupButton* nextPowerup=powerupButtons[selectedPowerup];
	nextPowerup->setIsEnabled(true);
	nextPowerup->setIsVisible(true);
	nextPowerup->setPosition(deselectedPowerupPositionLower);
	nextPowerup->stopAllActions();	
	nextPowerup->runAction(CCEaseBackOut::actionWithAction(CCMoveTo::actionWithDuration(easeDuration, selectedPowerupPosition)));
	nextPowerup->setOpacity(0);
	nextPowerup->runAction(CCFadeTo::actionWithDuration(easeDuration*2, 50));
}

void PowerupScroller::buttonPressedCallback(CCObject* sender)
{

}

PowerupButton* PowerupScroller::getCurrentPowerup()
{
	if (powerupButtons.size()<1)
		return NULL;
	PowerupButton* currentPowerup=powerupButtons[selectedPowerup];
	return currentPowerup;
}

void PowerupScroller::addPowerup(PowerupType powerupType)
{
	PowerupButton* powerupButton=standardPowerupButton(powerupType);
	this->addChild(powerupButton);
	powerupButton->setScale(spriteScale);			
	powerupButton->setOpacity(0);
	powerupButton->setIsVisible(false);		
	
	//if there are no current powerups, add this one as the current one (scale it in nicely)
	if (powerupButtons.size()==0)
	{
		powerupButtons.push_back(powerupButton);
		selectedPowerup=0;	
		powerupButtons[selectedPowerup]->setPosition(selectedPowerupPosition);
		powerupButtons[selectedPowerup]->setIsVisible(true);		
		//initially, no opactiy and no size
		powerupButton->setScale(0.0f);
		powerupButton->setAnchorPoint(ccp(0.5f, 0.5f));
		//fade in and scale up
		powerupButton->runAction(CCEaseBounceOut::actionWithAction(CCScaleTo::actionWithDuration(0.5f, spriteScale, spriteScale)));
		powerupButton->runAction(CCFadeTo::actionWithDuration(0.5f, 50));
	}
	//if there are already powerups, just add it to the pile
	else
	{		
		powerupButtons.push_back(powerupButton);
	}
}

void PowerupScroller::removeCurrentPowerup()
{
	PowerupButton* currentPowerup=powerupButtons[selectedPowerup];
	currentPowerup->runAction(CCScaleTo::actionWithDuration(0.5f, 0, 0));
	currentPowerup->runAction(CCFadeTo::actionWithDuration(0.5f, 0));
	this->runAction(CCSequence::actionOneTwo(CCDelayTime::actionWithDuration(0.5f),CCCallFuncN::actionWithTarget(this, callfuncN_selector(PowerupScroller::removePowerupFinishedCallback))));
	this->setIsEnabled(false);
}

void PowerupScroller::removePowerupFinishedCallback(CCNode* sender)
{
	this->removeChild(sender, true);
	//if there is another option
	if (powerupButtons.size()>1)
	{
		PowerupButton* pressedPowerup=powerupButtons[selectedPowerup];
		doFlingUp(0);
		PowerupButton* futurePowerup=powerupButtons[selectedPowerup];
		
		//this is an inefficient way to do it, but with max 3 or 4 powerups, it really doesn't matter
		vector<PowerupButton*> temp;
		int newIndex=0;
		for (int i=0;i<powerupButtons.size();i++)
		{
			if (powerupButtons[i]!=pressedPowerup)
				temp.push_back(powerupButtons[i]);
			
		}
		powerupButtons.clear();
		powerupButtons=temp;
		for (int i=0;i<powerupButtons.size();i++)
		{
			if (powerupButtons[i]==futurePowerup)
				newIndex=i;
		}
		selectedPowerup=newIndex;
	}
	else
	{
		powerupButtons.clear();
	}
	//enable again
	this->setIsEnabled(true);
}

void PowerupScroller::visit()
{	
	/*
	m_renderTexture->beginWithClear(255, 0, 0, 10);
	for (int i=0;i<powerupButtons.size();i++)
	{
		//manual rendering
		glPushMatrix();
		glLoadIdentity();
		powerupButtons[i]->setIsVisible(true);
		powerupButtons[i]->visit();
		powerupButtons[i]->setIsVisible(false);
		glPopMatrix();
	}
	m_renderTexture->end();
	*/
	CCControl::visit();

}


#ifdef DEBUG_DRAW_SCROLLER
void PowerupScroller::draw()
{
	CCControl::draw();
	    // draw bounding box
    CCSize s = m_tContentSize;
    CCPoint vertices[4] = {
        ccp(0,0), ccp(s.width,0),
        ccp(s.width,s.height), ccp(0,s.height)
    };
    ccDrawPoly(vertices, 4, true);
}
#endif