/*
  ==============================================================================

    ModulatedSlider.cpp
    Created: 24 May 2025 9:26:54pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "ModulatedSlider.h"
#include "../params.h"
#include "util.h"

namespace nvs::gui
{
ModulatedSlider::ModulatedSlider(juce::AudioProcessorValueTreeState &apvts,
								 juce::RangedAudioParameter const &param,
								 String paramGroupName,
								 Slider::SliderStyle sliderStyle,
								 juce::Slider::TextEntryBoxPosition entryPos)
:
_baseSlider(),
_baseAttachment(apvts, param.getParameterID(), _baseSlider),
_modulationSlider(),
_modulationAttachment(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider),
_paramName(param.getName(20)),
_paramGroupName(paramGroupName)
{
	setupSlider(apvts, param.getParameterID(), _baseSlider);
	_baseSlider.setSliderStyle(sliderStyle);
	_baseSlider.setTextBoxStyle(entryPos, true, 50, 25);
	
	_baseSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	_baseSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::darkgrey);
	
	setupSlider(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider);
	_modulationSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	_modulationSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 10, 10);
	
	_modulationSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	_modulationSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);
	
	_label.setFont({"Palatino", 13.f, 0});
	_label.setJustificationType(juce::Justification::centred);
	_label.attachToComponent(&_baseSlider, false);
	_label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	_label.setText(_paramName, dontSendNotification);
	
	addAndMakeVisible(_baseSlider);
	addAndMakeVisible(_modulationSlider);
	addAndMakeVisible(_label);
}

void ModulatedSlider::resized()
{
	int const pad = 3;
	auto area = getLocalBounds().withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(1.1*pad).withTrimmedRight(1.1*pad).toFloat();
	
	float totalH = area.getHeight();
	float labelH  = totalH * 0.11f;
	float baseH   = totalH * 0.64f;
	float modH   = totalH - labelH - baseH;
	
	using namespace juce;
	FlexBox flex;
	
	flex.flexDirection  = FlexBox::Direction::column;     		// vertical main axis
	flex.alignItems     = FlexBox::AlignItems::stretch;   		// stretch to full width
	flex.justifyContent = FlexBox::JustifyContent::flexStart; 	// start at top
	
	flex.items.add ( FlexItem (_label)           .withFlex (0.0f, 0.0f, labelH)  );
	flex.items.add ( FlexItem (_baseSlider)       .withFlex (0.0f, 0.0f, baseH) );
	flex.items.add ( FlexItem (_modulationSlider).withFlex (0.0f, 0.0f, modH)  );
	
	flex.performLayout (area);
}
}
