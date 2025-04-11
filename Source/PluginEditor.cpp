/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
CsgAudioProcessorEditor::CsgAudioProcessorEditor (CsgAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
    labelFont("Palatino", 13.f, 0)
{
    setSize (640, 431);
    
    for (int n=0; n<N_PARAMS_MAIN; n++){
        (*sliders_main[n]).setSliderStyle(Slider::LinearBarVertical);
        (*sliders_main[n]).setNumDecimalPlacesToDisplay(3);
        (*sliders_main[n]).setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 104, 32);
        addAndMakeVisible(sliders_main[n]);
        (*sliders_main[n]).addListener(this);
        (*labels[n]).setFont(labelFont);
        (*labels[n]).attachToComponent(sliders_main[n], false);
    }
    selfFM_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Self_FM", selfFM_Slider);
    selfFM_Slider.setRange((double)processor.selfFM_AmountRange.start, (double)processor.selfFM_AmountRange.end, 0.01f);
    selfFM_Slider.setValue(0.f);
    selfFM_Label.setText("SELF-FM", dontSendNotification);
    
    MemorySliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Memory", MemorySlider);
    MemorySlider.setRange((double)processor.memoryRange.start, (double)processor.memoryRange.end, 0.01f);
    MemorySlider.setValue(1.f);
    MemoryLabel.setText("MEMORY", dontSendNotification);
    
    FMsmoothSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "FM_Smooth", FMsmoothSlider);
    FMsmoothSlider.setRange((double)processor.smoothingRange.start, (double)processor.smoothingRange.end, 0.01f);
    FMsmoothSlider.setSkewFactorFromMidPoint(2000.f);
    FMsmoothSlider.setValue(2000.f);
    FMsmoothLabel.setText("FM SMOOTH", dontSendNotification);
    
    Bits_A_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Bits_A", Bits_A_Slider);
    Bits_A_Slider.setRange((double)processor.BitsRange.start, (double)processor.BitsRange.end, 0.001f);
    Bits_A_Slider.setSkewFactorFromMidPoint(6.f);
    Bits_A_Slider.setValue(1024.f);
    Bits_A_Label.setText("FM DEGRADE", dontSendNotification);
    
    PM_preampSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "PM_Preamp", PM_preampSlider);
    PM_preampSlider.setRange((double)processor.PM_preamp_range.start, (double)processor.PM_preamp_range.end, 0.01f);
    PM_preampSlider.setSkewFactorFromMidPoint(2.f);
    PM_preampSlider.setValue(0.f);
    PM_preampLabel.setText("PM AMOUNT", dontSendNotification);
    
    PMsmoothSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "PM_Smooth", PMsmoothSlider);
    PMsmoothSlider.setRange((double)processor.smoothingRange.start, (double)processor.smoothingRange.end, 0.01f);
    PMsmoothSlider.setSkewFactorFromMidPoint(2000.f);
    PMsmoothSlider.setValue(2000.f);
    PMsmoothLabel.setText("PM TAME", dontSendNotification);
    
    Sin2CosSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Sin2Cos", Sin2CosSlider);
    Sin2CosSlider.setRange((double)processor.sin2cosRange.start, (double)processor.sin2cosRange.end, 0.01f);
    Sin2CosSlider.setNumDecimalPlacesToDisplay(3);
    Sin2CosSlider.setValue(0.0f);
    Sin2CosLabel.setText("PM SHAPE", dontSendNotification);

    Bits_B_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Bits_B", Bits_B_Slider);
    Bits_B_Slider.setRange((double)processor.BitsRange.start, (double)processor.BitsRange.end, 0.001f);
    Bits_B_Slider.setSkewFactorFromMidPoint(6.f);
    Bits_B_Slider.setValue(1024.f);
    Bits_B_Label.setText("PM DEGRADE", dontSendNotification);

    
    DroneSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Drone", DroneSlider);
    DroneSlider.setRange((double)processor.droneRange.start, (double)processor.droneRange.end, 0.01f);
    DroneSlider.setSkewFactorFromMidPoint(0.707f);
    DroneSlider.setValue(0.f);
    DroneLabel.setText("DRONE", dontSendNotification);
    
    RiseSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Rise", RiseSlider);
    RiseSlider.setRange((double)processor.slewRange.start, (double)processor.slewRange.end, 0.01f);
    RiseSlider.setSkewFactorFromMidPoint(100.f);
    RiseSlider.setValue(1.f);
    RiseLabel.setText("RISE", dontSendNotification);
    
    FallSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Fall", FallSlider);
    FallSlider.setRange((double)processor.slewRange.start, (double)processor.slewRange.end, 0.01f);
    FallSlider.setSkewFactorFromMidPoint(100.f);
    FallSlider.setValue(1.f);
    FallLabel.setText("FALL", dontSendNotification);
    
    CutoffSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Cutoff", CutoffSlider);
    CutoffSlider.setRange((double)processor.cutoffRange.start, (double)processor.cutoffRange.end, 0.001f);
    CutoffSlider.setSkewFactorFromMidPoint(1000.f);
    CutoffSlider.setValue(16000.f);
    CutoffLabel.setText("CUTOFF", dontSendNotification);
    
    ResSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Resonance", ResSlider);
    ResSlider.setRange((double)processor.resRange.start, (double)processor.resRange.end, 0.001f);
    ResSlider.setValue(1.f);
    ResLabel.setText("RESON", dontSendNotification);
    
    LFOspeedSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "LFO_Speed", LFOspeedSlider);
    LFOspeedSlider.setRange((double)processor.lfoSpeedRange.start, (double)processor.lfoSpeedRange.end, 0.001f);
    LFOspeedSlider.setValue(1.f);
    LFOspeedLabel.setText("LFO RATE", dontSendNotification);
    
    LFOwaveSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "LFO_Wave", LFOwaveSlider);
    LFOwaveSlider.setRange((double)processor.lfoWaveRange.start, (double)processor.lfoWaveRange.end, 0.001f);
    LFOwaveSlider.setValue(0.f);
    LFOwaveLabel.setText("LFO WAVE", dontSendNotification);
    for (int n=0; n<N_PARAMS_MOD;n++){
        (*sliders_mod[n]).setSliderStyle(Slider::Rotary);
        (*sliders_mod[n]).setTextBoxStyle(Slider::NoTextBox, false, 10, 10);
        (*sliders_mod[n]).setValue(0.f);
        addAndMakeVisible(sliders_mod[n]);
        (*sliders_mod[n]).addListener(this);
    }
    MODselfFM_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Self_FM_MOD", MODselfFM_Slider);
    MODselfFM_Slider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODmemorySliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Memory_MOD", MODmemorySlider);
    MODmemorySlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODFMsmoothSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "FM_Smooth_MOD", MODFMsmoothSlider);
    MODFMsmoothSlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODBits_A_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Bits_A_MOD", MODBits_A_Slider);
    MODBits_A_Slider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODPM_preampSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "PM_Preamp_MOD", MODPM_preampSlider);
    MODPM_preampSlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODPMsmoothSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "PM_Smooth_MOD", MODPMsmoothSlider);
    MODPMsmoothSlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODSin2CosSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Sin2Cos_MOD", MODSin2CosSlider);
    MODSin2CosSlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODBits_B_SliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Bits_B_MOD", MODBits_B_Slider);
    MODBits_B_Slider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    MODCutoffSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Cutoff_MOD", MODCutoffSlider);
    MODCutoffSlider.setRange(0.f, 1.f, 0.01f);    // will just be scaled by processor
    
    filterTypeSlider_L_Attachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Filter Type Left", filterTypeSlider_L);
    filterTypeSlider_L.setRange(0.f, 3.f, 1.f);
    filterTypeSlider_L.setSliderStyle(Slider::LinearBarVertical);
    filterTypeSlider_L.setTextBoxStyle(Slider::NoTextBox, false, 10, 10);
    filterTypeSlider_L.setValue(0);
    addAndMakeVisible(&filterTypeSlider_L);
    filterTypeSlider_L.addListener(this);
    
    filterTypeSlider_R_Attachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "Filter Type Right", filterTypeSlider_R);
    filterTypeSlider_R.setRange(0.f, 3.f, 1.f);
    filterTypeSlider_R.setSliderStyle(Slider::LinearBarVertical);
    filterTypeSlider_R.setTextBoxStyle(Slider::NoTextBox, false, 10, 10);
    filterTypeSlider_R.setValue(0);
    addAndMakeVisible(&filterTypeSlider_R);
    filterTypeSlider_R.addListener(this);
}

