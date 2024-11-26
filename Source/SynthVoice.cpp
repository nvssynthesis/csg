/*
==============================================================================

SynthVoice.cpp
Created: 26 Nov 2024 1:25:49pm
Author:  Nicholas Solem

==============================================================================
*/

#include "SynthVoice.h"


CSGVoice::CSGVoice()
: oversample_factor(4)
{
	svf.set_oversample(oversample_factor);
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
	// FOR PRO VERSION: MAKE SCALABLE TUNING, MAYBE ALLOW .TUN FILES
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
	if (velocity == 0)
		clearCurrentNote();
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
void CSGVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
}
void CSGVoice::setCurrentPlaybackSampleRate(double newSampleRate) {
	
	env.setSampleRate(newSampleRate);
}
//===========================================================================
void CSGVoice::renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
	unit.setCurrentBlockSize(numSamples);
	env.setBlockSize(numSamples);
	svf.setBlockSize(numSamples);
	oneOverBlockSize = 1.f / static_cast<float>(numSamples);
	// was in sample for loop
	//unit._freq = frequency / oversample_factor;   // 2X oversampling
	unit.setFrequency(frequency);
	
	// later 'advanced' version will have another CSG as lfo!
	if (last_lfo_freq != lfo._freq)
		lfo._freq = last_lfo_freq;
	
//        if (last_lfo_wave != lfo._wave)
//            lfo._wave = last_lfo_wave;
	
//		svf.setCutoffTarget(voice_cutoff);
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

	cutoff_smoothed_.reset(numSamples);
	cutoff_smoothed_.setTargetValue(voice_cutoff);
	for (int sample = 0; sample < numSamples; ++sample)
	{
		svf.updateCutoff();
		svf.setCutoff(cutoff_smoothed_.getNextValue());
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
