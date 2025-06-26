/*
  ==============================================================================

    UtilityKnob.cpp
    Created: 24 May 2025 9:27:04pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "UtilityKnob.h"
#include "util.h"

namespace nvs::gui
{
UtilityKnob::UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
						 juce::RangedAudioParameter const &param,
						 int numDecimalPlacesToDisplay)
:	_knob(apvts, param),
_attachment(apvts, param.getParameterID(), _knob),
_paramName(param.getName(20))
{
	setupSlider(apvts, param.getParameterID(), _knob);
	
	_label.setFont({"Palatino", 10.f, 0});
	_label.attachToComponent(&_knob, false);
	_label.setText(_paramName, dontSendNotification);
	
	addAndMakeVisible(_knob);
	addAndMakeVisible(_label);
}


void UtilityKnob::resized() {
	_knob.setBounds(getLocalBounds());
}

}
