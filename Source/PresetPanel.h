/*
  ==============================================================================

    PresetPanel.h
    Created: 23 May 2025 7:02:25pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PresetManager.h"

namespace nvs::gui {

class PresetPanel	:
public juce::Component,
juce::Button::Listener,
juce::ComboBox::Listener
{
public:
	PresetPanel(nvs::service::PresetManager &presetManager)
	:	_presetManager(presetManager)
	{
		configureButton(saveButton, "Save");
		configureButton(deleteButton, "Delete");
		configureButton(previousPresetButton, "<");
		configureButton(nextPresetButton, ">");
		
		presetListBox.setTextWhenNothingSelected("No Preset Selected");
		presetListBox.setMouseCursor(MouseCursor::PointingHandCursor);
		addAndMakeVisible(presetListBox);
		presetListBox.addListener(this);
		
		loadPresetList();
	}
	~PresetPanel() {
		saveButton.removeListener(this);
		deleteButton.removeListener(this);
		previousPresetButton.removeListener(this);
		nextPresetButton.removeListener(this);
		presetListBox.removeListener(this);
	}
	void loadPresetList() {
		presetListBox.clear(dontSendNotification);
		
		const auto allPresets = _presetManager.getAllPresets();
		const auto currentPreset = _presetManager.getCurrentPreset();
		presetListBox.addItemList(allPresets, 1);
		presetListBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
	}
	
	void comboBoxChanged(juce::ComboBox *cb) override {
		if (cb == &presetListBox){
			_presetManager.loadPreset(presetListBox.getItemText(presetListBox.getSelectedItemIndex()));
		}
	}
	
	void configureButton(juce::Button &b, juce::String const& buttonText){
		b.setButtonText(buttonText);
		b.setMouseCursor(MouseCursor::PointingHandCursor);
		addAndMakeVisible(b);
		b.addListener(this);
	}
	void paint(juce::Graphics &g) override {
		auto const p0 = Point<float>(getX(), getY());
		auto const p1 = p0 + Point<float>(getWidth(), getHeight());
		g.setGradientFill(ColourGradient(Colour(Colours::lightgrey).withMultipliedAlpha(0.15f), p0,
										 Colour(Colours::darkgrey).withMultipliedAlpha(0.85f), p1, false));
		g.fillRect(getLocalBounds());
	}
	void resized() override
	{
		auto area = getLocalBounds().toFloat();

		// set up flex container
		FlexBox fb;
		fb.flexDirection = FlexBox::Direction::row;
		fb.alignItems    = FlexBox::AlignItems::stretch;
		fb.alignContent  = FlexBox::AlignContent::spaceAround;

		float margin = 4.0f;
		// add your controls with the same relative widths and 1px margin
		fb.items = {
			FlexItem (saveButton)               .withFlex (0.08f).withMargin (margin),
			FlexItem (previousPresetButton)     .withFlex (0.03f).withMargin (margin),
			FlexItem (presetListBox)            .withFlex (0.6f).withMargin (margin),
			FlexItem (nextPresetButton)         .withFlex (0.03f).withMargin (margin),
			FlexItem (deleteButton)             .withFlex (0.08f).withMargin (margin)
		};

		// perform the layout in our component's bounds
		fb.performLayout (area);
	}
	
private:
	void buttonClicked(juce::Button *b) override {
		if (b == &saveButton) {
			fileChooser = std::make_unique<juce::FileChooser>(
				"Enter preset name to save...",
				service::PresetManager::defaultDirectory,
				"*." + service::PresetManager::extension
			);
			fileChooser->launchAsync(FileBrowserComponent::saveMode, [&](const juce::FileChooser &fc)
			{
				const auto resultFile = fc.getResult();
				_presetManager.savePreset(resultFile.getFileNameWithoutExtension());
				loadPresetList();
			});
		}
		if (b == &previousPresetButton){
			const auto idx = _presetManager.loadPreviousPreset();
			presetListBox.setSelectedItemIndex(idx, dontSendNotification);
		}
		if (b == &nextPresetButton){
			const auto idx = _presetManager.loadNextPreset();
			presetListBox.setSelectedItemIndex(idx, dontSendNotification);
		}
		if (b == &deleteButton){
			// present warning message
#pragma message("present warning message")
			
			_presetManager.deletePreset(_presetManager.getCurrentPreset());
			loadPresetList();
		}
	}
	service::PresetManager &_presetManager;
	
	juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
	juce::ComboBox presetListBox;
	
	std::unique_ptr<juce::FileChooser> fileChooser;
};

}
