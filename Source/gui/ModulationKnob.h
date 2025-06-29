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
			   juce::RangedAudioParameter const &param);
	
	void mouseDown(const juce::MouseEvent& event) override;
	
	using Polarity_e = nvs::modulation::Polarity_e;
	using ModSource_e = nvs::modulation::Source_e;

	std::function<void(Polarity_e)> onPolarityChanged;
	std::function<void(ModSource_e)> onModSourceChanged;
	
	Polarity_e getPolarity() const { return _polarity; }
	void setPolarity(Polarity_e polarity) { _polarity = polarity; }
	
	ModSource_e getModSource() const { return _modSource; }
	void setModSource(ModSource_e source) { _modSource = source; }
	
private:
	Polarity_e _polarity { Polarity_e::Bipolar };
	ModSource_e _modSource { ModSource_e::LFO };
	
	
	void showPopupMenu();
	void handleMenuResult(int result);
};
