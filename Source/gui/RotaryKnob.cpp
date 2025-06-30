/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 8 Jun 2025 12:05:56am
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "RotaryKnob.h"


RotaryKnob::RotaryKnob(juce::AudioProcessorValueTreeState &apvts,
					   juce::RangedAudioParameter const &param)
:	_param(param)
{
	// Create floating value label
	valueLabel = std::make_unique<juce::Label>();
	valueLabel->setSize(10, 10);
	valueLabel->setJustificationType(juce::Justification::centred);
	valueLabel->setColour(juce::Label::textColourId, juce::Colours::white);
	valueLabel->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
	valueLabel->setFont(juce::Font(12.0f));
	valueLabel->setVisible(false);
	valueLabel->setInterceptsMouseClicks(false, false);
	
	setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	setTextBoxStyle(juce::Slider::NoTextBox, false, 15, 15);
	
	setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	setColour(Slider::ColourIds::trackColourId, juce::Colours::darkgrey);
	setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);
	
	addAndMakeVisible(*valueLabel);
}
void RotaryKnob::mouseDown(const juce::MouseEvent& e) {
	juce::Slider::mouseDown(e);
	showValueLabel();
}

void RotaryKnob::mouseDrag(const juce::MouseEvent& e) {
	juce::Slider::mouseDrag(e);
	updateValueLabel();
}

void RotaryKnob::mouseUp(const juce::MouseEvent& e) {
	juce::Slider::mouseUp(e);
	hideValueLabel();
}

void RotaryKnob::resized() {
	juce::Slider::resized();
	positionValueLabel();
}

void RotaryKnob::showValueLabel() {
	updateValueLabel();
	valueLabel->setVisible(true);
}

void RotaryKnob::updateValueLabel() {
	String const labelText = _param.getText(_param.getValue(), 0);
	valueLabel->setText(labelText, juce::dontSendNotification);
}

void RotaryKnob::hideValueLabel() {
	valueLabel->setVisible(false);
}

void RotaryKnob::positionValueLabel() {
	auto bounds = getLocalBounds();
	
	// Position above the knob
	auto labelBounds = bounds.withHeight(20).withY(bounds.getY() + 5 );

	valueLabel->setBounds(labelBounds);
}

juce::String RotaryKnob::getTextFromValue(double value) {
	return juce::String(value * 100.f, 1) + "%";
}
