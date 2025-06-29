/*
  ==============================================================================

    Modulation.h
    Created: 29 Jun 2025 1:54:17pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once

namespace nvs {
namespace modulation {

enum class Polarity_e {
	Bipolar = 0,
	Unipolar
};
enum class Source_e {
	LFO = 0,
	ASR,
	Mixer1,
	Mixer2
};

}	// namespace modulation
}	// namespace nvs
