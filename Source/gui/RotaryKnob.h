/*
  ==============================================================================

    RotaryKnob.h
    Created: 8 Jun 2025 12:05:56am
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "EditableSlider.h"

class RotaryKnob	:	public EditableSlider
{
// any time we want a rotary-style knob, use this instead.
public:
	RotaryKnob(juce::AudioProcessorValueTreeState &apvts,
			   juce::RangedAudioParameter const &param);
	void mouseDown(const juce::MouseEvent& e) override;
	void mouseDrag(const juce::MouseEvent& e) override;
	void mouseUp(const juce::MouseEvent& e) override;
	void resized() override;
private:
	void showValueLabel();
	void updateValueLabel();
	void hideValueLabel();
	void positionValueLabel();
	juce::String getTextFromValue(double value) override;
	
	std::unique_ptr<juce::Label> valueLabel;
	juce::RangedAudioParameter const &_param;
	
};
