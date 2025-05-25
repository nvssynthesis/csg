/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "gui/PresetPanel.h"
#include "gui/CSG_LookAndFeel.h"
#include "gui/ModulatedSlider.h"
#include "gui/UtilityKnob.h"
#include "gui/AttachedComboBox.h"

class CsgAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    CsgAudioProcessorEditor (CsgAudioProcessor&);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    CsgAudioProcessor& processor;
	TooltipWindow  tooltipWindow;
	nvs::gui::CSG_LookAndFeel csgLAF;
	nvs::gui::PresetPanel presetPanel;
	juce::Rectangle<int> nameAndVersionBounds;
	juce::HyperlinkButton   visitSiteButton { "info & donate @ nvssynthesis",
								   URL ("https://nvssynthesis.github.io/") };
	
	std::vector<std::unique_ptr<nvs::gui::ModulatedSlider>> sliders;
	std::vector<std::unique_ptr<nvs::gui::UtilityKnob>> knobs;
	std::vector<std::unique_ptr<AttachedComboBox>> comboBoxes;
	
	std::vector<juce::Rectangle<float>> groupAreas;
	
	Image backgroundImage;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
