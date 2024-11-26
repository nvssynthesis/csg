/*
  ==============================================================================

    SynthVoice.h
    Created: 5 Dec 2018 8:47:46pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthSound.h"
#include "CSG_dsp.h"
#include "nvs_libraries/include/nvs_LFO.h"
//#include "src/nvs_LFO.cpp"
#include "nvs_libraries/include/nvs_filters.h"
//#include "src/nvs_filters.cpp"
#define MINUS_NINE_DB 0.354813389233575

class CSGVoice      :       public SynthesiserVoice
{
public:
	CSGVoice();
	bool canPlaySound (SynthesiserSound* sound) override;
    //===========================================================================
	void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
    //===========================================================================
	void stopNote (float velocity, bool allowTailOff) override;
	void pitchWheelMoved (int newPitchWheelValue) override;
    //===========================================================================
	void controllerMoved (int controllerNumber, int newControllerValue) override;
	void setCurrentPlaybackSampleRate(double newSampleRate) override;
    //===========================================================================
	void renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    //===========================================================================
    
	
public:
	/*
	 goal: make this section private
	 */
	csg unit;
    nvssynthesis_lfo::simple_lfo<float> lfo;
    nvs::filters::svf_nl_rk<float> svf;
    nvs::filters::slewlim<float> env;
    
	float voice_drone, voice_droneCurrent,
        voice_rise, voice_fall, voice_cutoff, voice_res;
    float MODselfFM, MODMemory, MODFMsmooth, MODBits_A,
        MODPM_preamp, MODPMsmooth, MODSin2Cos, MODBits_B,
        MODCutoff, MODResonance;
	
    float last_lfo_freq, last_lfo_wave;
    int filterTypeL, filterTypeR;
    float env_currentVal;

private:
    double velocityLevel;
	double frequency {110.0};
	
	juce::LinearSmoothedValue<float> cutoff_smoothed_;

    float gate, preGain, amplitude;
    float vcf_outL, vcf_outR;
    float oneOverBlockSize;
    
    int oversample_factor;
    
	double csg_wave{0.0}, lfo_out{0.0};
};
