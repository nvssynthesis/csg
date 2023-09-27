/*
  ==============================================================================

    CSG_dsp.h
    Created: 5 Dec 2018 8:48:55pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "include/nvs_filters.h"
#include "../JuceLibraryCode/JuceHeader.h"


class csg
{
public:
    csg();
    csg(float sample_rate);
    ~csg();
    
    void setSampleRate(float sample_rate);
    void clearDelay();
    float phasor();
    float phasor_fm(float sample);
    
    float _oneOverBlockSize;
    double fs, fs_delta, _freq;
    
    float getWave();
    
    float selfFM_target, memory_target, FM_smooth_target, bits1_target,
            PM_preamp_target, PM_sin2cos_target, bits2_target, PM_smooth_target;
    
    float _selfFM_MOD, _FM_smooth_MOD, _bits_A_MOD, _PM_preamp_MOD, _PM_sin2cos_MOD, _bits_B_MOD, _PM_smooth_MOD;
    
private:
    double _phase;  // master phasor
    nvs_filters::onePole<float> FM_filter, PM_filter;   // for smoothing feedback modulations
    nvs_filters::dcBlock<float> dc_b;

    int wp, rp, rp_neighbor, zLength;     // iterators for delay
    
    float selfPM, selfFM, memory, shaperPreamp;
    float crushed_freqmod_sig, crushed_phasemod_sig, weighted_sincos, sinwin, junction;
    float output, z2, feedback;
    
    //FM = frequency modulate,
    //PM = phase modulate.
    double bits1, bits2, self_FM, FM_smooth, PM_sin2cos, PM_preamp, PM_smooth;
    nvs::memoryless::trigTables<double> trig_tables;
    
    //float zLine[16];
    juce::AudioSampleBuffer zLine;
    
};
