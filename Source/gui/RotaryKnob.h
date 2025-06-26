/*
  ==============================================================================

    RotaryKnob.h
    Created: 8 Jun 2025 12:05:56am
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class RotaryKnob	:	public juce::Slider
{
// any time we want a rotary-style knob, use this instead.
public:
	RotaryKnob(juce::AudioProcessorValueTreeState &apvts,
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
		
		setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		setTextBoxStyle(juce::Slider::NoTextBox, false, 15, 15);
		
		setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
		setColour(Slider::ColourIds::trackColourId, juce::Colours::darkgrey);
		setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);
		
		addAndMakeVisible(*valueLabel);
	}
	void mouseDown(const juce::MouseEvent& e) override {
		juce::Slider::mouseDown(e);
		showValueLabel();
	}
	
	void mouseDrag(const juce::MouseEvent& e) override {
		juce::Slider::mouseDrag(e);
		updateValueLabel();
	}
	
	void mouseUp(const juce::MouseEvent& e) override {
		juce::Slider::mouseUp(e);
		hideValueLabel();
	}
	
	void resized() override {
		juce::Slider::resized();
		positionValueLabel();
	}
private:
	void showValueLabel()
	{
		updateValueLabel();
		valueLabel->setVisible(true);
	}
	
	void updateValueLabel()
	{
		String const labelText = _param.getText(_param.getValue(), 0);
		valueLabel->setText(labelText, juce::dontSendNotification);
	}
	
	void hideValueLabel()
	{
		valueLabel->setVisible(false);
	}
	
	void positionValueLabel()
	{
		auto bounds = getLocalBounds();
		
		// Position above the knob
		auto labelBounds = bounds.withHeight(20).withY(bounds.getY() + 5 );

		valueLabel->setBounds(labelBounds);
	}
	
	juce::String getTextFromValue(double value) override
	{
		return juce::String(value * 100.f, 1) + "%";
	}
	
	std::unique_ptr<juce::Label> valueLabel;
	juce::RangedAudioParameter const &_param;
	
};
