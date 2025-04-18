/*
  ==============================================================================

    CSG_dsp.h
    Created: 5 Dec 2018 8:48:55pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include "nvs_libraries/include/nvs_filters.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "params.h"

namespace nvs {

template<typename float_t>
inline float_t midiToHertz(float_t midiNote) // converts midiPitch to frequency in Hz
{
	return 440.0 * std::pow(2.0, ((midiNote - 69.0) / 12.0));
}
template<typename float_t>
inline float_t hertzToMidi(float_t hertz)
{
	return 12.0 * log2(hertz / 440.0) + 69.0;
}

inline float calcLinearRange(nvs::param::SmoothedParamsManager const &smoothed, param::PID_e pid){
	auto normRange = smoothed.getRange(pid);
	return normRange.end - normRange.start;
}
inline float calcLinearModdedVal(nvs::param::SmoothedParamsManager const &smoothed, param::PID_e pid, float currentBaseVal, float currentModulationValM1P1){
	return currentBaseVal + currentModulationValM1P1 * calcLinearRange(smoothed, pid);
}
inline float calcLogModdedVal(nvs::param::SmoothedParamsManager const &smoothed, param::PID_e pid, float currentBaseValHz, float currentModulationValM1P1){
	auto modValSemitones = jmap(currentModulationValM1P1, -1.f, 1.f, -64.f, 64.f);
	auto baseValSemitones = hertzToMidi(currentBaseValHz);
	auto finalValSemitones = baseValSemitones + modValSemitones;
	return midiToHertz(finalValSemitones);
}

namespace csg
{
class CSG
{
public:
	explicit CSG(nvs::param::SmoothedParamsManager *smoothedParams);
	
	void setSampleRate(double sample_rate);
	double getSampleRate() const;
	void setFrequency(float newFreq);
	void clearDelay();
	float phasor();
	float phasor_fm(float sample);
	float getWave();
	
public:
	/*
	 TODO: figure out better schema for coupling direct params and their modulation counterparts, and make raw values private
	 */
	
	float _selfFM_MOD, _FM_smooth_MOD, _bits_A_MOD, _PM_preamp_MOD, _PM_sin2cos_MOD, _bits_B_MOD, _PM_smooth_MOD;
	
private:
	double fs, fs_delta;
	double _freq {110.0};
	double _phase;  // master phasor
	
	nvs::param::SmoothedParamsManager *_smoothedParams;
	
	nvs::filters::onePole<float> FM_filter, PM_filter;   // for smoothing feedback modulations
	nvs::filters::dcBlock<float> dc_b;
	
	int wp, rp, rp_neighbor, zLength;     // iterators for delay
	
	float selfFM, memory;
	
	//FM = frequency modulate,
	//PM = phase modulate.
	double bits1, bits2, FM_smooth, PM_sin2cos, PM_preamp, PM_smooth;
	nvs::memoryless::trigTables<double> trig_tables;
	
	//float zLine[16];
	juce::AudioSampleBuffer zLine;
	
	bool sampleRateValid() const;
};
}	// namespace csg
}	// namespace nvs
