/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


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



struct ModulatedSlider	:	public juce::Component
{
	using Slider = juce::Slider;
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using String = juce::String;
	
	ModulatedSlider(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					String paramGroupName = "",
					Slider::SliderStyle sliderStyle = juce::Slider::LinearBarVertical,
					juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::TextBoxBelow);
	void resized();
	String getParamName() const { return _paramName; }
	String getParamGroupName() const { return _paramGroupName; }
	
	Slider _baseSlider;
	SliderAttachment _baseAttachment;
	Slider _modulationSlider;
	SliderAttachment _modulationAttachment;
	
private:
	String _paramName;
	String _paramGroupName;
	juce::Label _label;
};

struct UtilityKnob	:	public juce::Component
{
	using Slider = juce::Slider;
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using String = juce::String;
	
	UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					int numDecimalPlacesToDisplay = 3,
					Slider::SliderStyle sliderStyle = juce::Slider::LinearBarVertical,
				juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::TextBoxBelow);
	
	String getParamName() const { return _paramName; }

	Slider _slider;
	SliderAttachment _attachment;
	
private:
	String _paramName;
	juce::Label _label;
};


class CsgAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    CsgAudioProcessorEditor (CsgAudioProcessor&);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CsgAudioProcessor& processor;
    
	std::vector<std::unique_ptr<ModulatedSlider>> sliders;
	std::vector<std::unique_ptr<UtilityKnob>> knobs;
	
	std::vector<juce::Rectangle<float>> groupAreas;
	
	Image backgroundImage;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
