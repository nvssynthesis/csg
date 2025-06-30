/*
  ==============================================================================

    CSG_dsp.cpp
    Created: 5 Dec 2018 8:48:55pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#include "CSG_dsp.h"

namespace nvs::csg {

CSG::CSG(SharedState *s)
:	_phase(0.f)
,	_sstate(s)
,	zLine(1, 32)
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

void CSG::setFrequencyMultiplier(double freqMult){
	jassert(0.0 < freqMult);
	_freq_multiplier = freqMult;
}

void CSG::clearDelay()
{
	zLength = zLine.getNumSamples() - 1;
	zLine.clear();
	rp = 0;
	rp_neighbor = zLength;
	wp = zLength;
}

float CSG::phasor()
{
	using namespace nvs::memoryless;
	_phase +=_freq_multiplier *  base_freq * fs_delta;
	_phase = mspWrap(_phase);
	return _phase;
}
float CSG::phasor_fm(float sample)
{
	using namespace nvs::memoryless;
	_phase += _freq_multiplier * (base_freq + sample) * fs_delta;
	_phase = mspWrap(_phase);
	return _phase;
}


float CSG::getWave()
{
	jassert (sampleRateValid());
	jassert (_sstate != nullptr);
	// called per sample
	using namespace nvs::memoryless;
	using namespace nvs::param;
	{
		// if this ever fails, then the 'while' protective mechanism becomes too far from constant time
		jassert (-1000 < rp);
		jassert (rp < zLength + 1000);
	}
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
	int const feedback_algo = _sstate->smoothedParamsManager.getNextValue(parameter_e::FEEDBACK_CIRCUIT);
	
	float const fedback_delta =
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

	float const feedback_amt = clamp<float>(calcLinearModdedVal(*_sstate, parameter_e::SELF_FM), 0.0f, 1.0f) * 24000.0f; // is the clamp really essential?
	float const feedback_cutoff = clamp<float>( calcLogModdedVal(*_sstate, parameter_e::FM_SMOOTH), 1.f, getSampleRate() / 2 - 50.f);
	
	FM_filter.setCutoff(feedback_cutoff); // not smoothed currently
	
	float const feedback = FM_filter(fedback_delta * (fs * 0.000021f) * feedback_amt);
	
	// bitcrush that fed-back signal.
	auto const fm_degrade = clamp<float>(calcLinearModdedVal(*_sstate, parameter_e::FM_DEGRADE), 1.01f, 2048.f);
	base_freq = calcLogModdedVal(*_sstate, parameter_e::PITCH);
	float const crushed_freqmod_sig = crush<float>(phasor_fm(feedback), fm_degrade);
	
	// on other hand, take sin and cos of phasor to do phase modulation (what will basically sound like wavefolding).
	// we can fade between the sin and cos parts with equal power.
	float const pmPreampTmp = calcLinearModdedVal(*_sstate, parameter_e::PM_AMOUNT);
	auto const pmShapeTmp = clamp(calcLinearModdedVal(*_sstate, parameter_e::PM_SHAPE), 0.f, 1.f);
	

	// now, bitcrush and filter that signal.
	float const pmSmoothTmp = clamp<float>( calcLogModdedVal(*_sstate, parameter_e::PM_TAME), 1.f, getSampleRate() / 2 - 50.f);
	PM_filter.setCutoff(pmSmoothTmp);

	double constexpr twopi = nvs::memoryless::math_impl::two_pi<double>();
	
	auto const sinx = cosTable.sin(_phase * twopi);
	auto const cosx = cosTable.cos(_phase * twopi * 2.f - 1.f);
	float const weighted_sincos = (sinx * sqrt(1.f - (pmShapeTmp)) + (cosx * sqrt(pmShapeTmp))) * pmPreampTmp;
	
	auto const pm_degrade = calcLinearModdedVal(*_sstate, parameter_e::PM_DEGRADE);
	float const sinwin = PM_filter(crush<float>(weighted_sincos, clamp<float>(pm_degrade, 1.01f, 2048.f) ));

	// sum the FM part and PM part. Make them both bipolar.
	float const junction = unibi<float>(crushed_freqmod_sig) + sinwin;
	// 2 samples ago, used for derivative.
	//z2 = output;
	
	//increment delay line============================================================================
	float const mem_tmp = [mem_raw_val = calcLinearModdedVal(*_sstate, parameter_e::MEMORY), L = float(this->zLength)](){
		auto val = scale(mem_raw_val, 1.f, L);
		val = mspWrap(val);
		return scale(val, 0.f, 1.f, 1.f, L);
	}();
	wp -= 1;
	wp = fmodf(wp, mem_tmp);
	rp = wp + (int)mem_tmp;
	rp = fmodf(rp, mem_tmp);
	rp_neighbor = rp + 1;
	rp_neighbor = fmodf(rp_neighbor, mem_tmp);
	//end increment delay line========================================================================
	
	if (wp < 0)
		wp = zLength;
	if (wp > zLength)
		wp = 0;
	
	// now take the sin of the summed signals! this is the CSG.
	float const output = nvs::memoryless::padeSin(junction * M_PI);
	
	zLine.setSample(0, wp, output);
	
	return output;
}
}	// namespace csg
