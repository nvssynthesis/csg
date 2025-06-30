/*
==============================================================================

SynthVoice.cpp
Created: 26 Nov 2024 1:25:49pm
Author:  Nicholas Solem

==============================================================================
*/

#include "SynthVoice.h"

namespace nvs::csg {

CSGVoice::CSGVoice(nvs::csg::SharedState *state)
:	_sstate(state)
,	unit(state)
{
	svf.set_oversample(4);
	
	state->modSources.push_back(&lfo_out);
	state->modSources.push_back(&asr_out);
}
bool CSGVoice::canPlaySound (SynthesiserSound* sound)
{
	/* if it's able to cast the juce SynthesiserSound to
	 the homebrewed SynthSound class, we want to mark
	 it as successful. So we return true if the cast works.*/
	return (dynamic_cast<CSGSound*>(sound) != nullptr);
}
//===========================================================================
void CSGVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
	gate = 1;
	
	unit.setFrequencyMultiplier(MidiMessage::getMidiNoteInHertz(midiNoteNumber) / 440.0);

	velocityLevel = velocity;
}
//===========================================================================
void CSGVoice::stopNote (float velocity, bool allowTailOff)
{
	gate = 0;
	allowTailOff = true;
	/* we are letting the synth know that the voice whose note has been
	 released is now OK to use for a new note.
	 This may end up being an assumption that shouldn't be made, if instead
	 it would be best to wait for the release portion of the envelope to
	 have fully ended.
	 */
	if (velocity == 0) {
		clearCurrentNote();
	}
}

/*
 I CAN OVERRIDE NOTEOFF TO GET TO RIGHT MONO BEHAVIOR
 CALL BASE CLASS NOTE OFF, THEN IMPLEMENT OWN CODE.
 ...not done yet...
 */
//===========================================================================
void CSGVoice::pitchWheelMoved (int newPitchWheelValue)
{
#pragma message("implement me")
}

//===========================================================================
void CSGVoice::setCurrentPlaybackSampleRate(double sampleRate) {
	unit.setSampleRate(sampleRate);
	env.setSampleRate(sampleRate);
	svf.setSampleRate(sampleRate);
	svf.clear();
	lfo.setSampleRate(sampleRate);
}
bool CSGVoice::sampleRateValid() const {
	auto almost_equal = [](auto f1, auto f2) -> bool {
		return std::abs(f1 - f2) < 0.0001f;
	};
	return almost_equal(unit.getSampleRate(), env.getSampleRate()) == almost_equal(svf.getSampleRate(), lfo.getSampleRate()) == true;
}

//===========================================================================
void CSGVoice::prepareToPlay(double sampleRate, int samplesPerBlock) {
	setCurrentPlaybackSampleRate(sampleRate);
	_sstate->smoothedParamsManager.reset(sampleRate, samplesPerBlock);
}

unsigned int powi2(int exp) {
	// integer power of 2
	if (exp < 0) {
	  return 0;
	}
	return 1 << exp;
}

static const std::map<int, nvs::filters::character_e> filterCharacterMap {
	{0, nvs::filters::character_e::wrong},
	{1, nvs::filters::character_e::correct}
};

void CSGVoice::renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
	jassert (sampleRateValid());
	jassert (_sstate != nullptr);
	
	using param_e = nvs::param::parameter_e;

	auto &smoothedParams = _sstate->smoothedParamsManager;
	
	unsigned int const oversampleFactor = powi2(smoothedParams._apvts.getRawParameterValue(param::paramToID( param_e::OVERSAMPLE_FACTOR ))->load());
	setSVFOversampling(oversampleFactor);
	
	env.setBlockSize(numSamples);
	svf.setBlockSize(numSamples);
	svf.setCharacter(filterCharacterMap.at(smoothedParams._apvts.getRawParameterValue(param::paramToID(param_e::FILTER_CHARACTER))->load()));
	
	smoothedParams.updateTargets();
	
	
	for (int sample = 0; sample < numSamples; ++sample)
	{
		lfo._freq = calcLogModdedVal(_sstate, param_e::LFO_RATE);
		lfo.phasor();   // increment internal phase of LFO
		auto const lfo_wave_idx = calcLinearModdedVal(_sstate, param_e::LFO_WAVE);

//		lfo_out = lfo.multi(fmod(lfo_wave_idx, 4.0));
		lfo_out = lfo.multi(lfo_wave_idx);

		using namespace nvs::memoryless;
		
		float const finalCutoff = clamp<float>( calcLogModdedVal(_sstate, param_e::CUTOFF), 1.f, (float)getSampleRate() / 2.f - 50.f);
		svf.setCutoff(finalCutoff);
		
#pragma message ("resonance param must conform to mod matrix logic too")
//		auto const res = jlimit(0.0f, 6.0f, 5.0f * lfo_out * smoothedParams.getNextValue(param_e::RESO_MOD) + smoothedParams.getNextValue(param_e::RESO));
		auto const res = jlimit(0.f, 6.f, calcLinearModdedVal(_sstate, param_e::RESO));
		svf.setResonance(res);
		
		env.setRise(smoothedParams.getNextValue(param_e::RISE));
		env.setFall(smoothedParams.getNextValue(param_e::FALL));

		auto const csg_wave = unit.getWave();
		auto const drive = calcLogModdedVal(_sstate, param_e::DRIVE);
		svf(csg_wave * drive);
		
		auto getFilterVal = [&](float filterSelection){
			switch ((int)filterSelection) {
				case 0: { return svf.lp(); }
				case 1: { return svf.bp(); }
				case 2: { return svf.hp(); }
				case 3: { return svf.np(); }
				default:{ return svf.lp(); }
			}
		};
		
		asr_out = [this](){
			auto const val = env.ASR(gate);
			return val * val;
		}();
		
//		auto const drone = smoothedParams.getNextValue(param_e::DRONE);
		auto const drone = jlimit(0.0f, 1.0f, calcLogModdedVal(_sstate, param_e::DRONE));

		float const vcf_outL = getFilterVal(smoothedParams.getNextValue(param_e::TYPE_L));
		jassert (0.0 < drive);
		
		// still need a good atan approximation
//		auto const drive_compensate = 10.f * nvs::memoryless::atan_aprox(0.1f / math_impl::tanh(drive));
//		auto const drive_compensate = 10.f * std::atan(0.1f / math_impl::tanh(drive));
		auto const drive_compensate = 10.f * std::atan(0.1f / std::tanh(drive));

		auto const outGain = smoothedParams.getNextValue(nvs::param::parameter_e::OUTPUT_GAIN);
		
		outputBuffer.addSample(0, startSample,
							(std::atan(vcf_outL * nvs::memoryless::linterp<float>(asr_out, drone, drone)) * drive_compensate) * MINUS_NINE_DB * outGain);
		
		if (outputBuffer.getNumChannels() > 1)
		{
			float const vcf_outR = getFilterVal(smoothedParams.getNextValue(param_e::TYPE_R));
			outputBuffer.addSample(1, startSample,
							(std::atan(vcf_outR * nvs::memoryless::linterp<float>(asr_out, drone, drone)) * drive_compensate) * MINUS_NINE_DB * outGain);
		}
		
		++startSample;
	}
}

}	// namespace nvs::csg