CsgAudioProcessorEditor::~CsgAudioProcessorEditor()
{
    selfFM_SliderAttachment = nullptr;
    MemorySliderAttachment = nullptr;
    FMsmoothSliderAttachment = nullptr;
    Bits_A_SliderAttachment = nullptr;
    PM_preampSliderAttachment = nullptr;
    PMsmoothSliderAttachment = nullptr;
    Sin2CosSliderAttachment = nullptr;
    Bits_B_SliderAttachment = nullptr;
    
    DroneSliderAttachment = nullptr;
    RiseSliderAttachment = nullptr;
    FallSliderAttachment = nullptr;
    
    CutoffSliderAttachment = nullptr;
    ResSliderAttachment = nullptr;
    
    LFOspeedSliderAttachment = nullptr;
    LFOwaveSliderAttachment = nullptr;
    
    MODselfFM_SliderAttachment = nullptr;
    MODmemorySliderAttachment = nullptr;
    MODFMsmoothSliderAttachment = nullptr;
    MODBits_A_SliderAttachment = nullptr;
    MODPM_preampSliderAttachment = nullptr;
    MODPMsmoothSliderAttachment = nullptr;
    MODSin2CosSliderAttachment = nullptr;
    MODBits_B_SliderAttachment = nullptr;
    MODCutoffSliderAttachment = nullptr;
    
    filterTypeSlider_L_Attachment = nullptr;
    filterTypeSlider_R_Attachment = nullptr;
}

