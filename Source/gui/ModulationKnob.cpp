/*
  ==============================================================================

    ModulationKnob.cpp
    Created: 29 Jun 2025 1:32:52pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "ModulationKnob.h"

ModulationKnob::ModulationKnob(juce::AudioProcessorValueTreeState &apvts,
		   juce::RangedAudioParameter const &param)
:	RotaryKnob(apvts, param)
{}

void ModulationKnob:: mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()){
		showPopupMenu();
		return;
	}
	RotaryKnob::mouseDown(event);
}
	

void ModulationKnob::showPopupMenu() {
	juce::PopupMenu menu;

	{
		juce::PopupMenu polarityMenu;
		polarityMenu.addItem(1, "Bipolar", true, _polarity == Polarity_e::Bipolar);
		polarityMenu.addItem(2, "Unipolar", true, _polarity == Polarity_e::Unipolar);
		menu.addSubMenu("Polarity", polarityMenu);
	}
	menu.addSeparator();
	{
		juce::PopupMenu sourceMenu;
		sourceMenu.addItem(10, "LFO", true, _modSource == ModSource_e::LFO);
		sourceMenu.addItem(11, "ASR", true, _modSource == ModSource_e::ASR);
		sourceMenu.addItem(12, "Mixer1", false, _modSource == ModSource_e::Mixer1);
		sourceMenu.addItem(13, "Mixer2", false, _modSource == ModSource_e::Mixer2);
		menu.addSubMenu("ModSource", sourceMenu);
	}
	
	
	menu.showMenuAsync(juce::PopupMenu::Options(),
					   [this](int result)
					   {
						   handleMenuResult(result);
					   });
}

void ModulationKnob::handleMenuResult(int result)
{
	if (result == 1) // Bipolar
	{
		setPolarity(Polarity_e::Bipolar);
		if (onPolarityChanged)
			onPolarityChanged(Polarity_e::Bipolar);
	}
	else if (result == 2) // Unipolar
	{
		setPolarity(Polarity_e::Unipolar);
		if (onPolarityChanged)
			onPolarityChanged(Polarity_e::Unipolar);
	}
	else if (result == 10) // LFO
	{
		setModSource(ModSource_e::LFO);
		if (onModSourceChanged)
			onModSourceChanged(ModSource_e::LFO);
	}
	else if (result == 11) // Envelope
	{
		setModSource(ModSource_e::ASR);
		if (onModSourceChanged)
			onModSourceChanged(ModSource_e::ASR);
	}
}
