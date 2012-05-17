#ifndef __CCSETTING_H__
#define __CCSETTING_H__

#include "cocos2d.h"
#include "CCControlExtensions.h"
#include "GameState.h"
#include <string>
#include <sstream>

USING_NS_CC;

#define LABEL_SLIDER_PADDING 10
#define LABEL_PADDING 10

class CCSetting: public CCControl
{
	CC_SYNTHESIZE_READONLY(string, m_valueName, ValueName);
	CC_SYNTHESIZE_READONLY(string, m_settingName, SettingName);
	CC_SYNTHESIZE_READONLY(string, m_settingKey, SettingKey);
	CC_SYNTHESIZE_READONLY(float, m_value, Value);
	CC_SYNTHESIZE_READONLY(bool, m_isDiscrete, IsDiscrete);
	
protected:
	CCMenuPassive *layoutV, *layoutH;
	CCSpacer* horizontalSpacer;
	CCControlSlider *slider;
	CCLabelTTF* label;
	std::vector<string> valueStrings;
	float uiScale, spriteScale;

public:
	virtual ~CCSetting();
	static CCSetting* settingsControl(string settingName, string settingKey, std::vector<string> values, float minValue, float maxValue, float defaultValue);
	static CCSetting* settingsControl(string settingName, string settingKey, float minValue, float maxValue, float defaultvalue);

	virtual bool init(string settingName, string settingKey, std::vector<string> values, float minValue, float maxValue, float defaultValue);
	virtual bool init(string settingName, string settingKey, float minValue, float maxValue, float defaultValue);

	void needsLayout();
	virtual void sliderChangedCallback(CCObject* sender);
	virtual void saveValue();

public:

};


#endif