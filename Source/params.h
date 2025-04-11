/*
  ==============================================================================

    params.h
    Created: 26 Nov 2024 2:48:01pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace nvs {

struct Params {
	using AudioParameter = juce::AudioProcessorParameter;
	using APF = juce::AudioParameterFloat;
	using API = juce::AudioParameterInt;
	using String = juce::String;
	using NormRangeF = juce::NormalisableRange<float>;
	using AudioParameterGroup = juce::AudioProcessorParameterGroup;
	using PID = juce::ParameterID;
	
	
private:
	static NormRangeF makeFrequencyNormRange(float f_low=20.f, float f_high=20000.f) {
		return NormRangeF(f_low, f_high,
			[](float start, float end, float normalised) {	// convertFrom0To1Func
			  return start * std::pow(end / start, normalised);
			},
			[](float start, float end, float value) {		// convertTo0To1Func
			  return std::log(value / start) / std::log(end / start);
		});
	}
	static NormRangeF makeTimingNormRange() {
		return NormRangeF(0.01f, 10000.f);
	}
	
	std::unique_ptr<API> makeFilterTypesParameter(juce::String channel_str) {
		// channel_str should simply be either "R" or "L".
		auto pid_str = juce::String("FILTER_TYPE_");
		pid_str.append(channel_str, channel_str.length());
		auto param_str = juce::String("FILTER TYPE ");
		param_str.append(channel_str, channel_str.length());
		
		return std::make_unique<API>( PID{ pid_str, 1 }, param_str, 0, 3, 0,
							 juce::AudioParameterIntAttributes()
									 .withStringFromValueFunction([](int value, int maximumStringLength) {	// stringFromInt
																	   switch (value) {
																		   case 0:		return "LP";
																		   case 1:		return "BP";
																		   case 2:		return "HP";
																		   case 3:		return "NP";
																		   default:	return "LP";
																	   }
																   })
									 .withValueFromStringFunction([](const String &text) { 	// intFromString
																	   if (text.isEmpty())      return 0;
																	   auto c = text[0];
																	   switch (c)
																	   {
																		   case 'L': case 'l':   return 0;  // Lowpass
																		   case 'B': case 'b':   return 1;  // Bandpass
																		   case 'H': case 'h':   return 2;  // Highpass
																		   case 'N': case 'n':   return 3;  // Notch
																		   default:              return 0;
																	   }
																   })
									 .withAutomatable(true)
								);
	}
	
	AudioParameterGroup mainParamsGroup {
		"MAIN_PARAMS", "MAIN PARAMETERS", "|",
		std::make_unique<APF>( PID{ "SELF_FM", 1 }, 	"SELF-FM", 		NormRangeF(0.0f, 1.0f),  0.0f),
		std::make_unique<APF>( PID{ "MEMORY", 1 }, 		"MEMORY", 		NormRangeF(1.f, 32.f), 1.f ),	// minimum feedback delay = 1 sample
		std::make_unique<APF>( PID{ "FM_SMOOTH", 1 }, 	"FM SMOOTH", 	makeFrequencyNormRange(), 2000.f ),
		std::make_unique<APF>( PID{ "FM_DEGRADE", 1 }, 	"FM DEGRADE",	NormRangeF(0.f, 1024.f), 256.f ),
		std::make_unique<APF>( PID{ "PM_AMOUNT", 1 }, 	"PM AMOUNT", 	NormRangeF(0.f, 8.f), 0.f ),
		std::make_unique<APF>( PID{ "PM_TAME", 1 }, 	"PM TAME", 		makeFrequencyNormRange(), 2000.f ),
		std::make_unique<APF>( PID{ "PM_SHAPE", 1 }, 	"PM SHAPE", 	NormRangeF(0.f, 1.f), 0.f ),
		std::make_unique<APF>( PID{ "PM_DEGRADE", 1 }, 	"PM DEGRADE", 	NormRangeF(0.f, 1024.f), 256.f ),
		std::make_unique<APF>( PID{ "CUTOFF", 1 }, 		"CUTOFF", 		makeFrequencyNormRange(), 12000.f ),
		std::make_unique<APF>( PID{ "RES", 1 }, 		"RES", 			NormRangeF(1.f, 13.f), 1.f ),
		std::make_unique<APF>( PID{ "LFO_RATE", 1 }, 	"LFO RATE", 	makeFrequencyNormRange(0.023f, 23.f), 1.f ),
		std::make_unique<APF>( PID{ "LFO_WAVE", 1 }, 	"LFO WAVE", 	NormRangeF(0.f, 3.f), 0.f ),
		std::make_unique<APF>( PID{ "DRONE", 1 }, 		"DRONE",
								NormRangeF { 0.f, 1.f,
									[](float start, float end, float normalized) { return std::sqrt(normalized); },
									[](float start, float end, float value) { return std::pow(value, 2.f); }
								}, 0.f ),
		std::make_unique<APF>( PID{ "RISE", 1 }, 		"RISE", 		makeTimingNormRange(), 0.1f ),
		std::make_unique<APF>( PID{ "FALL", 1 }, 		"FALL", 		makeTimingNormRange(), 0.1f ),
		makeFilterTypesParameter("L"),
		makeFilterTypesParameter("R")
	};
	static AudioParameterGroup makeModulationParams(const AudioParameterGroup& sourceGroup)
	{
		AudioParameterGroup result("MODULATION_PARAMS", "MODULATION PARAMETERS", "|");

		for (auto* param : sourceGroup.getParameters(false)) {
			if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID *>(param)){
				auto id = paramWithID->getParameterID() + "_MOD";
				auto name = paramWithID->getName(128) + " MOD";

				result.addChild(std::make_unique<APF>(
					PID{id, 1},
					name,
					juce::NormalisableRange<float>(0.0f, 1.0f),
					0.0f
				));
			}
			else {
				std::cerr << "could not cast to parameter with ID\n";
			}
		}

		return result;
	}
	AudioParameterGroup modulationParamsGroup {makeModulationParams(mainParamsGroup)};
	AudioParameterGroup outputParamsGroup {
		"OUTPUT_PARAMS", "OUTPUT PARAMETERS", "|",
		std::make_unique<APF>( PID{ "OUTPUT_GAIN", 1 }, "OUTPUT GAIN",
								NormRangeF { 0.f, 1.f,
									[](float start, float end, float normalized) { return std::sqrt(normalized); },
									[](float start, float end, float value) { return std::pow(value, 2.f); }
						  }, 0.f )
	};
};

}	// namespace nvs
