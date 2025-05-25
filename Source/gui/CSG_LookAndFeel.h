/*
  ==============================================================================

    CSG_LookAndFeel.h
    Created: 24 May 2025 9:26:32pm
    Author:  Nicholas Solem

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace nvs::gui
{
class CSG_LookAndFeel  : public juce::LookAndFeel_V4
{
public:
	CSG_LookAndFeel();
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
	
	void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
					   int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& cb) override;
	Font getComboBoxFont (ComboBox& /*cb*/) override;
	
	Font getPopupMenuFont() override;
	void drawPopupMenuBackground (Graphics& g, int width, int height) override;
	void drawPopupMenuItem (Graphics& g,
							const Rectangle<int>& area,
							bool isSeparator, bool isActive,
							bool isHighlighted, bool isTicked,
							bool hasSubMenu, const String& text,
							const String& shortcutKeyText,
							const Drawable* icon, Colour const* textColour) override;
	
	void drawButtonBackground (Graphics& g, Button& b,
							   const Colour& backgroundColour,
							   bool isMouseOverButton, bool isButtonDown) override;
	void drawButtonText (Graphics& g, TextButton& b,
						 bool /*isMouseOverButton*/, bool /*isButtonDown*/) override;

	void drawTooltip (Graphics &, const String &text, int width, int height) override;
private:
	float const notchWidthDegrees {8.f};
	juce::Colour notchColour {juce::Colour(Colours::blueviolet).withMultipliedLightness(0.5f)};
};
}
