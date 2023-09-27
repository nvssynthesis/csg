/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "SynthVoice.h"

//==============================================================================
/**
*/
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

    NormalisableRange<float> selfFM_AmountRange, memoryRange, PM_preamp_range, sin2cosRange, smoothingRange, BitsRange,
        slewRange, droneRange, cutoffRange, resRange, lfoSpeedRange, lfoWaveRange, /*,
    selfFMMODRange, PM_preampMODrange, sin2cosMODRange, smoothingMODRange, BitsMODRange*/
    filterTypeRange;
    float oneOverBlockSize;
    
    AudioProcessorValueTreeState parameters;
private:
    Synthesiser csg_synth;
    CSGVoice* csg_voice;
    
    double lastSampleRate;
    float Last_Self_FM, Last_Memory, Last_FM_Smooth, Last_Bits_A,
        Last_PM_Preamp, Last_PM_Smooth, Last_Sin2Cos, Last_Bits_B,
        Last_Drone, Last_Rise, Last_Fall, Last_Cutoff, Last_Resonance,
        Last_lfoSpeed, Last_lfoWave,
        Last_MODselfFM, Last_MODMemory, Last_MODFMsmooth, Last_MODBits_A,
        Last_MODPM_preamp, Last_MODPMsmooth, Last_MODSin2Cos, Last_MODBits_B,
        Last_MODCutoff, Last_MODResonance;
    
    int Last_FilterTypeLeft, Last_FilterTypeRight;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessor)
};
