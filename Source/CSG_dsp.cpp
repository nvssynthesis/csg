/*
  ==============================================================================

    CSG_dsp.cpp
    Created: 5 Dec 2018 8:48:55pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "CSG_dsp.h"
//#include <iostream>

csg::csg()
    : _oneOverBlockSize(0.0009765625f), _freq(110.f), _phase(0.f), memory(2.f), output(0.f), bits1(2048.f), bits2(2048.f), zLine(1, 32)
{
    setSampleRate(44100.f);
    clearDelay();
}
csg::csg(float sample_rate)
    : _oneOverBlockSize(0.0009765625f), _freq(110.f), _phase(0.f), memory(2.f), output(0.f), bits1(2048.f), bits2(2048.f), zLine(1, 32)
{
    setSampleRate(sample_rate);
    clearDelay();
}
csg::~csg()
{
}

void csg::setSampleRate(float sample_rate)
{
    fs = sample_rate;
    fs_delta = 1.f / sample_rate;
    FM_filter.setSampleRate(sample_rate);
    PM_filter.setSampleRate(sample_rate);
    dc_b.setSampleRate(sample_rate);
}
void csg::clearDelay()
{
    //zLength = (sizeof(zLine)/sizeof(*zLine));
    zLength = zLine.getNumSamples() - 1;
    zLine.clear();
    //std::cout << zLength;
    rp = 0;
    rp_neighbor = zLength;
    wp = zLength;
}

float csg::phasor()
{
    using namespace nvs::memoryless;
    _phase += _freq * fs_delta;
    _phase = mod_1<double>(_phase);
    return _phase;
}
float csg::phasor_fm(float sample)
{
    using namespace nvs::memoryless;
    _phase += (_freq + sample) * fs_delta;
    _phase = mod_1<double>(_phase);
    return _phase;
}

float csg::getWave()
{
    using namespace nvs::memoryless;
    
    //update params===================================================================================
    if (selfFM_target != selfFM)
        selfFM      += (selfFM_target - selfFM) * _oneOverBlockSize;
    if (memory_target != memory)
        memory      += (memory_target - memory) * _oneOverBlockSize;
    if (FM_smooth_target != FM_smooth)
        FM_smooth   += (FM_smooth_target - FM_smooth) * _oneOverBlockSize;
    if (bits1_target != bits1)
        bits1       += (bits1_target - bits1) * _oneOverBlockSize;
    if (PM_preamp_target != PM_preamp)
        PM_preamp   += (PM_preamp_target - PM_preamp) * _oneOverBlockSize;
    if (PM_smooth_target != PM_smooth)
        PM_smooth   += (PM_smooth_target - PM_smooth) * _oneOverBlockSize;
    if (PM_sin2cos_target != PM_sin2cos)
        PM_sin2cos   += (PM_sin2cos_target - PM_sin2cos) * _oneOverBlockSize;
    if (bits2_target != bits2)
        bits2       += (bits2_target - bits2) * _oneOverBlockSize;
    //end update params===============================================================================
    

    while (rp < 0)
        rp = (rp + zLength) - 1;
    while (rp > zLength)
        rp -= zLength;
    while (rp_neighbor < 0)
        rp_neighbor = (rp_neighbor + zLength) - 1;
    while (rp_neighbor > zLength)
        rp_neighbor -= zLength;

    // take last output, get derivative, variably lowpass it to smoothen, and amplify it based on FM mod amount.
    //feedback = FM_filter.tpt_lp((output - z2) * (fs * 0.000021) * pow((clamp<float>(selfFM + _selfFM_MOD, 0.f, 1.f) * 29), 3.f), clamp<float>(FM_smooth + (FM_smooth * _FM_smooth_MOD * 3.f), 20.f, 20000.f));
    float fedback_delta = dc_b.filter(dc_b.filter(zLine.getSample(0, rp) - zLine.getSample(0, rp_neighbor)));
//    float fedback_delta = dc_b.filter(zLine.getSample(0, rp));
    float feedback_amt = (clamp<float>(selfFM + _selfFM_MOD, 0.0f, 1.f) * 24000.f);
    float feedback_cutoff = clamp<float>(FM_smooth + (FM_smooth * _FM_smooth_MOD * 3.f), 20.f, 20000.f);
	
	FM_filter.setCutoff(feedback_cutoff); // not smoothed currently
	
    feedback = FM_filter(fedback_delta * (fs * 0.000021f) * feedback_amt);
    
    // bitcrush that fed-back signal.
    crushed_freqmod_sig = crush<float>(phasor_fm(feedback), clamp<float>(bits1 + (_bits_A_MOD * bits1), 1.01f, 2048.f));
    
    // on other hand, take sin and cos of phasor to do phase modulation (what will basically sound like wavefolding).
    // we can fade between the sin and cos parts with equal power.
    weighted_sincos = ((trig_tables.up_sin_LUT(_phase) *  sqrt(1.f - (PM_sin2cos + _PM_sin2cos_MOD))) + (trig_tables.up_cos_LUT(_phase * 2.f - 1.f) * sqrt(PM_sin2cos + _PM_sin2cos_MOD))) * (PM_preamp + _PM_preamp_MOD);
    
    // now, bitcrush and filter that signal.
	PM_filter.setCutoff(PM_smooth);	// not smoothed currently
    sinwin = PM_filter(crush<float>(weighted_sincos, clamp<float>(bits2 + (_bits_B_MOD * bits2), 1.01f, 2048.f) ));
    
    // sum the FM part and PM part. Make them both bipolar.
    junction = unibi<float>(crushed_freqmod_sig) + sinwin;
    // 2 samples ago, used for derivative.
    //z2 = output;

    //increment delay line============================================================================
    wp -= 1;
    wp = fmodf(wp, memory);
    rp = wp + (int)memory;
    rp = fmodf(rp, memory);
    rp_neighbor = rp + 1;
    rp_neighbor = fmodf(rp_neighbor, memory);
    //end increment delay line========================================================================

    if (wp < 0)
        wp = zLength;
    if (wp > zLength)
        wp = 0;

    
    // now take the sin of the summed signals! this is the CSG.
    output = (float)trig_tables.bp_sin_LUT(wrap<float>(junction, -1.f, 1.f));
    
    zLine.setSample(0, wp, output);

    return output;
}
