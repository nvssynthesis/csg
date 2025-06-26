/*
  ==============================================================================

    PresetPanel.h
    Created: 23 May 2025 7:02:25pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../PresetManager.h"

namespace nvs::gui {

class PresetPanel
:	public juce::Component
,	juce::Button::Listener
,	juce::ComboBox::Listener
{
public:
	PresetPanel(nvs::service::PresetManager &presetManager);
	~PresetPanel();
	void loadPresetList() ;
	
	void comboBoxChanged(juce::ComboBox *cb) override;
	
	void configureButton(juce::Button &b, juce::String const& buttonText);
	void paint(juce::Graphics &g) override;
	void resized() override;
	
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
			[[maybe_unused]]
			bool const button1Clicked = juce::AlertWindow::showOkCancelBox(
				juce::AlertWindow::WarningIcon,
				"Delete Preset",
				"Are you sure you want to delete the preset '" +
				_presetManager.getCurrentPreset() + "'?\n\nThis action cannot be undone.",
				"Delete",
				"Cancel",
				this,
				juce::ModalCallbackFunction::create([this](int result) {
					if (result == 1) { // User clicked "Delete"
						_presetManager.deletePreset(_presetManager.getCurrentPreset());
						loadPresetList();
					}
					// If result == 0, user clicked "Cancel" - do nothing
				})
			);
		}
	}
	service::PresetManager &_presetManager;
	
	juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
	juce::ComboBox presetListBox;
	
	std::unique_ptr<juce::FileChooser> fileChooser;
};

}
