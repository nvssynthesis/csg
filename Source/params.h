/*
  ==============================================================================

    params.h
    Created: 26 Nov 2024 2:48:01pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>

namespace nvs {
namespace param {

// for now, this doesn't need to be split up into proper hierarchies.
#define PARAM_GROUP_LIST \
X(MAIN)			\
X(FM)			\
X(PM)			\
X(FILTER)		\
X(LFO)			\
X(ENVELOPE)		\
X(MODULATION)	\
X(OUTPUT)

enum class GroupID_e : size_t {
#define X(sym) sym,
	PARAM_GROUP_LIST
#undef X
	NUM_PARAMS
};

inline juce::String groupToString(GroupID_e group) {
	switch (group)
	{
#define X(sym) case GroupID_e::sym: return #sym;
		PARAM_GROUP_LIST
#undef X
		default: return {};
	}
}
inline juce::String groupToName(GroupID_e group) {
	auto s = groupToString(group);
	s.append(" PARAMETERS", 30);
	return s;
}
inline juce::String groupToID(GroupID_e group) {
	auto s = groupToString(group);
	s.append("_PARAMS", 30);
	return s;
}

#define PARAM_LIST          \
  X(SELF_FM)                \
  X(MEMORY)                 \
  X(FM_SMOOTH)              \
  X(FM_DEGRADE)             \
  X(PM_AMOUNT)              \
  X(PM_TAME)                \
  X(PM_SHAPE)               \
  X(PM_DEGRADE)             \
  X(CUTOFF)                 \
  X(RESONANCE)              \
  X(FILTER_TYPE_L)          \
  X(FILTER_TYPE_R)          \
  X(LFO_RATE)               \
  X(LFO_WAVE)               \
  X(DRONE)                  \
  X(RISE)                   \
  X(FALL)                   \
  X(SELF_FM_MOD)            \
  X(MEMORY_MOD)             \
  X(FM_SMOOTH_MOD)          \
  X(FM_DEGRADE_MOD)         \
  X(PM_AMOUNT_MOD)          \
  X(PM_TAME_MOD)            \
  X(PM_SHAPE_MOD)           \
  X(PM_DEGRADE_MOD)         \
  X(CUTOFF_MOD)             \
  X(RESONANCE_MOD)          \
  X(FILTER_TYPE_L_MOD)      \
  X(FILTER_TYPE_R_MOD)      \
  X(LFO_RATE_MOD)           \
  X(LFO_WAVE_MOD)           \
  X(DRONE_MOD)              \
  X(RISE_MOD)               \
  X(FALL_MOD)               \
  Y(OUTPUT_GAIN)

enum class PID_e : size_t {
#define X(name) name,
#define Y(name) name,
	PARAM_LIST
	NUM_PARAMS
};
#undef X
#undef Y

inline juce::String paramToID(PID_e id) {
	switch (id)
	{
#define X(sym) case PID_e::sym: return #sym;
#define Y(name)
		PARAM_LIST
#undef X
#undef Y
		default: return {};
	}
}

inline juce::String replaceUnderscores(juce::String str) {
	str.replace("_", " ");
	return str;
}

inline juce::String paramToName(PID_e id){
	return replaceUnderscores(paramToID(id));
}

static juce::String makeModID(juce::String paramID){
	paramID += "_MOD";
	return paramID;
}
static juce::String makeModName(juce::String paramName){
	paramName += " MOD";
	return paramName;
}
static juce::String getBaseIDFromModPID(juce::String baseParamID){
	baseParamID.removeCharacters("_MOD");
	return baseParamID;
}
static juce::String getBaseNameFromModName(juce::String baseParamID){
	baseParamID.removeCharacters(" MOD");
	return baseParamID;
}

struct Params {
	Params(juce::AudioProcessor& p)
	:	apvts(p, nullptr, juce::Identifier("PARAMETERS"), createParamLayout())
	{}
	
	
	AudioProcessorValueTreeState apvts;
	
	using AudioParameter = juce::AudioProcessorParameter;
	using APF = juce::AudioParameterFloat;
	using API = juce::AudioParameterInt;
	using String = juce::String;
	using NormRangeF = juce::NormalisableRange<float>;
	using AudioParameterGroup = juce::AudioProcessorParameterGroup;
	using JPID = juce::ParameterID;
	
private:
	static std::unique_ptr<API> makeFilterTypesParameter(juce::String pid_str, juce::String param_str) {
		return std::make_unique<API>( JPID{ pid_str, 1 }, param_str, 0, 3, 0,
									 juce::AudioParameterIntAttributes()
									 .withStringFromValueFunction([](int value, int maximumStringLength) {	// stringFromInt
										 switch (value) {
											 case 0:		return "LP";
											 case 1:		return "BP";
											 case 2:		return "HP";
											 case 3:		return "NP";
											 default:		return "LP";
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
	
	static std::unique_ptr<AudioParameterGroup> makeMainParamsGroup() {
		auto makeAPF = [](PID_e pid, NormRangeF range, float defaultVal) {
			return std::make_unique<APF>(
				JPID{ paramToID(pid), 1 },
				paramToName(pid),
				std::move(range),
				defaultVal,
				juce::AudioParameterFloatAttributes().withStringFromValueFunction([](float value, int maximumStringLength) {
					return juce::String(value, 3);
				}));
		};
		auto makeFrequencyNormRange = [](float f_low=20.f, float f_high=20000.f) -> NormRangeF {
			return NormRangeF(f_low, f_high,
			[](float start, float end, float normalised) {	// convertFrom0To1Func
				return start * std::pow(end / start, normalised);
			},
			[](float start, float end, float value) {		// convertTo0To1Func
				return std::log(value / start) / std::log(end / start);
			});
		};
		auto makeTimingNormRange = []() -> NormRangeF { return NormRangeF(0.01f, 10000.f); };
		auto makeBitsRange = []() -> NormRangeF { return NormRangeF(0.01f, 1024.f,
		[](float start, float end, float normalised) -> float	// convertFrom0To1Func
		{
			return start * std::pow(end / start, normalised);
		},
		[](float start, float end, float value) -> float		// convertTo0To1Func
		{
			return std::log(value / start) / std::log(end / start);
		}
		); };

		std::unique_ptr<AudioParameterGroup> FMParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::FM), 	groupToName(GroupID_e::FM), "|",
												makeAPF(PID_e::SELF_FM, 	NormRangeF{0.0f, 1.0f}, 0.0f),
												makeAPF(PID_e::MEMORY,		NormRangeF{1.f, 32.f}, 1.f),
												makeAPF(PID_e::FM_SMOOTH, 	makeFrequencyNormRange(), 2000.f),
												makeAPF(PID_e::FM_DEGRADE,	makeBitsRange(), 256.f)
																									  );
		std::unique_ptr<AudioParameterGroup> PMParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::PM), 	groupToName(GroupID_e::PM), "|",
												makeAPF(PID_e::PM_AMOUNT, 	NormRangeF(0.f, 8.f), 0.f ),
												makeAPF(PID_e::PM_TAME, 	makeFrequencyNormRange(), 2000.f ),
												makeAPF(PID_e::PM_SHAPE, 	NormRangeF(0.f, 1.f), 0.f ),
												makeAPF(PID_e::PM_DEGRADE, 	makeBitsRange(), 256.f )
																									  );
		std::unique_ptr<AudioParameterGroup> filterParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::FILTER), 	groupToName(GroupID_e::FILTER), "|",
												makeAPF(PID_e::CUTOFF, 		makeFrequencyNormRange(), 12000.f ),
												makeAPF(PID_e::RESONANCE, 	NormRangeF(1.f, 13.f), 1.f ),
												makeFilterTypesParameter(paramToID(PID_e::FILTER_TYPE_L), paramToName(PID_e::FILTER_TYPE_L)),
												makeFilterTypesParameter(paramToID(PID_e::FILTER_TYPE_R), paramToName(PID_e::FILTER_TYPE_R))
																										  );
		std::unique_ptr<AudioParameterGroup> LFOParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::LFO), 	groupToName(GroupID_e::LFO), "|",
												makeAPF(PID_e::LFO_RATE, 	makeFrequencyNormRange(0.023f, 23.f), 1.f ),
												makeAPF(PID_e::LFO_WAVE, 	NormRangeF(0.f, 3.f), 0.f )
																									   );
		std::unique_ptr<AudioParameterGroup> envelopeParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::ENVELOPE), 	groupToName(GroupID_e::ENVELOPE), "|",
												makeAPF(PID_e::DRONE, 		NormRangeF { 0.f, 1.f,
																							[](float start, float end, float normalized) { return std::sqrt(normalized); },
																							[](float start, float end, float value) { return std::pow(value, 2.f); }
																						}, 0.f ),
												makeAPF(PID_e::RISE, 		makeTimingNormRange(), 0.1f ),
												makeAPF(PID_e::FALL, 		makeTimingNormRange(), 0.1f )
																											);

		return std::make_unique<AudioParameterGroup>(
											groupToID(GroupID_e::MAIN), groupToName(GroupID_e::MAIN), "|",
												std::move(FMParameterGroup),
												std::move(PMParameterGroup),
												std::move(filterParameterGroup),
												std::move(LFOParameterGroup),
												std::move(envelopeParameterGroup)
													 );
	}

	static std::unique_ptr<AudioParameterGroup> makeModulationParamsGroup(const AudioParameterGroup& sourceGroup)
	{
		jassert ( sourceGroup.getID().equalsIgnoreCase(groupToID(GroupID_e::MAIN)) );
		
		std::unique_ptr<AudioParameterGroup> result = std::make_unique<AudioParameterGroup>( groupToID(GroupID_e::MODULATION), groupToName(GroupID_e::MODULATION), "|" );
		
		for (auto* param : sourceGroup.getParameters(true)) {
			// skip groups with no need for modulation
			if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID *>(param)){
				auto id = makeModID(paramWithID->getParameterID());
				auto name = makeModName(paramWithID->getName(128));
				
				result->addChild(std::make_unique<APF>(JPID{id, 1},
													   name,
													   juce::NormalisableRange<float>(0.0f, 1.0f),
													   0.0f));
			}
			else {
				std::cerr << "could not cast to parameter with ID\n";
			}
		}
		return result;
	}
	
	static std::unique_ptr<AudioParameterGroup> makeOutputParamsGroup() {
		return std::make_unique<AudioParameterGroup>(
							 groupToID(GroupID_e::OUTPUT), groupToName(GroupID_e::OUTPUT), "|",
							 std::make_unique<APF>(JPID{ paramToID(PID_e::OUTPUT_GAIN), 1 }, paramToName(PID_e::OUTPUT_GAIN),
												   NormRangeF { 0.f, 1.f,
													   [](float start, float end, float normalized) { return std::sqrt(normalized); },
													   [](float start, float end, float value) { return std::pow(value, 2.f); }},
												   0.f));
	}
	
	static juce::AudioProcessorValueTreeState::ParameterLayout createParamLayout() {
		juce::AudioProcessorValueTreeState::ParameterLayout layout;
		std::unique_ptr<AudioParameterGroup> mainParamsTmp = makeMainParamsGroup();
		std::unique_ptr<AudioParameterGroup> modParamsTmp = makeModulationParamsGroup(*mainParamsTmp.get());
		layout.add(std::move(mainParamsTmp));
		layout.add(std::move(modParamsTmp));
		layout.add(makeOutputParamsGroup());
		
		return layout;
	}
};

struct SmoothedParamsManager {
	using PID_e = nvs::param::PID_e;

	void reset (double sampleRate, int samplesPerBlock, juce::AudioProcessorValueTreeState& apvts) {
		auto const rampTime = samplesPerBlock / sampleRate;
		for (size_t i = 0; i < values.size(); ++i) {
			values[i].reset (sampleRate, rampTime);
			juce::String pid_str = nvs::param::paramToID((PID_e)(i));
			values[i].setCurrentAndTargetValue(*apvts.getRawParameterValue(pid_str));
		}
	}

	// call once per block
	void updateTargets (juce::AudioProcessorValueTreeState& apvts) {
		for (size_t i = 0; i < values.size(); ++i){
			juce::String pid_str = nvs::param::paramToID((PID_e)(i));
			values[i].setTargetValue(*apvts.getRawParameterValue (pid_str));
		}
	}

	float getNextValue (PID_e id) noexcept {
		return values[(size_t)id].getNextValue();
	}
private:
	std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, (size_t)PID_e::NUM_PARAMS> values;
};

}	// namespace param
}	// namespace nvs
