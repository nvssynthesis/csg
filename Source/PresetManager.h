/*
  ==============================================================================

    PresetManager.h
    Created: 23 May 2025 7:34:50pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace nvs::service {

class PresetManager	:	juce::ValueTree::Listener
{
public:
	using String = juce::String;
	using StringArray = juce::StringArray;
	using APVTS = juce::AudioProcessorValueTreeState;
	using File = juce::File;
	using Value = juce::Value;
	
	static const File defaultDirectory;
	static const String extension;
	static const String presetNameProperty;

	PresetManager(APVTS& apvts);
	void savePreset(const String &name);
	void deletePreset(const String &name);
	void loadPreset(const String &name);
	int loadNextPreset();
	int loadPreviousPreset();
	StringArray getAllPresets() const;
	String getCurrentPreset() const;
private:
	void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;
	APVTS& _apvts;
	Value currentPreset;
};

}
