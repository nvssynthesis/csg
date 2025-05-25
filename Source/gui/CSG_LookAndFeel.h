/*
  ==============================================================================

    CSG_LookAndFeel.h
    Created: 24 May 2025 9:26:32pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class NotchLookAndFeel  : public juce::LookAndFeel_V4
{
public:
	NotchLookAndFeel();
	void drawLinearSlider (Graphics& g,
						   int x, int y, int width, int height,
						   float sliderPos,
						   float minSliderPos, float maxSliderPos,
						   Slider::SliderStyle style,
						   Slider& s) override;
	
	void drawPieSegment(juce::Graphics &g, juce::Rectangle<float> ellipseRect, float angle, float notchWidth, float sliderPosProportional, juce::Colour notchCol);
	
	void drawRotarySlider (juce::Graphics& g,
						   int x, int y, int width, int height,
						   float sliderPosProportional,
						   float rotaryStartAngle,
						   float rotaryEndAngle,
						   juce::Slider& slider) override;
	
private:
	float const notchWidthDegrees {8.f};
	juce::Colour notchColour {juce::Colour(Colours::blueviolet).withMultipliedLightness(0.5f)};
};
