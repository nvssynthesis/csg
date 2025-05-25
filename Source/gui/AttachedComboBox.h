/*
  ==============================================================================

    AttachedComboBox.h
    Created: 24 May 2025 9:27:24pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct AttachedComboBox	:	public juce::Component
{
	using ComboBox = juce::ComboBox;
	using ComboBoxAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
	using AudioParamChoice = juce::AudioParameterChoice;
	
	AttachedComboBox(juce::AudioProcessorValueTreeState &apvts,
					 juce::RangedAudioParameter const &param);
	
	void resized() override;

	ComboBox _comboBox;
	ComboBoxAttachment _attachment;
};

