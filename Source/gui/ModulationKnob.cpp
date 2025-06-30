/*
  ==============================================================================

    ModulationKnob.cpp
    Created: 29 Jun 2025 1:32:52pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "ModulationKnob.h"
#include "../params.h"


ModulationKnob::ModulationKnob(juce::AudioProcessorValueTreeState &apvts,
		   juce::String const &parameterID)
:	RotaryKnob(apvts, *(apvts.getParameter(nvs::param::makeModID(parameterID))))
,	_apvts(apvts)
,	_polarity_param_id(nvs::param::makeModPolarityID(parameterID))
,	_mod_source_param_id(nvs::param::makeModSourceID(parameterID))
,	_polarity_param( *dynamic_cast<ChoiceParam*>(apvts.getParameter(_polarity_param_id)) )
,	_mod_source_param( *dynamic_cast<ChoiceParam*>(apvts.getParameter(_mod_source_param_id)) )
{}

void ModulationKnob:: mouseDown(const juce::MouseEvent& event) {
	if (event.mods.isRightButtonDown()){
		showPopupMenu();
		return;
	}
	RotaryKnob::mouseDown(event);
}


void ModulationKnob::handleMenuResult(int result)
{
	if (result == 1) // Bipolar
	{
		_apvts.getParameterAsValue(_polarity_param_id).setValue(static_cast<int>(Polarity_e::Bipolar));
	}
	else if (result == 2) // Unipolar
	{
		_apvts.getParameterAsValue(_polarity_param_id).setValue(static_cast<int>(Polarity_e::Unipolar));
	}
	else if (result == 10) // LFO
	{
		_apvts.getParameterAsValue(_mod_source_param_id).setValue(static_cast<int>(ModSource_e::LFO));
	}
	else if (result == 11) // ASR envelope
	{
		_apvts.getParameterAsValue(_mod_source_param_id).setValue(static_cast<int>(ModSource_e::ASR));
	}
}

void ModulationKnob::showPopupMenu() {
	juce::PopupMenu menu;
//	auto currentPolarity = static_cast<Polarity_e>(_polarity_param.getIndex());
//	auto currentSource = static_cast<ModSource_e>(_mod_source_param.getIndex());
	{
		juce::PopupMenu polarityMenu;
		polarityMenu.addItem(1, "Bipolar", true, _polarity_param.getCurrentChoiceName().contains("Bipolar"));
		polarityMenu.addItem(2, "Unipolar", true, _polarity_param.getCurrentChoiceName().contains("Unipolar"));
		menu.addSubMenu("Polarity", polarityMenu);
	}
	menu.addSeparator();
	{
		juce::PopupMenu sourceMenu;
		sourceMenu.addItem(10, "LFO", true, _mod_source_param.getCurrentChoiceName().contains("LFO"));
		sourceMenu.addItem(11, "ASR", true, _mod_source_param.getCurrentChoiceName().contains("ASR"));
		sourceMenu.addItem(12, "Mixer1", false, _mod_source_param.getCurrentChoiceName().contains("Mixer1"));
		sourceMenu.addItem(13, "Mixer2", false, _mod_source_param.getCurrentChoiceName().contains("Mixer2"));
		menu.addSubMenu("ModSource", sourceMenu);
	}
	
	
	menu.showMenuAsync(juce::PopupMenu::Options(),
					   [this](int result)
					   {
						   handleMenuResult(result);
					   });
}
