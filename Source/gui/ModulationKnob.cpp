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
		for (int i = 0; i < nvs::param::PolarityLabels.size(); ++i){
			juce::String const &label = nvs::param::PolarityLabels[i];
			polarityMenu.addItem(i+1, label, true, _polarity_param.getCurrentChoiceName().contains(label));
		}
		menu.addSubMenu("Polarity", polarityMenu);
	}
	menu.addSeparator();
	{
		juce::PopupMenu sourceMenu;
		for (int i = 0; i < nvs::param::ModSourceLabels.size(); ++i){
			juce::String const &label = nvs::param::ModSourceLabels[i];
			
			bool const active = !label.contains("Mixer");	// mixer item not implemented yet
			sourceMenu.addItem(i+10, label, active, _mod_source_param.getCurrentChoiceName().contains(label));
		}
		menu.addSubMenu("ModSource", sourceMenu);
	}
	
	
	menu.showMenuAsync(juce::PopupMenu::Options(),
					   [this](int result)
					   {
						   handleMenuResult(result);
					   });
}
