/*
  ==============================================================================

    AttachedComboBox.cpp
    Created: 24 May 2025 9:27:24pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "AttachedComboBox.h"

AttachedComboBox::AttachedComboBox(juce::AudioProcessorValueTreeState &apvts,
								   juce::RangedAudioParameter const &param)
:	_comboBox(),
	_attachment(apvts, param.getParameterID(), _comboBox)
{
	addAndMakeVisible(_comboBox);
}
void AttachedComboBox::resized() {
	_comboBox.setBounds(getLocalBounds());
}
