/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CsgAudioProcessorEditor  : public AudioProcessorEditor,
                                 public Slider::Listener
{
public:
    CsgAudioProcessorEditor (CsgAudioProcessor&);
    ~CsgAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(Slider*) override;
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment>
        selfFM_SliderAttachment, MemorySliderAttachment, FMsmoothSliderAttachment, Bits_A_SliderAttachment,
        PM_preampSliderAttachment, PMsmoothSliderAttachment, Sin2CosSliderAttachment, Bits_B_SliderAttachment,
        DroneSliderAttachment, RiseSliderAttachment, FallSliderAttachment,
        CutoffSliderAttachment, ResSliderAttachment,
        LFOspeedSliderAttachment, LFOwaveSliderAttachment,
        MODselfFM_SliderAttachment, MODmemorySliderAttachment, MODFMsmoothSliderAttachment, MODBits_A_SliderAttachment,
        MODPM_preampSliderAttachment, MODPMsmoothSliderAttachment, MODSin2CosSliderAttachment, MODBits_B_SliderAttachment,
        MODCutoffSliderAttachment,
        filterTypeSlider_L_Attachment, filterTypeSlider_R_Attachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CsgAudioProcessor& processor;
    
    Slider selfFM_Slider, MemorySlider, FMsmoothSlider, Bits_A_Slider,
        PM_preampSlider, PMsmoothSlider, Sin2CosSlider, Bits_B_Slider,
        DroneSlider, RiseSlider, FallSlider, 
        CutoffSlider, ResSlider,
        LFOspeedSlider, LFOwaveSlider,
        LFO_FreqSlider,
        MODselfFM_Slider, MODmemorySlider, MODFMsmoothSlider, MODBits_A_Slider,
        MODPM_preampSlider, MODPMsmoothSlider, MODSin2CosSlider, MODBits_B_Slider,
        MODCutoffSlider;
    
    Slider filterTypeSlider_L, filterTypeSlider_R;
    
    Label selfFM_Label, MemoryLabel, FMsmoothLabel, Bits_A_Label,
        PM_preampLabel, PMsmoothLabel, Sin2CosLabel, Bits_B_Label,
        DroneLabel, RiseLabel, FallLabel,
        CutoffLabel, ResLabel,
        LFOspeedLabel, LFOwaveLabel;
    
    Font labelFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
