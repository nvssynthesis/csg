/*
  ==============================================================================

    PresetPanel.cpp
    Created: 23 May 2025 7:02:25pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "PresetPanel.h"
namespace nvs::gui
{

PresetPanel::PresetPanel(nvs::service::PresetManager &presetManager)
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
PresetPanel::~PresetPanel() {
	saveButton.removeListener(this);
	deleteButton.removeListener(this);
	previousPresetButton.removeListener(this);
	nextPresetButton.removeListener(this);
	presetListBox.removeListener(this);
}
void PresetPanel::loadPresetList() {
	presetListBox.clear(dontSendNotification);
	
	const auto allPresets = _presetManager.getAllPresets();
	const auto currentPreset = _presetManager.getCurrentPreset();
	presetListBox.addItemList(allPresets, 1);
	presetListBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
}

void PresetPanel::comboBoxChanged(juce::ComboBox *cb) {
	if (cb == &presetListBox){
		_presetManager.loadPreset(presetListBox.getItemText(presetListBox.getSelectedItemIndex()));
	}
}

void PresetPanel::configureButton(juce::Button &b, juce::String const& buttonText){
	b.setButtonText(buttonText);
	b.setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(b);
	b.addListener(this);
}
void PresetPanel::paint(juce::Graphics &g) {
	auto const p0 = Point<float>(getX(), getY());
	auto const p1 = p0 + Point<float>(getWidth(), getHeight());
	g.setGradientFill(ColourGradient(Colour(Colours::lightgrey).withMultipliedAlpha(0.1f), p0,
									 Colour(Colours::grey).withMultipliedAlpha(0.3f), p1, false));
	g.fillRect(getLocalBounds());
}
void PresetPanel::resized() {
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

}	// namespace nvs::gui
