/*
  ==============================================================================

    util.h
    Created: 24 May 2025 9:37:41pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace {
void setupSlider(juce::AudioProcessorValueTreeState const &state, juce::String paramID, juce::Slider &slider) {
	// adapted from https://github.com/juce-framework/JUCE/blob/master/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.cpp#L464
	juce::NormalisableRange<float> range (state.getParameterRange (paramID));
	
	if (range.interval != 0.0f || range.skew != 1.0f) {
		slider.setRange (range.start, range.end, range.interval);
		slider.setSkewFactor (range.skew, range.symmetricSkew);
	}
	else {
		auto convertFrom0To1Function = [range](double currentRangeStart,
											   double currentRangeEnd,
											   double normalisedValue) mutable
		{
			range.start = (float) currentRangeStart;
			range.end = (float) currentRangeEnd;
			return (double) range.convertFrom0to1 ((float) normalisedValue);
		};
		
		auto convertTo0To1Function = [range](double currentRangeStart,
											 double currentRangeEnd,
											 double mappedValue) mutable
		{
			range.start = (float) currentRangeStart;
			range.end = (float) currentRangeEnd;
			return (double) range.convertTo0to1 ((float) mappedValue);
		};
		
		auto snapToLegalValueFunction = [range](double currentRangeStart,
												double currentRangeEnd,
												double valueToSnap) mutable
		{
			range.start = (float) currentRangeStart;
			range.end = (float) currentRangeEnd;
			return (double) range.snapToLegalValue ((float) valueToSnap);
		};
		
		slider.setNormalisableRange ({ (double) range.start,
			(double) range.end,
			convertFrom0To1Function,
			convertTo0To1Function,
			snapToLegalValueFunction });
	}
}
}
