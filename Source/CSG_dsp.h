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
namespace csg {
struct SharedState {
	SharedState(juce::AudioProcessorValueTreeState &apvts)	:	smoothedParamsManager(apvts){}
	nvs::param::SmoothedParamsManager smoothedParamsManager;
	
	std::vector<float const*> modSources;
	
	std::unordered_map<nvs::param::parameter_e, juce::AudioParameterChoice*> polarityParams;
	std::unordered_map<nvs::param::parameter_e, juce::AudioParameterChoice*> modSourceParams;
};
}
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

inline float calcLinearRange(nvs::param::SmoothedParamsManager const &smoothed, param::parameter_e pid){
#pragma message("This is somewhat of a bottleneck-string comparison for every param at every sample. TODO: cache the ranges instead.")
	auto normRange = smoothed.getRange(pid);
	return normRange.end - normRange.start;
}
inline float calcLinearModdedVal(nvs::csg::SharedState &state, param::parameter_e pid){
	auto const polarityIdx = state.polarityParams[pid]->getIndex();
	auto const modSourceIdx = state.modSourceParams[pid]->getIndex();
	
	auto const &modSources = state.modSources;
	
	auto const modVal = [modSources, modSourceIdx, polarityIdx]() -> float
	{
		decltype(modSourceIdx) constexpr LFOidx = 0;
		decltype(modSourceIdx) constexpr ASRidx = 1;
		decltype(modSourceIdx) constexpr mixer1idx = 2;
		decltype(modSourceIdx) constexpr mixer2idx = 3;
		
		decltype(polarityIdx) constexpr bipolarIdx = 0;

		if (modSourceIdx == LFOidx) {
			return (polarityIdx == bipolarIdx) ? *modSources[modSourceIdx] : nvs::memoryless::biuni(*modSources[modSourceIdx]);
		}
		else if (modSourceIdx == ASRidx) {
			return (polarityIdx == bipolarIdx) ? nvs::memoryless::unibi(*modSources[modSourceIdx]) : *modSources[modSourceIdx];
		}
		jassertfalse;
		return 0.f;
	}();
	
	auto &smoothed = state.smoothedParamsManager;
	
	auto const retval = smoothed.getNextValue(pid) + modVal * smoothed.getNextValue(basePIDToModPID(pid)) * calcLinearRange(smoothed, pid);
	jassert (retval == retval);
	return retval;
}
inline float calcLogModdedVal(nvs::csg::SharedState &state, param::parameter_e pid){
	auto const &modSources = state.modSources;
	
	auto &smoothed = state.smoothedParamsManager;
	
	auto const modSourceIdx = state.modSourceParams[pid]->getIndex();
	
	auto const modVal = *modSources[modSourceIdx];
	auto const baseValHz = smoothed.getNextValue(pid);
	auto const modAmtVal = smoothed.getNextValue(basePIDToModPID(pid));
	
	auto const baseValSemitones = hertzToMidi(baseValHz);

	
	auto const polarityIdx = state.polarityParams[pid]->getIndex();
	

	
	float const min = [polarityIdx, modSourceIdx](){
		decltype(modSourceIdx) constexpr LFOidx = 0;
		decltype(modSourceIdx) constexpr ASRidx = 1;
	//	decltype(modSourceIdx) constexpr mixer1idx = 2;
	//	decltype(modSourceIdx) constexpr mixer2idx = 3;
		
		decltype(polarityIdx) constexpr bipolarIdx = 0;

		if (modSourceIdx == LFOidx) {
			if (polarityIdx == bipolarIdx) {
				return -1.f;
			}
			else {
				return 0.f;
			}
		}
		else if (modSourceIdx == (ASRidx)) {
			if (polarityIdx == bipolarIdx) {
				return 0.f; // mapping an already unipolar signal to a bp output
			}
			else {
				return -1.f; // mapping a bp signal to remain so
			}
		}
		return 0.f;
	}();
	
	if (polarityIdx == 0){}
	auto const modValSemitones = jmap(modVal, min, 1.f, -64.f, 64.f) * modAmtVal;
	
	auto const finalValSemitones = baseValSemitones + modValSemitones;
	return midiToHertz(finalValSemitones);
}

namespace csg {
class CSG {
public:
	explicit CSG(SharedState *sharedState);
	
	void setSampleRate(double sample_rate);
	double getSampleRate() const;
	void setFrequencyMultiplier(double freqMult);
	void clearDelay();
	float phasor();
	float phasor_fm(float sample);
	float getWave();
	
private:
	double fs, fs_delta;
	double _freq_multiplier {1.0};
	double _phase;  // master phasor
	
	SharedState *_sstate;
	
	nvs::filters::onePole<float> FM_filter, PM_filter;   // for smoothing feedback modulations
	nvs::filters::dcBlock<float> dc_b;
	
	int wp, rp, rp_neighbor, zLength;     // iterators for delay
	
	double base_freq;
	
	nvs::memoryless::CosTable<float, double, 16384, nvs::memoryless::InterpolationType::Linear> cosTable;
	
	//float zLine[16];
	juce::AudioSampleBuffer zLine;
	
	bool sampleRateValid() const;
};
}	// namespace csg
}	// namespace nvs
