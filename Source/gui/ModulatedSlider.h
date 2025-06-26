/*
  ==============================================================================

    ModulatedSlider.h
    Created: 24 May 2025 9:26:54pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RotaryKnob.h"

namespace nvs::gui
{
struct ModulatedSlider	:	public juce::Component
{
	using Slider = juce::Slider;
	using Knob = RotaryKnob;
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using String = juce::String;
	
	ModulatedSlider(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					String paramGroupName = "",
					Slider::SliderStyle sliderStyle = juce::Slider::LinearBarVertical,
					juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::NoTextBox);
	void resized() override;
	String getParamName() const { return _paramName; }
	String getParamGroupName() const { return _paramGroupName; }
	
	void disableModulation();
	
	Slider _baseSlider;
	SliderAttachment _baseAttachment;
	Knob _modulationSlider;
	SliderAttachment _modulationAttachment;
	
private:
	String _paramName;
	String _paramGroupName;
	juce::Label _label;
};
}
