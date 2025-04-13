/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
CsgAudioProcessorEditor::CsgAudioProcessorEditor (CsgAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	juce::AudioProcessorValueTreeState &apvts = processor.getAPVTS();
	
	AudioProcessorParameterGroup const &paramTree = processor.getParameterTree();
	juce::Array<const AudioProcessorParameterGroup*> subgroups = paramTree.getSubgroups(false);
	for (auto const *paramGroup : subgroups){
		if (paramGroup->getID().equalsIgnoreCase("MAIN_PARAMS")){
			for (AudioProcessorParameter const *param : paramGroup->getParameters(true)){
				if (juce::RangedAudioParameter const *rap = dynamic_cast<juce::RangedAudioParameter const*>(param)){
					sliders.push_back(std::make_unique<ModulatedSlider>(apvts, *rap));
				}
			}
		}
		else if (paramGroup->getID().equalsIgnoreCase("MODULATION_PARAMS")){
			// should be automatically attached by the modulatedSlider being constructed above
		}
		else if (paramGroup->getID().equalsIgnoreCase("OUTPUT_PARAMS")){
			for (auto const *param : paramGroup->getParameters(false)){
				if (juce::RangedAudioParameter const *rap = dynamic_cast<juce::RangedAudioParameter const*>(param)){
					knobs.push_back(std::make_unique<UtilityKnob>(apvts, *rap));
				}
			}
		}
		else {
			jassertfalse;
		}
	}
	
	// make them visible
	for (auto &s : sliders){
		addAndMakeVisible(s.get());
	}
	for (auto &k : knobs){
		addAndMakeVisible(k.get());
	}
	
	setSize (640, 432);
}

CsgAudioProcessorEditor::~CsgAudioProcessorEditor()
{
 
}

//==============================================================================
void CsgAudioProcessorEditor::paint (Graphics& g)
{   // let's make this plugin feel like a DIY noise synth
    Image background = ImageCache::getFromMemory (BinaryData::enclosure_medium_png, BinaryData::enclosure_medium_pngSize);
    g.drawImageAt (background, 0, 0);
	g.setColour(Colours::white);
#ifdef JUCE_DEBUG
	g.drawText("debug", 1, 1, 100, 10, Justification::topLeft);
#endif
}

void CsgAudioProcessorEditor::resized()
{
	const auto topPad = 20;
	const int pad = 10;
	auto area = getLocalBounds().withTrimmedLeft(pad).withTrimmedRight(pad).withTrimmedBottom(pad).withTrimmedTop(topPad).toFloat();

	const int numRows       = 2;
	const int numSliders    = (int) sliders.size();
	const int itemsPerRow   = (numSliders + numRows - 1) / numRows;  // ceil(N/2)
	const float rowHeight   = area.getHeight() / (float) numRows;

	using namespace juce;

	for (int row = 0; row < numRows; ++row)
	{
		// carve off the top rowHeight from our area
		auto rowArea = area.removeFromTop(rowHeight);

		FlexBox flex;
		flex.flexDirection  = FlexBox::Direction::row;
		flex.justifyContent = FlexBox::JustifyContent::spaceBetween;
		flex.alignItems     = FlexBox::AlignItems::stretch;

		int startIndex = row * itemsPerRow;
		int endIndex   = jmin(startIndex + itemsPerRow, numSliders);

		for (int i = startIndex; i < endIndex; ++i)
			flex.items.add ( FlexItem (*sliders[i]).withFlex (1, 1) );

		flex.performLayout (rowArea);
	}
}
