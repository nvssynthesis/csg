/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class NotchLookAndFeel  : public juce::LookAndFeel_V4
{
public:
	NotchLookAndFeel()
	{
		// You can customize default colours here if you like:
		setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
		setColour (juce::Slider::thumbColourId, juce::Colours::white); // not used
	}

	void drawRotarySlider (juce::Graphics& g,
						   int x, int y, int width, int height,
						   float sliderPosProportional,
						   float rotaryStartAngle,
						   float rotaryEndAngle,
						   juce::Slider& slider) override
	{
		const float radius = juce::jmin (width, height) * 0.4f;
		const float centreX = x + width  * 0.5f;
		const float centreY = y + height * 0.5f;

		// draw the circular outline
		g.setColour (slider.findColour (juce::Slider::rotarySliderOutlineColourId));
		g.drawEllipse (centreX - radius,
					   centreY - radius,
					   radius * 2.0f,
					   radius * 2.0f,
					   2.0f);

		// compute current angle
		rotaryStartAngle -= juce::MathConstants<float>::halfPi;
		rotaryEndAngle -= juce::MathConstants<float>::halfPi;
		const float angle = rotaryStartAngle
						  + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

		
		std::cout << "rotaryStartAngle: " << rotaryStartAngle << "\n";
		std::cout << "\tsliderPosProportional: " << sliderPosProportional << "\n";
		std::cout << "\t\trotaryEndAngle: " << rotaryEndAngle << "\n";
		std::cout << "\t\t\trotaryStartAngle: " << rotaryStartAngle << "\n";

		std::cout << "\t\t\t\tangle: " << angle << "\n";

		// compute notch endpoints
		const float notchLength     = radius * 0.15f;  // how long the notch is
		const float notchThickness  = 2.0f;           // thickness of the line
		const float innerRadius     = radius * 0.6f;  // start of the notch
		const float outerRadius     = innerRadius + notchLength;

		const float x1 = centreX + std::cos (angle) * innerRadius;
		const float y1 = centreY + std::sin (angle) * innerRadius;
		const float x2 = centreX + std::cos (angle) * outerRadius;
		const float y2 = centreY + std::sin (angle) * outerRadius;

		// draw the notch
		g.drawLine (x1, y1, x2, y2, notchThickness);
	}
};
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
	void resized() override;
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
	void resized() override;
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
    
	NotchLookAndFeel notchLAF;
	
	std::vector<std::unique_ptr<ModulatedSlider>> sliders;
	std::vector<std::unique_ptr<UtilityKnob>> knobs;
	
	std::vector<juce::Rectangle<float>> groupAreas;
	
	Image backgroundImage;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
