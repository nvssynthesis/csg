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
    CSGVoice()
    : //vcf(44100.f), env(44100.f),
	oversample_factor(4)
    {
        csg unit = csg(44100.f);
        
        //nkvdu_filters::onePole vcf;
//        nvs_filters::onePole vcf =  nvs_filters::onePole(44100.f);
        svf.setSampleRate(44100.f);
        svf.set_oversample(oversample_factor);
//        nvs_filters::slewlim env = nvs_filters::slewlim(44100.f);
        //simple_lfo lfo = simple_lfo(44100.f);   // may complain about unused, but it is used...
        csg_wave = 0.f;
    }
    CSGVoice(float sample_rate)
    : //vcf(sample_rate), env(sample_rate),
	oversample_factor(4)
    {
        csg unit = csg(sample_rate);

//        nvs_filters::onePole vcf = nvs_filters::onePole(sample_rate);
        svf.setSampleRate(sample_rate);
        svf.set_oversample(oversample_factor);
//        nvs_filters::slewlim env = nvs_filters::slewlim(sample_rate);
        //simple_lfo lfo = simple_lfo(sample_rate);   // may complain about unused, but it is used...
        csg_wave = 0.f;
    }
    bool canPlaySound (SynthesiserSound* sound) override
    {
        /* if it's able to cast the juce SynthesiserSound to
         the homebrewed SynthSound class, we want to mark
         it as successful. So we return true if the cast works.*/
        return (dynamic_cast<CSGSound*>(sound) != nullptr);
    }
    //===========================================================================
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override
    {
        gate = 1;
        // FOR PRO VERSION: MAKE SCALABLE TUNING, MAYBE ALLOW .TUN FILES
        frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        velocityLevel = velocity;
    }
    //===========================================================================
    void stopNote (float velocity, bool allowTailOff) override
    {
        gate = 0;
        allowTailOff = true;
        /* we are letting the synth know that the voice whose note has been
         released is now OK to use for a new note.
         This may end up being an assumption that shouldn't be made, if instead
         it would be best to wait for the release portion of the envelope to
         have fully ended.
         */
        if (velocity == 0)
            clearCurrentNote();
    }
    
    /*
     I CAN OVERRIDE NOTEOFF TO GET TO RIGHT MONO BEHAVIOR
     CALL BASE CLASS NOTE OFF, THEN IMPLEMENT OWN CODE.
     ...not done yet...
     */
    //===========================================================================
    void pitchWheelMoved (int newPitchWheelValue) override
    {
    }
    //===========================================================================
    void controllerMoved (int controllerNumber, int newControllerValue) override
    {
    }
	void setCurrentPlaybackSampleRate(double newSampleRate) override{
		vcf.setSampleRate(newSampleRate);
		env.setSampleRate(newSampleRate);
	}
    //===========================================================================
    void renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override
    {
		svf.setBlockSize(numSamples);
		oneOverBlockSize = 1.f / static_cast<float>(numSamples);
        // was in sample for loop
        //unit._freq = frequency / oversample_factor;   // 2X oversampling
        unit._freq = frequency;
        
        // later 'advanced' version will have another CSG as lfo!
        if (last_lfo_freq != lfo._freq)
            lfo._freq = last_lfo_freq;
        
//        if (last_lfo_wave != lfo._wave)
//            lfo._wave = last_lfo_wave;
        
		svf.setCutoffTarget(voice_cutoff);
		svf.setResonanceTarget(voice_res);

        env.setRiseTarget(voice_rise);
        env.setFallTarget(voice_fall);

        unit._selfFM_MOD = lfo_out * MODselfFM;
        
        unit._FM_smooth_MOD = lfo_out * MODFMsmooth;
        
        unit._bits_A_MOD = lfo_out * MODBits_A;
        
        unit._PM_preamp_MOD = lfo_out * MODPM_preamp;
        unit._PM_smooth_MOD = lfo_out * MODPMsmooth;
        
        unit._bits_B_MOD = ((lfo_out * 2 - 1) * 256.f) * MODBits_B;
        
        unit._PM_sin2cos_MOD = lfo_out * MODSin2Cos;

        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            svf.updateCutoff();
            svf.updateResonance();
            
            env.updateRise();
            env.updateFall();
            lfo.phasor();   // increment internal phase of LFO
            lfo_out = lfo.multi(last_lfo_wave);
            
            csg_wave = unit.getWave();

            svf.filter(csg_wave);
            switch (filterTypeL)
            {
                case 0:
                {
                    vcf_outL = svf.lp();
                    break;
                }
                case 1:
                {
                    vcf_outL = svf.bp();
                    break;
                }
                case 2:
                {
                    vcf_outL = svf.hp();
                    break;
                }
                case 3:
                {
                    vcf_outL = svf.np();
                    break;
                }
            }
            
            voice_droneCurrent += (voice_drone - voice_droneCurrent) * oneOverBlockSize;
            
            env_currentVal = env.ASR(gate);
            env_currentVal *= env_currentVal;
            
            outputBuffer.addSample(0, startSample,
                                   atanf(vcf_outL * nvs::memoryless::linterp<float>(env_currentVal, voice_droneCurrent, voice_droneCurrent)) * MINUS_NINE_DB);
            if (outputBuffer.getNumChannels() > 1)
            {
                switch (filterTypeR)
                {
                    case 0:
                    {
                        vcf_outR = svf.lp();
                        break;
                    }
                    case 1:
                    {
                        vcf_outR = svf.bp();
                        break;
                    }
                    case 2:
                    {
                        vcf_outR = svf.hp();
                        break;
                    }
                    case 3:
                    {
                        vcf_outR = svf.np();
                        break;
                    }
                }
                outputBuffer.addSample(1, startSample,
                                       atanf(vcf_outR * nvs::memoryless::linterp<float>(env_currentVal, voice_droneCurrent, voice_droneCurrent)) * MINUS_NINE_DB);
            }
            ++startSample;
        }
    }
    //===========================================================================
    csg unit;   // instance of the device!
    nvssynthesis_lfo::simple_lfo<float> lfo;
    nvs::filters::onePole<float> vcf;    //unused.
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
    double frequency;
    float gate, preGain, amplitude;
    float vcf_outL, vcf_outR;
    float oneOverBlockSize;
    
    int oversample_factor;
    
    double csg_wave, lfo_out;
};
