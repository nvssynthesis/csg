/*
==============================================================================

SynthVoice.cpp
Created: 26 Nov 2024 1:25:49pm
Author:  Nicholas Solem

==============================================================================
*/

#include "SynthVoice.h"

namespace nvs::csg {

CSGVoice::CSGVoice(nvs::param::SmoothedParamsManager *smoothedParams)
:	_smoothedParams(smoothedParams),
	unit(_smoothedParams)
{
	svf.set_oversample(4);
	unit.addModSource(&lfo_out);
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
	frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
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
}
//===========================================================================
void CSGVoice::setCurrentPlaybackSampleRate(double sampleRate) {
	unit.setSampleRate(sampleRate);
	env.setSampleRate(sampleRate);
	svf.setSampleRate(sampleRate);
	svf.clear();
	//	env.setRise(0.01f);		// why needed? if needed then get rid of this necessity
	//	env.setFall(0.01f);
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
	_smoothedParams->reset(sampleRate, samplesPerBlock);
}

unsigned int powi2(int exp) {
	// integer power of 2
	if (exp < 0) {
	  return 0;
	}
	return 1 << exp;
}

void CSGVoice::renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
	jassert (sampleRateValid());
	using PID_e = nvs::param::PID_e;

	unsigned int const oversampleFactor = powi2(_smoothedParams->_apvts.getRawParameterValue(param::paramToID( PID_e::OVERSAMPLE_FACTOR ))->load());
	setSVFOversampling(oversampleFactor);
	
	env.setBlockSize(numSamples);
	svf.setBlockSize(numSamples);
	
	_smoothedParams->updateTargets();

	// was in sample for loop
	unit.setFrequency(frequency);
	
	jassert(_smoothedParams);
	
	for (int sample = 0; sample < numSamples; ++sample)
	{
		lfo._freq = _smoothedParams->getNextValue(PID_e::LFO_RATE);
		lfo.phasor();   // increment internal phase of LFO
		lfo_out = lfo.multi(_smoothedParams->getNextValue(PID_e::LFO_WAVE));
		
//		unit._selfFM_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::SELF_FM_MOD);
//		unit._FM_smooth_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::FM_SMOOTH_MOD);
//		unit._bits_A_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::FM_DEGRADE_MOD);
//
//
//		unit._PM_preamp_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::PM_AMOUNT_MOD);
//		unit._PM_smooth_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::PM_TAME_MOD);
//
//		unit._bits_B_MOD = ((lfo_out * 2 - 1) * 256.f) * _smoothedParams->getNextValue(PID_e::PM_DEGRADE_MOD);
//
//		unit._PM_sin2cos_MOD = lfo_out * _smoothedParams->getNextValue(PID_e::PM_SHAPE_MOD);
		
		float const finalCutoff = nvs::memoryless::clamp<float>( calcLogModdedVal(*_smoothedParams, PID_e::CUTOFF, modSources), 1.f, (float)getSampleRate() / 2.f - 50.f);
		svf.setCutoff(finalCutoff);
		
		auto const res = jlimit(0.0f, 6.0f, 5.0f * lfo_out * _smoothedParams->getNextValue(PID_e::RESO_MOD) + _smoothedParams->getNextValue(PID_e::RESO));
		svf.setResonance(res);
		
		env.setRise(_smoothedParams->getNextValue(PID_e::RISE));
		env.setFall(_smoothedParams->getNextValue(PID_e::FALL));

		auto const csg_wave = unit.getWave();
		
		auto const drive = _smoothedParams->getNextValue(PID_e::DRIVE);
		svf.filter(csg_wave * drive);
		
		auto getFilterVal = [&](float filterSelection){
			switch ((int)filterSelection) {
				case 0: { return svf.lp(); }
				case 1: { return svf.bp(); }
				case 2: { return svf.hp(); }
				case 3: { return svf.np(); }
				default:{ return svf.lp(); }
			}
		};
		
		float env_currentVal = env.ASR(gate);
		env_currentVal *= env_currentVal;
		
		auto drone = _smoothedParams->getNextValue(PID_e::DRONE);
		
		float vcf_outL = getFilterVal(_smoothedParams->getNextValue(PID_e::TYPE_L));
		jassert (0.0 < drive);
		auto drive_compensate = 10.f * atan(0.1f / tanh(drive));

		
		auto const outGain = _smoothedParams->getNextValue(nvs::param::PID_e::OUTPUT_GAIN);
		
		outputBuffer.addSample(0, startSample,
							(atanf(vcf_outL * nvs::memoryless::linterp<float>(env_currentVal, drone, drone)) * drive_compensate) * MINUS_NINE_DB * outGain);
		
		if (outputBuffer.getNumChannels() > 1)
		{
			float vcf_outR = getFilterVal(_smoothedParams->getNextValue(PID_e::TYPE_R));
			outputBuffer.addSample(1, startSample,
							(atanf(vcf_outR * nvs::memoryless::linterp<float>(env_currentVal, drone, drone)) * drive_compensate) * MINUS_NINE_DB * outGain);
		}
		
		++startSample;
	}
}

}	// namespace nvs::csg
