/*
 * CCControl.m
 *
 * Copyright 2011 Yannick Loriot.
 * http://yannickloriot.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * converted to c++ / cocos2d-x by Angus C
 */

#include "CCControl.h"

CCControl::CCControl()
{

}

bool CCControl::init()
{
	if (CCLayer::init())
	{
	    // Initialise instance variables
		m_nState=CCControlStateNormal;
		m_bEnabled=true;
		m_bSelected=false;
		m_bHighlighted=false;

		// Set the touch dispatcher priority by default to 1
		m_nDefaultTouchPriority = 1;
		this->setDefaultTouchPriority(m_nDefaultTouchPriority);
        // Initialise the tables
		dispatchTable=new CCMutableDictionary<CCControlState,CCMutableArray<CCInvocation*>*>();	
		return true;
    }
	else
		return false;
}

CCControl::~CCControl()
{
	CC_SAFE_RELEASE(dispatchTable);
}

//Menu - Events
void CCControl::registerWithTouchDispatcher()
{
	CCTouchDispatcher::sharedDispatcher()->addTargetedDelegate(this, 0, true);
}

void CCControl::onEnter()
{
	CCLayer::onEnter();
}

void CCControl::onExit()
{
	CCLayer::onExit();
}

void CCControl::sendActionsForControlEvents(CCControlEvent controlEvents)
{
	// For each control events
    for (int i = 0; i < CONTROL_EVENT_TOTAL_NUMBER; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
            // Call invocations
			CCMutableArray<CCInvocation*>* invocationList=CCControl::dispatchListforControlEvent(1<<i);
			
			CCMutableArray<CCInvocation *>::CCMutableArrayIterator it;
			for (it = invocationList->begin(); it != invocationList->end(); it++) 
			{
				CCInvocation *invocation = *it;
				invocation->invoke(this);
			}                    
        }
    }
}
void CCControl::addTargetWithActionForControlEvents(CCObject* target, SEL_MenuHandler action, CCControlEvent controlEvents)
{
    // For each control events
    for (int i = 0; i < CONTROL_EVENT_TOTAL_NUMBER; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
			CCControl::addTargetWithActionForControlEvent(target, action, 1<<i);            
        }
    }
}



/**
 * Adds a target and action for a particular event to an internal dispatch 
 * table.
 * The action message may optionnaly include the sender and the event as 
 * parameters, in that order.
 * When you call this method, target is not retained.
 *
 * @param target The target object�that is, the object to which the action 
 * message is sent. It cannot be nil. The target is not retained.
 * @param action A selector identifying an action message. It cannot be NULL.
 * @param controlEvent A control event for which the action message is sent.
 * See "CCControlEvent" for constants.
 */
void CCControl::addTargetWithActionForControlEvent(CCObject* target, SEL_MenuHandler action, CCControlEvent controlEvent)
{	
    // Create the invocation object
	CCInvocation *invocation=new CCInvocation(target, action, controlEvent);

    // Add the invocation into the dispatch list for the given control event
	CCMutableArray<CCInvocation*> *eventInvocationList = dispatchListforControlEvent(controlEvent);
	eventInvocationList->addObject(invocation);    
}

void CCControl::removeTargetWithActionForControlEvents(CCObject* target, SEL_MenuHandler action, CCControlEvent controlEvents)
{
	 // For each control events
    for (int i = 0; i < CONTROL_EVENT_TOTAL_NUMBER; i++)
    {
        // If the given controlEvents bitmask contains the curent event
        if ((controlEvents & (1 << i)))
        {
			removeTargetWithActionForControlEvent(target, action, 1 << i);
        }
    }
}

/**
 * Removes a target and action for a particular event from an internal dispatch
 * table.
 *
 * @param target The target object�that is, the object to which the action 
 * message is sent. Pass nil to remove all targets paired with action and the
 * specified control events.
 * @param action A selector identifying an action message. Pass NULL to remove
 * all action messages paired with target.
 * @param controlEvent A control event for which the action message is sent.
 * See "CCControlEvent" for constants.
 */
void CCControl::removeTargetWithActionForControlEvent(CCObject* target, SEL_MenuHandler action, CCControlEvent controlEvent)
{
    // Retrieve all invocations for the given control event
	CCMutableArray<CCInvocation*> *eventInvocationList=dispatchListforControlEvent(controlEvent);
    
	//remove all invocations if the target and action are null
	//TODO: should the invocations be deleted, or just removed from the array? Won't that cause issues if you add a single invocation for multiple events?
	bool bDeleteObjects=true;
    if (target && action)
    {
		//remove objects
		eventInvocationList->removeAllObjects(bDeleteObjects);
    } 
	else
    {
			//normally we would use a predicate, but this won't work here. Have to do it manually			
			CCMutableArray<CCInvocation *>::CCMutableArrayIterator it;
			for (it = eventInvocationList->begin(); it != eventInvocationList->end(); it++) 
			{
				CCInvocation *invocation = *it;
				bool shouldBeRemoved=true;
				if (target)				
					shouldBeRemoved=(target==invocation->getTarget());				
				if (action)
					shouldBeRemoved=(shouldBeRemoved && (action==invocation->getAction()));
				// Remove the corresponding invocation object
				if (shouldBeRemoved)
					eventInvocationList->removeObject(invocation, bDeleteObjects);
			}
	}
}


//CRGBA protocol
void CCControl::setColor(const ccColor3B& color)
{
	m_tColor=color;
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setColor(m_tColor);
		}
	}
}

const ccColor3B& CCControl::getColor(void)
{
	return m_tColor;
}


void CCControl::setOpacity(GLubyte opacity)
{
	m_cOpacity = opacity;
    
	CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setOpacity(opacity);
		}
	}

}

GLubyte CCControl::getOpacity()
{
	return m_cOpacity;
}


void CCControl::setIsOpacityModifyRGB(bool opacityModifyRGB)
{
	m_bIsOpacityModifyRGB=opacityModifyRGB;
		CCObject* child;
	CCArray* children=getChildren();
	CCARRAY_FOREACH(children, child)
	{
		CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);		
		if (pNode)
		{
			pNode->setIsOpacityModifyRGB(opacityModifyRGB);
		}
	}
}

bool CCControl::getIsOpacityModifyRGB()
{
	return m_bIsOpacityModifyRGB;
}


CCPoint CCControl::getTouchLocation(CCTouch* touch)
{
	CCPoint touchLocation=touch->locationInView(touch->view());;                      // Get the touch position
	touchLocation           = CCDirector::sharedDirector()->convertToGL(touchLocation);  // Convert the position to GL space
	touchLocation           = this->getParent()->convertToNodeSpace(touchLocation);         // Convert to the node space of this class
    
    return touchLocation;
}

bool CCControl::isTouchInside(CCTouch* touch)
{
	CCPoint touchLocation=getTouchLocation(touch);
	CCRect bBox=boundingBox();
	return CCRect::CCRectContainsPoint(bBox, touchLocation);
}

CCMutableArray<CCInvocation*>* CCControl::dispatchListforControlEvent(CCControlEvent controlEvent)
{
	CCMutableArray<CCInvocation*> *invocationList=dispatchTable->objectForKey(controlEvent);

    // If the invocation list does not exist for the  dispatch table, we create it
    if (invocationList == NULL)
    {
		invocationList=new CCMutableArray<CCInvocation*>();        
		dispatchTable->setObject(invocationList, controlEvent);
    }    
    return invocationList;
}
