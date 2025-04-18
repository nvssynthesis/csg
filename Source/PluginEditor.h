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
		auto scheme = getDarkColourScheme();
		scheme.setUIColour(ColourScheme::UIColour::defaultFill, notchColour);
		setColourScheme(scheme);
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
		juce::Rectangle<float> ellipseRect(centreX - radius,
										   centreY - radius,
										   radius * 2.0f,
										   radius * 2.0f);
		g.drawEllipse (ellipseRect,
					   2.0f);

		const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

		using juce::Path;
		using Point = juce::Point<float>;
		using Line = juce::Line<float>;
	
		// compute notch endpoints
		const float innerRadius     = radius * 0.2f;  // start of the notch
		const float notchLength     = radius * 0.6f;  // how long the notch is
		const float outerRadius     = innerRadius + notchLength;
		const float notchThickness  = 0.9f;           // thickness of the line
		[[maybe_unused]] auto makeLine = [=](float a){
			float x1 = centreX + std::cos (a) * innerRadius;
			float y1 = centreY + std::sin (a) * innerRadius;
			float x2 = centreX + std::cos (a) * outerRadius;
			float y2 = centreY + std::sin (a) * outerRadius;
			return Line(Point(x1, y1), Point(x2, y2));
		};
		
		juce::Path p;
		p.addPieSegment(ellipseRect.reduced(5.f), angle - juce::degreesToRadians(notchWidthDegrees), angle + juce::degreesToRadians(notchWidthDegrees), 0.f);
//		p.closeSubPath();  // <— this closes the polygon

		// Now fill it
		auto notchColour = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::defaultFill);
		if (sliderPosProportional == 0.0f){
			notchColour = notchColour.withAlpha(0.5f);
		}
		g.setColour(notchColour);
		g.fillPath (p);
	}
	
private:
	float const notchWidthDegrees {6.f};
	juce::Colour notchColour {Colours::blueviolet};
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
struct AttachedComboBox	:	public juce::Component
{
	using ComboBox = juce::ComboBox;
	using ComboBoxAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
	using AudioParamChoice = juce::AudioParameterChoice;
	
	AttachedComboBox(juce::AudioProcessorValueTreeState &apvts,
					 juce::RangedAudioParameter const &param);
	
	void resized() override;

	ComboBox _comboBox;
	ComboBoxAttachment _attachment;
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
	std::vector<std::unique_ptr<AttachedComboBox>> comboBoxes;
	
	std::vector<juce::Rectangle<float>> groupAreas;
	
	Image backgroundImage;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsgAudioProcessorEditor)
};
