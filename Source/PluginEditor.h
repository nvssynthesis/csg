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
					int numDecimalPlacesToDisplay = 3,
					Slider::SliderStyle sliderStyle = juce::Slider::LinearBarVertical,
					juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::TextBoxBelow)
	:
	_baseSlider(),
	_baseAttachment(apvts, param.getParameterID(), _baseSlider),
	_modulationSlider(),
	_modulationAttachment(apvts, nvs::param::getBaseIDFromModPID(param.getParameterID()), _modulationSlider),
	_paramName(param.getName(20))
	{
		setupSlider(apvts, param.getParameterID(), _baseSlider);
		_baseSlider.setSliderStyle(sliderStyle);
		_baseSlider.setTextBoxStyle(entryPos, false, 50, 25);

		_baseSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
		_baseSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);
		_baseSlider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);
		
		setupSlider(apvts, nvs::param::getBaseIDFromModPID(param.getParameterID()), _modulationSlider);
		_modulationSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		_modulationSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 10, 10);
		
		_modulationSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
		_modulationSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);
		_modulationSlider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);

		_label.setFont({"Palatino", 13.f, 0});
		_label.attachToComponent(&_baseSlider, false);
		_label.setText(_paramName, dontSendNotification);
		
		addAndMakeVisible(_baseSlider);
		addAndMakeVisible(_modulationSlider);
		addAndMakeVisible(_label);
	}
	void resized() override {
		auto const b = getLocalBounds();
		_baseSlider.setBounds(b.getX(), b.getY(), b.getWidth(), b.getHeight());
	}
	
	Slider _baseSlider;
	SliderAttachment _baseAttachment;
	Slider _modulationSlider;
	SliderAttachment _modulationAttachment;
	
	String getParamName() const { return _paramName; }
private:
	String _paramName;
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
					juce::Slider::TextEntryBoxPosition entryPos = juce::Slider::TextBoxBelow)
	:
	_slider(),
	_attachment(apvts, param.getParameterID(), _slider),
	_paramName(param.getName(20))
	{
		setupSlider(apvts, param.getParameterID(), _slider);
		_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, /* width */ 15, /* height */ 10);
		_slider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::grey);
		_slider.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkgrey);
		_slider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);

		_label.setFont({"Palatino", 10.f, 0});
		_label.attachToComponent(&_slider, false);
		_label.setText(_paramName, dontSendNotification);
		
		addAndMakeVisible(_slider);
		addAndMakeVisible(_label);
	}
	
	Slider _slider;
	SliderAttachment _attachment;
	
	String getParamName() const { return _paramName; }
private:
	String _paramName;
	juce::Label _label;
};


class CsgAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    CsgAudioProcessorEditor (CsgAudioProcessor&);
    ~CsgAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CsgAudioProcessor& processor;
    
	std::vector<std::unique_ptr<ModulatedSlider>> sliders;
	std::vector<std::unique_ptr<UtilityKnob>> knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