//==============================================================================
void CsgAudioProcessorEditor::paint (Graphics& g)
{   // let's make this plugin feel like a DIY noise synth
    Image background = ImageCache::getFromMemory (BinaryData::enclosure_medium_png, BinaryData::enclosure_medium_pngSize);
    g.drawImageAt (background, 0, 0);
	g.setColour(Colours::white);
#ifdef JUCE_DEBUG
	g.drawText("debug", 1, 1, 100, 10, Justification::topLeft);
#endif
}

void CsgAudioProcessorEditor::resized()
{
    // nice to base positions on proportion of total size.
    float x_offset = 0.04;
    float y_offset = 0.08;
    float width = 0.11;
    float height = 0.3;
    
    selfFM_Slider.setBounds(proportionOfWidth(x_offset + width * 0),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height
    
    MemorySlider.setBounds(proportionOfWidth(x_offset + width * 1),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height
    
    FMsmoothSlider.setBounds(proportionOfWidth(x_offset + width * 2),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height
    
    Bits_A_Slider.setBounds(proportionOfWidth(x_offset + width * 3),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height

    PM_preampSlider.setBounds(proportionOfWidth(x_offset + width * 4),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height

    PMsmoothSlider.setBounds(proportionOfWidth(x_offset + width * 5),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height

    Sin2CosSlider.setBounds(proportionOfWidth(x_offset + width * 6),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height

    Bits_B_Slider.setBounds(proportionOfWidth(x_offset + width * 7),  // x
                            proportionOfHeight(y_offset),             // y
                            proportionOfWidth(width),                 // width
                            proportionOfHeight(height));              // height

    float env_x_offset = 0.6;
    float env_y_offset = .56;
    float env_width = width;
    float env_height = height;
    
    DroneSlider.setBounds(proportionOfWidth(env_x_offset + env_width * 0), // x
                            proportionOfHeight(env_y_offset),             // y
                            proportionOfWidth(env_width),                 // width
                            proportionOfHeight(env_height));              // height
    
    RiseSlider.setBounds(proportionOfWidth(env_x_offset + env_width * 1), // x
                            proportionOfHeight(env_y_offset),             // y
                            proportionOfWidth(env_width),                 // width
                            proportionOfHeight(env_height));              // height
    
    FallSlider.setBounds(proportionOfWidth(env_x_offset + env_width * 2), // x
                            proportionOfHeight(env_y_offset),             // y
                            proportionOfWidth(env_width),                 // width
                            proportionOfHeight(env_height));              // height
    
    float filt_x_offset = x_offset;
    float filt_y_offset = env_y_offset;
    float filt_width = width;
    float filt_height = height;
    
    CutoffSlider.setBounds(proportionOfWidth(filt_x_offset + filt_width * 0), // x
                         proportionOfHeight(filt_y_offset),             // y
                         proportionOfWidth(filt_width),                 // width
                         proportionOfHeight(filt_height));              // height
    
    ResSlider.setBounds(proportionOfWidth(filt_x_offset + filt_width * 1), // x
                         proportionOfHeight(filt_y_offset),             // y
                         proportionOfWidth(filt_width),                 // width
                         proportionOfHeight(filt_height));              // height
    
    LFOspeedSlider.setBounds(proportionOfWidth(filt_x_offset + filt_width * 2.5), // x
                             proportionOfHeight(filt_y_offset),             // y
                             proportionOfWidth(filt_width),                 // width
                             proportionOfHeight(filt_height));              // height
    
    LFOwaveSlider.setBounds(proportionOfWidth(filt_x_offset + filt_width * 3.5), // x
                            proportionOfHeight(filt_y_offset),             // y
                            proportionOfWidth(filt_width),                 // width
                            proportionOfHeight(filt_height));              // height
    
    
    filterTypeSlider_L.setBounds(proportionOfWidth(filt_x_offset + filt_width * 2.1), // x
                               proportionOfHeight(filt_y_offset),
                               proportionOfWidth(width * 0.1),
                               proportionOfHeight(height * 0.5));
    
    filterTypeSlider_R.setBounds(proportionOfWidth(filt_x_offset + filt_width * 2.3), // x
                                 proportionOfHeight(filt_y_offset),
                                 proportionOfWidth(width * 0.1),
                                 proportionOfHeight(height * 0.5));
    
    float y_under = 0.01;
    float x_compensation = 0.08;
    float knob_dim = 56;
    
    MODselfFM_Slider.setBounds(selfFM_Slider.getX() - selfFM_Slider.getWidth() * x_compensation,
                               selfFM_Slider.getY() + selfFM_Slider.getHeight() - proportionOfHeight(y_under),
                               knob_dim,
                               knob_dim);
    
    MODmemorySlider.setBounds(MemorySlider.getX() - MemorySlider.getWidth() * x_compensation,
                              MemorySlider.getY() + MemorySlider.getHeight() - proportionOfHeight(y_under),
                              knob_dim,
                              knob_dim);
    
    MODFMsmoothSlider.setBounds(FMsmoothSlider.getX() - FMsmoothSlider.getWidth() * x_compensation,
                                FMsmoothSlider.getY() + FMsmoothSlider.getHeight() - proportionOfHeight(y_under),
                                knob_dim,
                                knob_dim);
    
    MODBits_A_Slider.setBounds(Bits_A_Slider.getX() - Bits_A_Slider.getWidth() * x_compensation,
                               Bits_A_Slider.getY() + Bits_A_Slider.getHeight() - proportionOfHeight(y_under),
                               knob_dim,
                               knob_dim);
    
    MODPM_preampSlider.setBounds(PM_preampSlider.getX() - PM_preampSlider.getWidth() * x_compensation,
                                 PM_preampSlider.getY() + PM_preampSlider.getHeight() - proportionOfHeight(y_under),
                                 knob_dim,
                                 knob_dim);
    
    MODPMsmoothSlider.setBounds(PMsmoothSlider.getX() - PMsmoothSlider.getWidth() * x_compensation,
                                PMsmoothSlider.getY() + PMsmoothSlider.getHeight() - proportionOfHeight(y_under),
                                knob_dim,
                                knob_dim);
    
    MODSin2CosSlider.setBounds(Sin2CosSlider.getX() - Sin2CosSlider.getWidth() * x_compensation,
                               Sin2CosSlider.getY() + Sin2CosSlider.getHeight() - proportionOfHeight(y_under),
                               knob_dim,
                               knob_dim);
    
    MODBits_B_Slider.setBounds(Bits_B_Slider.getX() - Bits_B_Slider.getWidth() * x_compensation,
                               Bits_B_Slider.getY() + Bits_B_Slider.getHeight() - proportionOfHeight(y_under),
                               knob_dim,
                               knob_dim);
    
    MODCutoffSlider.setBounds(CutoffSlider.getX() - CutoffSlider.getWidth() * x_compensation,
                               CutoffSlider.getY() + CutoffSlider.getHeight() - proportionOfHeight(y_under),
                               knob_dim,
                               knob_dim);
}

void CsgAudioProcessorEditor::sliderValueChanged(Slider* sliderThatWasMoved)
{
    // nothing needed, everything changed is a parameter
}
