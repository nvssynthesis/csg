/*
  ==============================================================================

    nkvdu_LFO.h
    Created: 14 Dec 2018 10:48:27am
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "nkvdu_memoryless.h"

class simple_lfo
{
public:
    simple_lfo()
    {
        setSampleRate(44100.f);
        _phase = 0.f;
    }
    simple_lfo(float sample_rate)
    {
        setSampleRate(sample_rate);
        _phase = 0.f;
    }
    void setSampleRate(float sample_rate)
    {
        this->sampleRate = sample_rate;
        this->T = 1.f / sample_rate;
    }
    float _freq;
    
    float phasor()
    {
        using namespace nkvdu_memoryless;
        
        _phase += _freq * T;
        _phase = mod_1<float>(_phase);
        return _phase;
    }
    float saw()
    {
        return _phase * 2 - 1;
    }
    float square()
    {
        return _phase > 0.5f ? 1.f : 0.f;
    }
    float tri()
    {
        return _phase > 0.5f ? (_phase - 0.5) * 2 : (1.f - _phase - 0.5) * 2;
    }
    
private:
    float sampleRate, T;
    float _phase;
};
