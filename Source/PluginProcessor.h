/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "SynthVoice.h"
#include "params.h"
#include "PresetManager.h"

//==============================================================================

class CsgAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    CsgAudioProcessor();
    ~CsgAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
	
	[[deprecated]] juce::AudioProcessorValueTreeState &getAPVTS();

	nvs::service::PresetManager &getPresetManager() {
		return *presetManager;
	}
private:
    Synthesiser csgSynth;
	nvs::param::Params params;
	nvs::param::SmoothedParamsManager smoothedParams;

	//==============================================================================
	std::unique_ptr<nvs::service::PresetManager> presetManager;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessor)
};

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
