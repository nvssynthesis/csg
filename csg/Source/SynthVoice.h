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
#include "../../../gitstuff/nkvdu_libraries/nkvdu_LFO.h"
#define MINUS_NINE_DB 0.354813389233575

class CSGVoice      :       public SynthesiserVoice
{
public:
    CSGVoice()
    : oversample_factor(4)
    {
        csg unit = csg(44100.f);
        
        onePole vcf = onePole(44100.f);
        svf.setSampleRate(44100.f);
        svf.set_oversample(oversample_factor);
        slewlim env = slewlim(44100.f);
        //simple_lfo lfo = simple_lfo(44100.f);   // may complain about unused, but it is used...
        csg_wave = 0.f;
    }
    CSGVoice(float sample_rate)
    : oversample_factor(4)
    {
        csg unit = csg(sample_rate);

        onePole vcf = onePole(sample_rate);
        svf.setSampleRate(sample_rate);
        svf.set_oversample(oversample_factor);
        slewlim env = slewlim(sample_rate);
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
    //===========================================================================
    void renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override
    {
        oneOverBlockSize = 1.f / numSamples;
        svf._oneOverBlockSize = oneOverBlockSize;
        // was in sample for loop
        //unit._freq = frequency / oversample_factor;   // 2X oversampling
        unit._freq = frequency;
        
        // later 'advanced' version will have another CSG as lfo!
        if (last_lfo_freq != lfo._freq)
            lfo._freq = last_lfo_freq;
        
        if (last_lfo_wave != lfo._wave)
            lfo._wave = last_lfo_wave;
        
        svf._cutoffTarget = voice_cutoff;
        svf._resonanceTarget = voice_res;

        env._riseTarget = voice_rise;
        env._fallTarget = voice_fall;

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
            
            env.setRise();
            env.setFall();
            lfo.phasor();   // increment internal phase of LFO
            lfo_out = lfo.multi();
            
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
                                   atanf(vcf_outL * nkvdu_memoryless::linterp<float>(env_currentVal, voice_droneCurrent, voice_droneCurrent)) * MINUS_NINE_DB);
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
                                       atanf(vcf_outR * nkvdu_memoryless::linterp<float>(env_currentVal, voice_droneCurrent, voice_droneCurrent)) * MINUS_NINE_DB);
            }
            ++startSample;
        }
    }
    //===========================================================================
    csg unit;   // instance of the device!
    simple_lfo lfo;
    onePole vcf;    //unused.
    svf_nl_rk svf;
    slewlim env;
    float voice_drone, voice_droneCurrent,
    voice_rise, voice_fall, voice_cutoff, voice_res;
    float MODselfFM, MODFMsmooth, MODBits_A,
        MODPM_preamp, MODPMsmooth, MODSin2Cos, MODBits_B;
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
