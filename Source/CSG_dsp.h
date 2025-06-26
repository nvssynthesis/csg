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
inline float calcLinearModdedVal(nvs::param::SmoothedParamsManager &smoothed, param::PID_e pid, std::vector<float const*> modSources){
	auto const modVal = *modSources[0];
	return smoothed.getNextValue(pid) + modVal * smoothed.getNextValue(basePIDToModPID(pid)) * calcLinearRange(smoothed, pid);
}
inline float calcLogModdedVal(nvs::param::SmoothedParamsManager &smoothed, param::PID_e pid, std::vector<float const*> modSources){
	auto const modVal = *modSources[0];
	auto const baseValHz = smoothed.getNextValue(pid);
	auto const modAmtVal = smoothed.getNextValue(basePIDToModPID(pid)) * modVal;
	
	auto baseValSemitones = hertzToMidi(baseValHz);
	auto modValSemitones = jmap(modAmtVal, -1.f, 1.f, -64.f, 64.f);
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
	
	/*
	 TODO: figure out better schema for coupling direct params and their modulation counterparts, and make raw values private
	 */
	
//	float _selfFM_MOD, _FM_smooth_MOD, _bits_A_MOD, _PM_preamp_MOD, _PM_sin2cos_MOD, _bits_B_MOD, _PM_smooth_MOD;
	void addModSource(float const* source){
		modSources.push_back(source);
	}
private:
	double fs, fs_delta;
	double _freq {110.0};
	double _phase;  // master phasor
	
	std::vector<float const*> modSources;
	
	nvs::param::SmoothedParamsManager *_smoothedParams;
	
	nvs::filters::onePole<float> FM_filter, PM_filter;   // for smoothing feedback modulations
	nvs::filters::dcBlock<float> dc_b;
	
	int wp, rp, rp_neighbor, zLength;     // iterators for delay
	
	float base_freq, selfFM, memory;
	
	//FM = frequency modulate,
	//PM = phase modulate.
	double bits1, bits2, FM_smooth, PM_sin2cos, PM_preamp, PM_smooth;
	nvs::memoryless::CosTable<float, double, 16384, nvs::memoryless::InterpolationType::Linear> cosTable;
	
	//float zLine[16];
	juce::AudioSampleBuffer zLine;
	
	bool sampleRateValid() const;
};
}	// namespace csg
}	// namespace nvs
