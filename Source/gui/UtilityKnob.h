/*
  ==============================================================================

    UtilityKnob.h
    Created: 24 May 2025 9:27:04pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotaryKnob.h"

namespace nvs::gui
{
struct UtilityKnob	:	public juce::Component
{
	using Knob = RotaryKnob;
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using String = juce::String;
	
	UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
				juce::RangedAudioParameter const &param,
				int numDecimalPlacesToDisplay = 3);
	void resized() override;
	String getParamName() const { return _paramName; }
	
	Knob _knob;
	SliderAttachment _attachment;
	
private:
	String _paramName;
	juce::Label _label;
};
}
