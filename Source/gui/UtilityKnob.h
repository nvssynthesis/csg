/*
  ==============================================================================

    UtilityKnob.h
    Created: 24 May 2025 9:27:04pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct UtilityKnob	:	public juce::Component
{
	using Slider = juce::Slider;
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using String = juce::String;
	
	UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					int numDecimalPlacesToDisplay = 3,
					Slider::SliderStyle sliderStyle = juce::Slider::LinearBarVertical,
				juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::TextBoxBelow);
	void resized() override;
	String getParamName() const { return _paramName; }

	Slider _slider;
	SliderAttachment _attachment;
	
private:
	String _paramName;
	juce::Label _label;
};
