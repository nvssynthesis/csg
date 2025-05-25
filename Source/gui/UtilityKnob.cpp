/*
  ==============================================================================

    UtilityKnob.cpp
    Created: 24 May 2025 9:27:04pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "UtilityKnob.h"
#include "util.h"

UtilityKnob::UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					int numDecimalPlacesToDisplay,
					Slider::SliderStyle sliderStyle,
					juce::Slider::TextEntryBoxPosition entryPos)
:	_slider(),
	_attachment(apvts, param.getParameterID(), _slider),
	_paramName(param.getName(20))
{
	setupSlider(apvts, param.getParameterID(), _slider);
	_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, /* width */ 15, /* height */ 10);
	_slider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::grey);
	_slider.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkgrey);
	_slider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);

	_label.setFont({"Palatino", 10.f, 0});
	_label.attachToComponent(&_slider, false);
	_label.setText(_paramName, dontSendNotification);
	
	addAndMakeVisible(_slider);
	addAndMakeVisible(_label);
}


void UtilityKnob::resized() {
	_slider.setBounds(getLocalBounds());
}

