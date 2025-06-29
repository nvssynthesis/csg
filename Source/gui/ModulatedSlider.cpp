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
								 Slider::SliderStyle sliderStyle)
:
_baseSlider(),
_baseAttachment(apvts, param.getParameterID(), _baseSlider),
_modulationSlider(apvts, *(apvts.getParameter( nvs::param::makeModID(param.getParameterID()))) ),
_modulationAttachment(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider),
_paramName(param.getName(20)),
_paramGroupName(paramGroupName)
{
	setupSlider(apvts, param.getParameterID(), _baseSlider);
	_baseSlider.setSliderStyle(sliderStyle);
	_baseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	
	_baseSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	_baseSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::darkgrey);
	
	setupSlider(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider);
	
	_label.setFont({"Palatino", 13.f, 0});
	_label.setJustificationType(juce::Justification::centred);
	_label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	_label.setText(_paramName, dontSendNotification);
	
	_modulationSlider.setSize(3, 3);
	
	addAndMakeVisible(_baseSlider);
	addAndMakeVisible(_modulationSlider);
	addAndMakeVisible(_label);
}
void ModulatedSlider::disableModulation(){
	_modulationSlider.setValue(0.0);
	_modulationSlider.setEnabled(false);
	_modulationSlider.setTooltip(nvs::param::makeModName( getParamName() ) + ": This feature is not yet implemented.");
}


void ModulatedSlider::resized()
{
	int const pad = 1;
	auto area = getLocalBounds().withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(1.1*pad).withTrimmedRight(1.1*pad).toFloat();
	using namespace juce;

	
	FlexBox flex;
	
	flex.flexDirection  = FlexBox::Direction::column;     		// vertical main axis
	flex.alignItems     = FlexBox::AlignItems::stretch;   		// stretch to full width
	flex.justifyContent = FlexBox::JustifyContent::flexStart; 	// start at top
	
	flex.items.add ( FlexItem (_label)           .withFlex (0.11f, 1.0f, 11.f).withMinWidth(40.f) );
	auto constexpr lrPad = 9.0;
	flex.items.add ( FlexItem (_baseSlider)      .withFlex (0.64f, 1.0f, 64.f).withMinWidth(40.f).withMargin(FlexItem::Margin(0.0, lrPad, 0.0, lrPad)) );
	flex.items.add ( FlexItem (_modulationSlider).withFlex (0.1f,  1.0f, 33.f).withMinWidth(35.f) );
	
	flex.performLayout (area);
	
	float const labelHeight = _label.getHeight();
	float const fontSize = juce::jlimit(8.0f, 18.0f, labelHeight * 0.6f); // 70% of label height
	_label.setFont({"Palatino", fontSize, 0});
}
}
