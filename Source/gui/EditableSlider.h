/*
  ==============================================================================

    EditableSlider.h
    Created: 26 Jun 2025 7:24:21pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class EditableSlider	:	public juce::Slider
/**
 the basic slider behaviors i want to enable in all types of sliders
 */
{
public:
	EditableSlider() {
		setSliderSnapsToMousePosition(false);
		setVelocityBasedMode(true);
		setVelocityModeParameters(1.2,	// sensitivity
								1, 		// threshold
								0.01, 	// offset
								true,	// userCanPressKeyToSwapMode
								ModifierKeys::ctrlAltCommandModifiers	// ModifierKeys::Flags modifiersToSwapModes
							);
	}
	void mouseDoubleClick(const MouseEvent &event) override {
		setTextBoxStyle(TextEntryBoxPosition::TextBoxAbove, false, 100, 30);
		showTextBox();
	}
	void valueChanged() override {
		setTextBoxStyle(TextEntryBoxPosition::NoTextBox, false, 0, 0);
//		hideTextBox(true); // for some reason, using this alone (instead of the above setTextBoxStyle) had no effect
	}
	double getValueFromText(const String &text) override {
		valueChanged();
		
		return valueFromTextFunction(text);
	}
};
