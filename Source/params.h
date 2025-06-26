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

using AudioParameter = juce::AudioProcessorParameter;
using APF = juce::AudioParameterFloat;
using API = juce::AudioParameterInt;
using ChoiceParam = juce::AudioParameterChoice;
using String = juce::String;
using NormRangeF = juce::NormalisableRange<float>;
using AudioParameterGroup = juce::AudioProcessorParameterGroup;
using JPID = juce::ParameterID;

// for now, this doesn't need to be split up into proper hierarchies.
#define PARAM_GROUP_LIST \
X(MAIN)			\
X(FM)			\
X(PM)			\
X(FILTER)		\
X(LFO)			\
X(ENVELOPE)		\
X(MODULATION)	\
X(UTILITY)

enum class GroupID_e : size_t {
#define X(sym) sym,
	PARAM_GROUP_LIST
#undef X
	NUM_PARAMS
};

inline String groupToString(GroupID_e group) {
	switch (group)
	{
#define X(sym) case GroupID_e::sym: return #sym;
		PARAM_GROUP_LIST
#undef X
		default: return {};
	}
}
inline String groupToName(GroupID_e group) {
	auto s = groupToString(group);
	s.append(" PARAMETERS", 30);
	return s;
}
inline String groupToID(GroupID_e group) {
	auto s = groupToString(group);
	s.append("_PARAMS", 30);
	return s;
}

#define PARAM_LIST          \
  X(PITCH)					\
  X(SELF_FM)                \
  X(MEMORY)                 \
  X(FM_SMOOTH)              \
  X(FM_DEGRADE)             \
  X(PM_AMOUNT)              \
  X(PM_TAME)                \
  X(PM_SHAPE)               \
  X(PM_DEGRADE)             \
  X(DRIVE)					\
  X(CUTOFF)                 \
  X(RESO)              		\
  X(TYPE_L)  		        \
  X(TYPE_R) 		        \
  X(LFO_RATE)               \
  X(LFO_WAVE)               \
  X(DRONE)                  \
  X(RISE)                   \
  X(FALL)                   \
  X(PITCH_MOD)				\
  X(SELF_FM_MOD)            \
  X(MEMORY_MOD)             \
  X(FM_SMOOTH_MOD)          \
  X(FM_DEGRADE_MOD)         \
  X(PM_AMOUNT_MOD)          \
  X(PM_TAME_MOD)            \
  X(PM_SHAPE_MOD)           \
  X(PM_DEGRADE_MOD)         \
  X(DRIVE_MOD)				\
  X(CUTOFF_MOD)             \
  X(RESO_MOD)          		\
  X(TYPE_L_MOD)      		\
  X(TYPE_R_MOD)      		\
  X(LFO_RATE_MOD)           \
  X(LFO_WAVE_MOD)           \
  X(DRONE_MOD)              \
  X(RISE_MOD)               \
  X(FALL_MOD)               \
  Y(OVERSAMPLE_FACTOR)		\
  Y(OUTPUT_GAIN)

enum class PID_e : size_t {
#define X(name) name,
#define Y(name) name,
	PARAM_LIST
	NUM_PARAMS
};
#undef X
#undef Y

inline String paramToID(PID_e id) {
	switch (id)
	{
#define X(sym) case PID_e::sym: return #sym;
#define Y(sym) case PID_e::sym: return #sym;
		PARAM_LIST
#undef X
#undef Y
		default: return {};
	}
}


inline String replaceUnderscores(String str) {
	str = str.replace("_", " ");
	return str;
}

inline String paramToName(PID_e id){
	return replaceUnderscores(paramToID(id));
}

inline String makeModID(String paramID){
	paramID += "_MOD";
	return paramID;
}
inline String makeModName(String paramName){
	paramName += " MOD";
	return paramName;
}

inline constexpr PID_e basePIDToModPID(PID_e id) {
	return PID_e(size_t(id) + 19);
}
static_assert(basePIDToModPID(PID_e::PITCH) == PID_e::PITCH_MOD);
static_assert(basePIDToModPID(PID_e::SELF_FM) == PID_e::SELF_FM_MOD);
static_assert(basePIDToModPID(PID_e::PM_TAME) == PID_e::PM_TAME_MOD);
static_assert(basePIDToModPID(PID_e::CUTOFF) == PID_e::CUTOFF_MOD);


