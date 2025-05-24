/*
  ==============================================================================

    SynthVoice.h
    Created: 5 Dec 2018 8:47:46pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "params.h"
#include "SynthSound.h"
#include "CSG_dsp.h"
#include "nvs_libraries/include/nvs_LFO.h"
#include "nvs_libraries/include/nvs_filters.h"
#define MINUS_NINE_DB 0.354813389233575

namespace nvs::csg {

class CSGVoice      :       public SynthesiserVoice
{
public:
	explicit CSGVoice(nvs::param::SmoothedParamsManager *smoothedParams);
	bool canPlaySound (SynthesiserSound* sound) override;
	//===========================================================================
	void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	//===========================================================================
	void stopNote (float velocity, bool allowTailOff) override;
	void pitchWheelMoved (int newPitchWheelValue) override;
	void controllerMoved (int controllerNumber, int newControllerValue) override {}
	//===========================================================================
	void setCurrentPlaybackSampleRate(double newSampleRate) override;
	//===========================================================================
	void renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
	//===========================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock);

	void setSVFOversampling(unsigned int oversample_factor) {
		svf.set_oversample(oversample_factor);
	}
private:
	double velocityLevel;
	double frequency {110.0};
	
	nvs::param::SmoothedParamsManager *_smoothedParams;
	
	nvs::csg::CSG unit;
	nvs::lfo::simple_lfo<float> lfo;
	nvs::filters::svf_nl_rk<float> svf;
	nvs::filters::slewlim<float> env;
	
	bool sampleRateValid() const;
	
	float gate;
	float lfo_out;	// currently used to give float*const to CSG::addModSource
	std::vector<float const*> modSources {&lfo_out};
};

}	//	namespace nvs::csg
