/*
  ==============================================================================

    CSG_dsp.cpp
    Created: 5 Dec 2018 8:48:55pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "CSG_dsp.h"
namespace nvs::csg{


CSG::CSG(nvs::param::SmoothedParamsManager *smoothedParams)
:  _freq(110.f), _phase(0.f), _smoothedParams(smoothedParams), memory(2.f), bits1(2048.f), bits2(2048.f), zLine(1, 32)
{
	clearDelay();
}

void CSG::setSampleRate(double sample_rate)
{
	fs = sample_rate;
	fs_delta = 1.f / sample_rate;
	FM_filter.setSampleRate(sample_rate);
	PM_filter.setSampleRate(sample_rate);
	dc_b.setSampleRate(sample_rate);
}
double CSG::getSampleRate() const {
	return fs;
}
bool CSG::sampleRateValid() const {
	auto almost_equal = [](auto f1, auto f2) -> bool {
		return std::abs(f1 - f2) < 0.0001f;
	};
	return almost_equal(fs, 1.f/fs_delta) == almost_equal(FM_filter.getSampleRate(), PM_filter.getSampleRate()) == almost_equal(dc_b.getSampleRate(), fs) == true;
}

void CSG::setFrequency(float newFreq){
	assert(newFreq < (fs * 0.5) && newFreq > 0.0);
	_freq = newFreq;
}

void CSG::clearDelay()
{
	//zLength = (sizeof(zLine)/sizeof(*zLine));
	zLength = zLine.getNumSamples() - 1;
	zLine.clear();
	rp = 0;
	rp_neighbor = zLength;
	wp = zLength;
}

float CSG::phasor()
{
	using namespace nvs::memoryless;
	_phase += _freq * fs_delta;
	_phase = mod_1<double>(_phase);
	return _phase;
}
float CSG::phasor_fm(float sample)
{
	using namespace nvs::memoryless;
	_phase += (_freq + sample) * fs_delta;
	_phase = mod_1<double>(_phase);
	return _phase;
}



float CSG::getWave()
{
	jassert (sampleRateValid());
	// called per sample
	using namespace nvs::memoryless;
	using namespace nvs::param;
	//update params===================================================================================
	selfFM      = _smoothedParams->getNextValue(PID_e::SELF_FM);
	memory      = _smoothedParams->getNextValue(PID_e::MEMORY);
	FM_smooth   = _smoothedParams->getNextValue(PID_e::FM_SMOOTH);
	bits1       = _smoothedParams->getNextValue(PID_e::FM_DEGRADE);
	PM_preamp   = _smoothedParams->getNextValue(PID_e::PM_AMOUNT);
	PM_smooth   = _smoothedParams->getNextValue(PID_e::PM_TAME);
	PM_sin2cos  = _smoothedParams->getNextValue(PID_e::PM_SHAPE);
	bits2       = _smoothedParams->getNextValue(PID_e::PM_DEGRADE);
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
	// feedback = FM_filter.tpt_lp((output - z2) * (fs * 0.000021) * pow((clamp<float>(selfFM + _selfFM_MOD, 0.f, 1.f) * 29), 3.f), clamp<float>(FM_smooth + (FM_smooth * _FM_smooth_MOD * 3.f), 20.f, 20000.f));
#pragma warning("dc_b is used twice, which does not make sense for an iir filter because the state between calls will be changed")
	int feedback_algo = 0;
	
	float fedback_delta =
	[&](int algo){
		switch (feedback_algo) {
			case 0:
				return dc_b.filter(zLine.getSample(0, rp));
			case 1:
				return dc_b.filter(zLine.getSample(0, rp) - zLine.getSample(0, rp_neighbor));
			case 2:
				return dc_b.filter(dc_b.filter(zLine.getSample(0, rp)));
			case 3:
				return dc_b.filter(dc_b.filter(zLine.getSample(0, rp) - zLine.getSample(0, rp_neighbor)));
			default:
				return 0.f;
		}
	}(feedback_algo);

//	float feedback_amt = clamp<float>(selfFM + _selfFM_MOD, 0.0f, 1.f) * 24000.f;
	float feedback_amt = clamp<float>(calcLinearModdedVal(*_smoothedParams, PID_e::SELF_FM, modSources), 0.0f, 1.0f) * 24000.0f;	// is the clamp really essential?
	float feedback_cutoff = clamp<float>( calcLogModdedVal(*_smoothedParams, PID_e::FM_SMOOTH, modSources), 1.f, getSampleRate() / 2 - 50.f);
	
	FM_filter.setCutoff(feedback_cutoff); // not smoothed currently
	
	float feedback = FM_filter(fedback_delta * (fs * 0.000021f) * feedback_amt);
	
	// bitcrush that fed-back signal.
//	auto const bits_a = clamp<float>(bits1 + (_bits_A_MOD * bits1), 1.01f, 2048.f);
	auto const fm_degrade = clamp<float>(calcLinearModdedVal(*_smoothedParams, PID_e::FM_DEGRADE, modSources), 1.01f, 2048.f);
	float crushed_freqmod_sig = crush<float>(phasor_fm(feedback), fm_degrade);
	
	// on other hand, take sin and cos of phasor to do phase modulation (what will basically sound like wavefolding).
	// we can fade between the sin and cos parts with equal power.
	float const pmPreampTmp = calcLinearModdedVal(*_smoothedParams, PID_e::PM_AMOUNT, modSources);
	auto const pmShapeTmp = calcLinearModdedVal(*_smoothedParams, PID_e::PM_SHAPE, modSources);
	
	double constexpr twopi = nvs::memoryless::math_impl::two_pi<double>();
	
	auto const sinx = cosTable.sin(_phase * twopi);
	auto const cosx = cosTable.cos(_phase * twopi * 2.f - 1.f);
	float weighted_sincos = (sinx * sqrt(1.f - (pmShapeTmp)) + (cosx * sqrt(pmShapeTmp))) * pmPreampTmp;
	
	// now, bitcrush and filter that signal.
	float const pmSmoothTmp = clamp<float>( calcLogModdedVal(*_smoothedParams, PID_e::PM_TAME, modSources), 1.f, getSampleRate() / 2 - 50.f);
	PM_filter.setCutoff(pmSmoothTmp);
	
	auto const pm_degrade = calcLinearModdedVal(*_smoothedParams, PID_e::PM_DEGRADE, modSources);
//	float sinwin = PM_filter(crush<float>(weighted_sincos, clamp<float>(bits2 + (_bits_B_MOD * bits2), 1.01f, 2048.f) ));
	float sinwin = PM_filter(crush<float>(weighted_sincos, clamp<float>(pm_degrade, 1.01f, 2048.f) ));

	// sum the FM part and PM part. Make them both bipolar.
	float junction = unibi<float>(crushed_freqmod_sig) + sinwin;
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
	float output = nvs::memoryless::padeSin(junction * M_PI);
	
	zLine.setSample(0, wp, output);
	
	return output;
}
}	// namespace csg