inline String getBaseIDFromModPID(String baseParamID){
	baseParamID.removeCharacters("_MOD");
	return baseParamID;
}
inline String getBaseNameFromModName(String baseParamID){
	baseParamID.removeCharacters(" MOD");
	return baseParamID;
}
//=============================================================================================================================
auto makeAPF = [](PID_e pid, NormRangeF range, float defaultVal, int numDecimalPlaces = 2) {
	return std::make_unique<APF>(
		JPID{ paramToID(pid), 1 },
		paramToName(pid),
		std::move(range),
		defaultVal,
		juce::AudioParameterFloatAttributes().withStringFromValueFunction([=](float value, int maximumStringLength) {
			return juce::String(value, numDecimalPlaces, false);
		}));
};
auto makeAPF2 = [](PID_e pid, NormRangeF range, float defaultVal,
				   std::function< String(float value, int maximumStringLength)> stringFromValue=nullptr,
				   std::function< float(const String &text)> valueFromString=nullptr) {
	return std::make_unique<APF>(
		JPID{ paramToID(pid), 1 },
		paramToName(pid),
		std::move(range),
		defaultVal,
		paramToID(pid),	// label
		AudioProcessorParameter::genericParameter,
		stringFromValue,
		valueFromString);
};
//=============================================================================================================================
auto makeSelfFMRange = []() -> NormRangeF {
	auto range = NormRangeF{0.0f, 1.0f};
	range.setSkewForCentre(0.01f);
	return range;
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
auto makeTimingNormRange = []() -> NormRangeF {
	auto range = NormRangeF(0.01f, 10000.f);
	range.setSkewForCentre(100.0f);
	return range;
};
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
auto makeGainRange = [](float minDb, float maxDb) -> NormRangeF
{
	return NormRangeF(juce::Decibels::decibelsToGain(minDb), juce::Decibels::decibelsToGain(maxDb),
	[=] (float, float, float norm) -> float {	// normal to gain
	// norm [0,1] → dB [minDb,maxDb] → linear gain
		float db = juce::jmap (norm, 0.0f, 1.0f, minDb, maxDb);
		return juce::Decibels::decibelsToGain (db);
	},
	[=] (float, float, float gain) -> float {
	// linear gain → dB → norm [0,1]
		float db = juce::Decibels::gainToDecibels (gain);
		return juce::jmap (db, minDb, maxDb, 0.0f, 1.0f);
	});
};
//=============================================================================================================================
auto valueToString_dB = [] (float v, int /*precision*/)    {
	return juce::String (juce::Decibels::gainToDecibels (v), 1) + " dB";
};
auto dB_stringToValue = [] (const juce::String& text) {
	return juce::Decibels::decibelsToGain (text.upToFirstOccurrenceOf (" ", false, false).getFloatValue());
};
auto bitsValueToString = [](float actualValue, int) -> String
{
	// stringFromValue: show log2(actual) as an integer " N bits "
	float bits = std::log2(actualValue);
	return String (bits, 2) + " bits";
};
auto bitsStringToValue = [](const String& text) -> float
{
	// valueFromText: user types “8” (or “8 bits”) -> return 2^8 = 256.0f
	float bits = text.getFloatValue();
	// jlimit?
	return std::exp2(bits);
};
//=============================================================================================================================
const juce::StringArray oversampleLabels { "1x", "2x", "4x", "8x", "16x", "32x", "64x", "128x"};
//=============================================================================================================================
struct Params {
	Params(juce::AudioProcessor& p)
	:	apvts(p, nullptr, juce::Identifier("PARAMETERS"), createParamLayout())
	{}
	
	
	AudioProcessorValueTreeState apvts;
	
private:
	static std::unique_ptr<API> makeFilterTypesParameter(PID_e pid) {
		return std::make_unique<API>( JPID{ paramToID(pid), 1 }, paramToName(pid), 0, 3, 0,
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
	static std::unique_ptr<APF> makeFrequencyParameter(PID_e pid, float min_freq=20.f, float max_freq=22000.f, float default_freq=1000.f){
		return std::make_unique<APF>( JPID{ paramToID(pid), 1 }, paramToName(pid), makeFrequencyNormRange(min_freq, max_freq), default_freq,
		juce::AudioParameterFloatAttributes()
			.withStringFromValueFunction([](float value, int maximumStringLength) -> juce::String {	// stringFromFloat
				if (value >= 1000.f){
					auto s = juce::String(value / 1000.f, 1);
					s.append(" kHz", 4);
					return s;
				}
				auto s = juce::String(value, 1);
				s.append(" Hz", 3);
				return s;
			})
			.withValueFromStringFunction([](const String &text) -> float 	// floatFromString
			{
				return text.getFloatValue();
			}));
	}
	static std::unique_ptr<APF> makeTimingParameter(PID_e pid, float default_ms){
		return std::make_unique<APF>( JPID{ paramToID(pid), 1 }, paramToName(pid), makeTimingNormRange(), default_ms,
		juce::AudioParameterFloatAttributes()
			.withStringFromValueFunction([](float value, int maximumStringLength) -> juce::String {	// stringFromFloat
				if (value >= 1000.f){
					auto s = juce::String(value / 1000.f, 2);
					s.append(" s", 4);
					return s;
				}
				int numDec = value <= 0.01 ? 4 : value <= 0.1 ? 3 : 2;
				auto s = juce::String(value, numDec);
				s.append(" ms", 3);
				return s;
			})
			.withValueFromStringFunction([](const String &text) -> float 	// floatFromString
			{
				return text.getFloatValue();
			}));
	}
	static std::unique_ptr<APF> makeLFOWaveParameter(PID_e pid) {
//		(const ParameterID &parameterID, const String &parameterName, NormalisableRange< float > normalisableRange, float defaultValue, const AudioParameterFloatAttributes &attributes={})
		return std::make_unique<APF>( JPID{ paramToID(pid), 1 }, paramToName(pid), NormRangeF(0.f, 3.f), 0.f,
									 juce::AudioParameterFloatAttributes()
									 .withStringFromValueFunction([](float value, int maximumStringLength) -> juce::String {	// stringFromFloat
										 float thresh = 0.05;
										 if (value < thresh)					{ return "Sine";		}
										 else if (value < (1.0 - thresh)) 		{ return "Sin/Saw"; 	}
										 else if (value < (1.0 + thresh)) 		{ return "Saw"; 		}
										 else if (value < (2.0 - thresh)) 		{ return "Saw/Square"; 	}
										 else if (value < (2.0 + thresh)) 		{ return "Square"; 		}
										 else if (value < (3.0 - thresh))		{ return "Square/Tri"; 	}
										 else if (value < (3.0 + thresh))		{ return "Triangle"; 	}
										 return juce::String(value);
									 })
									 .withValueFromStringFunction([](const String &text) -> float 	// floatFromString
									 {
										 if (text.isEmpty())      return 0.f;
										 else if (text.startsWithIgnoreCase("si")) { return 0.f; }
										 else if (text.startsWithIgnoreCase("sa")) { return 1.f; }
										 else if (text.startsWithIgnoreCase("sq")) { return 2.f; }
										 else if (text.startsWithIgnoreCase("tr")) { return 3.f; }
										 else 	 {return text.getFloatValue();}
									 })
									 .withAutomatable(true)
									 );
	}
	
	static std::unique_ptr<AudioParameterGroup> makeMainParamsGroup() {
		std::unique_ptr<AudioParameterGroup> FMParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::FM), 	groupToName(GroupID_e::FM), "|",
												makeFrequencyParameter(PID_e::PITCH, 13.75, 880.0, 110.0),
												makeAPF(PID_e::SELF_FM, 	makeSelfFMRange(), 0.0f, 5),
												makeAPF(PID_e::MEMORY,		NormRangeF{1.f, 32.f}, 1.f),
												makeFrequencyParameter(PID_e::FM_SMOOTH),
												makeAPF2(PID_e::FM_DEGRADE,	makeBitsRange(), 256.f, bitsValueToString, bitsStringToValue)
																									  );
		std::unique_ptr<AudioParameterGroup> PMParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::PM), 	groupToName(GroupID_e::PM), "|",
												makeAPF(PID_e::PM_AMOUNT, 	NormRangeF(0.f, 8.f), 0.f ),
												makeFrequencyParameter(PID_e::PM_TAME),
												makeAPF(PID_e::PM_SHAPE, 	NormRangeF(0.f, 1.f), 0.f ),
												makeAPF2(PID_e::PM_DEGRADE, 	makeBitsRange(), 256.f, bitsValueToString, bitsStringToValue)
																									  );
		std::unique_ptr<AudioParameterGroup> filterParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::FILTER), 	groupToName(GroupID_e::FILTER), "|",
												makeAPF2(PID_e::DRIVE, 		makeGainRange(-60.f, 12.f), 1.0f, valueToString_dB, dB_stringToValue),
											    makeFrequencyParameter(PID_e::CUTOFF, 20.f, 22000.f, 12000.f),
												makeAPF(PID_e::RESO, 	NormRangeF(0.f, 5.f), 1.f ),
												makeFilterTypesParameter(PID_e::TYPE_L),
												makeFilterTypesParameter(PID_e::TYPE_R)
																										  );
		std::unique_ptr<AudioParameterGroup> LFOParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::LFO), 	groupToName(GroupID_e::LFO), "|",
												makeFrequencyParameter(PID_e::LFO_RATE, 0.023f, 230.f, 1.f),
											    makeLFOWaveParameter(PID_e::LFO_WAVE)
																									   );
		std::unique_ptr<AudioParameterGroup> envelopeParameterGroup = std::make_unique<AudioParameterGroup>(
												groupToID(GroupID_e::ENVELOPE), 	groupToName(GroupID_e::ENVELOPE), "|",
												makeAPF(PID_e::DRONE, 		NormRangeF { 0.f, 1.f,
																							[](float start, float end, float normalized) { return std::sqrt(normalized); },
																							[](float start, float end, float value) { return std::pow(value, 2.f); }
																						}, 0.f ),
												makeTimingParameter(PID_e::RISE, 10.f),
												makeTimingParameter(PID_e::FALL, 10.f));

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
				
				/*
				 (const ParameterID& parameterID,
									  const String& parameterName,
									  NormalisableRange<float> normalisableRange,
									  float defaultValue,
									  const AudioParameterFloatAttributes& attributes = {})
				 */
				result->addChild(std::make_unique<APF>(JPID{id, 1},
													   name,
													   juce::NormalisableRange<float>(0.0f, 1.0f),
													   0.0f,
													   AudioParameterFloatAttributes()
													   .withStringFromValueFunction([](float x, int numDecimalPlaces) -> String {
															return juce::String(x*100.f, 1) + "%";
													   })
													   ));
			}
			else {
				std::cerr << "could not cast to parameter with ID\n";
			}
		}
		return result;
	}
	static std::unique_ptr<AudioParameterGroup> makeUtilityParamsGroup() {
		return std::make_unique<AudioParameterGroup>(
							 groupToID(GroupID_e::UTILITY), groupToName(GroupID_e::UTILITY), "|",
							 std::make_unique<ChoiceParam>(
									 juce::ParameterID { paramToID(PID_e::OVERSAMPLE_FACTOR),  1 },
									 paramToName(PID_e::OVERSAMPLE_FACTOR),
									 oversampleLabels,
									 /*default index is 4x*/ 2 ),
							 std::make_unique<APF>(JPID{ paramToID(PID_e::OUTPUT_GAIN), 1 }, paramToName(PID_e::OUTPUT_GAIN),
												   makeGainRange(-60.0f, 0.0f),
												   0.5f,
												   AudioParameterFloatAttributes().withStringFromValueFunction([](float val, int numDecimals){
													   return String(Decibels::gainToDecibels(val), 1) + "dB";
												   })));	// not sure why it is not specified in the log scaling, but here: -6dB default
	}
	static juce::AudioProcessorValueTreeState::ParameterLayout createParamLayout() {
		juce::AudioProcessorValueTreeState::ParameterLayout layout;
		std::unique_ptr<AudioParameterGroup> mainParamsTmp = makeMainParamsGroup();
		std::unique_ptr<AudioParameterGroup> modParamsTmp = makeModulationParamsGroup(*mainParamsTmp.get());
		layout.add(std::move(mainParamsTmp));
		layout.add(std::move(modParamsTmp));
		layout.add(makeUtilityParamsGroup());
		
		return layout;
	}
};

