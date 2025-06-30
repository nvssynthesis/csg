/*
  ==============================================================================

    ModulationKnob.h
    Created: 29 Jun 2025 1:32:52pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "RotaryKnob.h"
#include "../Modulation.h"

class ModulationKnob	:	public RotaryKnob
{
public:
	ModulationKnob(juce::AudioProcessorValueTreeState &apvts,
			   juce::String const &parameterID);
	
	void mouseDown(const juce::MouseEvent& event) override;
	
	using Polarity_e = nvs::modulation::Polarity_e;
	using ModSource_e = nvs::modulation::Source_e;
	using ChoiceParam = juce::AudioParameterChoice;

private:
	juce::AudioProcessorValueTreeState& _apvts;  // Keep reference to APVTS
	juce::String _polarity_param_id;             // Store the parameter IDs
	juce::String _mod_source_param_id;
	
	ChoiceParam const &_polarity_param;
	ChoiceParam const &_mod_source_param;

	
	void showPopupMenu();
	void handleMenuResult(int result);
};
