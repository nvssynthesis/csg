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
