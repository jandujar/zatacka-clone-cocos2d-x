#include "CCSetting.h"

CCSetting::~CCSetting()
{
	valueStrings.clear();
}


CCSetting* CCSetting::settingsControl(string settingName, string settingKey, std::vector<string> values, float minValue, float maxValue, float defaultValue)
{
	CCSetting *pRet = new CCSetting();
	pRet->init(settingName, settingKey, values, minValue, maxValue, defaultValue);
	pRet->autorelease();
	return pRet;	
}

bool CCSetting::init(string settingName, string settingKey, std::vector<string> values, float minValue, float maxValue, float defaultValue)
{
	if (!CCControl::init())
		return false;
	m_settingName=settingName;
	m_settingKey=settingKey;
	m_isDiscrete=true;
	valueStrings=values;

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	uiScale=GameState::sharedGameState()->getUIScale();
	spriteScale=GameState::sharedGameState()->getSpriteScale();

	CC_ASSERT(values.size()>1);
	float snappingInterval=(maxValue-minValue)/(values.size()-1);

	label = CCLabelTTF::labelWithString(settingName.c_str(), "ButtonFont.ttf", 28*uiScale);	
	label->setAnchorPoint(ccp(0.0f, 0.5f));
	
	slider=CCControlSlider::sliderFromFiles(GameState::fileNameFull("Slider.png").c_str(), GameState::fileNameFull("SliderProgress.png").c_str(), GameState::fileNameFull("SliderKnob.png").c_str());
	slider->setMinimumValue(minValue);
	slider->setMaximumValue(maxValue);	
	slider->setSnappingInterval(snappingInterval);
	slider->setAnchorPoint(ccp(0.0f, 0.5f));
	//TODO: need to read from defaults
	m_value=CCUserDefault::sharedUserDefault()->getFloatForKey(m_settingKey.c_str(), defaultValue);
	slider->setValue(m_value);

	slider->addTargetWithActionForControlEvents(this, menu_selector(CCSetting::sliderChangedCallback), CCControlEventValueChanged);

	this->addChild(slider);
	this->addChild(label);
	needsLayout();
	return true;
}


CCSetting* CCSetting::settingsControl(string settingName, string settingKey, float minValue, float maxValue, float defaultValue)
{
	CCSetting *pRet = new CCSetting();
	pRet->init(settingName, settingKey, minValue, maxValue, defaultValue);
	pRet->autorelease();
	return pRet;
}

bool CCSetting::init(string settingName, string settingKey, float minValue, float maxValue, float defaultValue)
{
	if (!CCControl::init())
		return false;
	m_settingName=settingName;
	m_settingKey=settingKey;
	m_isDiscrete=false;
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	uiScale=size.width/480.0f;
	//using 240px assets
	spriteScale=size.width/240.0f;

	label = CCLabelTTF::labelWithString(settingName.c_str(), "ButtonFont.ttf", 28*uiScale);	
	label->setAnchorPoint(ccp(0.0f, 0.5f));
	
	slider=CCControlSlider::sliderFromFiles(GameState::fileNameFull("Slider.png").c_str(), GameState::fileNameFull("SliderProgress.png").c_str(), GameState::fileNameFull("SliderKnob.png").c_str());
	slider->setMinimumValue(minValue);
	slider->setMaximumValue(maxValue);	
	slider->setAnchorPoint(ccp(0.0f, 0.5f));
	//TODO: need to read from defaults
	m_value=CCUserDefault::sharedUserDefault()->getFloatForKey(m_settingKey.c_str(), defaultValue);
	slider->setValue(m_value);

	slider->addTargetWithActionForControlEvents(this, menu_selector(CCSetting::sliderChangedCallback), CCControlEventValueChanged);
	//layoutV=CCMenuPassive::menuWithItems(slider, NULL);

	this->addChild(slider);
	this->addChild(label);
	needsLayout();
	return true;
}



void CCSetting::needsLayout()
{
	if (m_isDiscrete)
	{
		int index=(int)floor(m_value-slider->getMinimumValue()+0.5);
		std::stringstream ss;
		ss<<m_settingName<<": ";
		ss<<valueStrings[index];
		label->setString(ss.str().c_str());
		//update label
	}
	float height=label->getContentSize().height+LABEL_SLIDER_PADDING*uiScale+slider->getContentSize().height;
	float width=max(label->getContentSize().width+LABEL_PADDING*uiScale, slider->getContentSize().width);

	label->setPosition(ccp(LABEL_PADDING*uiScale, 0));
	slider->setPosition(ccp(0, -label->getContentSize().height-LABEL_SLIDER_PADDING*uiScale));
	setContentSize(CCSizeMake(width, height));
}


void CCSetting::saveValue()
{
	CCUserDefault::sharedUserDefault()->setFloatForKey(m_settingKey.c_str(), m_value); 
}

void CCSetting::sliderChangedCallback(CCObject* sender)
{
	m_value=slider->getValue();
	needsLayout();
}