struct SmoothedParamsManager {
	using PID_e = nvs::param::PID_e;
	SmoothedParamsManager(juce::AudioProcessorValueTreeState& apvts)
	:	_apvts(apvts)
	{}
	
	void reset (double sampleRate, int samplesPerBlock) {
		auto const rampTime = samplesPerBlock / sampleRate;
		for (size_t i = 0; i < values.size(); ++i) {
			values[i].reset (sampleRate, rampTime);
			juce::String pid_str = nvs::param::paramToID((PID_e)(i));
			values[i].setCurrentAndTargetValue(*_apvts.getRawParameterValue(pid_str));
		}
	}

	// call once per block
	void updateTargets () {
		for (size_t i = 0; i < values.size(); ++i){
			juce::String pid_str = nvs::param::paramToID((PID_e)(i));
			values[i].setTargetValue(*_apvts.getRawParameterValue (pid_str));
		}
	}

	juce::NormalisableRange<float> getRange(PID_e id) const noexcept {
		return _apvts.getParameterRange(nvs::param::paramToID(id));
	}
	
	float getNextValue (PID_e id) noexcept {
		return values[(size_t)id].getNextValue();
	}
	juce::AudioProcessorValueTreeState& _apvts;
private:
	std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, (size_t)PID_e::NUM_PARAMS> values;
};

}	// namespace param
}	// namespace nvs
