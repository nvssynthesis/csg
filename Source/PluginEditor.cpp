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

ModulatedSlider::ModulatedSlider(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					String paramGroupName,
					Slider::SliderStyle sliderStyle,
					juce::Slider::TextEntryBoxPosition entryPos)
:
	_baseSlider(),
	_baseAttachment(apvts, param.getParameterID(), _baseSlider),
	_modulationSlider(),
	_modulationAttachment(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider),
	_paramName(param.getName(20)),
	_paramGroupName(paramGroupName)
{
	setupSlider(apvts, param.getParameterID(), _baseSlider);
	_baseSlider.setSliderStyle(sliderStyle);
	_baseSlider.setTextBoxStyle(entryPos, true, 50, 25);

	_baseSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	_baseSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::darkgrey);
	
	setupSlider(apvts, nvs::param::makeModID(param.getParameterID()), _modulationSlider);
	_modulationSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	_modulationSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 10, 10);
	
	_modulationSlider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::palevioletred);
	_modulationSlider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);

	_label.setFont({"Palatino", 13.f, 0});
	_label.setJustificationType(juce::Justification::centred);
	_label.attachToComponent(&_baseSlider, false);
	_label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	_label.setText(_paramName, dontSendNotification);
	
	addAndMakeVisible(_baseSlider);
	addAndMakeVisible(_modulationSlider);
	addAndMakeVisible(_label);
}

void ModulatedSlider::resized()
{
	int const pad = 3;
	auto area = getLocalBounds().withTrimmedTop(pad).withTrimmedBottom(pad).withTrimmedLeft(1.1*pad).withTrimmedRight(1.1*pad).toFloat();

	float totalH = area.getHeight();
	float labelH  = totalH * 0.11f;
	float baseH   = totalH * 0.64f;
	float modH   = totalH - labelH - baseH;

	using namespace juce;
	FlexBox flex;

	flex.flexDirection  = FlexBox::Direction::column;     		// vertical main axis
	flex.alignItems     = FlexBox::AlignItems::stretch;   		// stretch to full width
	flex.justifyContent = FlexBox::JustifyContent::flexStart; 	// start at top

	flex.items.add ( FlexItem (_label)           .withFlex (0.0f, 0.0f, labelH)  );
	flex.items.add ( FlexItem (_baseSlider)       .withFlex (0.0f, 0.0f, baseH) );
	flex.items.add ( FlexItem (_modulationSlider).withFlex (0.0f, 0.0f, modH)  );

	flex.performLayout (area);
}

UtilityKnob::UtilityKnob(juce::AudioProcessorValueTreeState &apvts,
					juce::RangedAudioParameter const &param,
					int numDecimalPlacesToDisplay,
					Slider::SliderStyle sliderStyle,
					juce::Slider::TextEntryBoxPosition entryPos)
:	_slider(),
	_attachment(apvts, param.getParameterID(), _slider),
	_paramName(param.getName(20))
{
	setupSlider(apvts, param.getParameterID(), _slider);
	_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, /* width */ 15, /* height */ 10);
	_slider.setColour(Slider::ColourIds::thumbColourId, juce::Colours::grey);
	_slider.setColour(Slider::ColourIds::trackColourId, juce::Colours::darkgrey);
	_slider.setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::lightgrey);

	_label.setFont({"Palatino", 10.f, 0});
	_label.attachToComponent(&_slider, false);
	_label.setText(_paramName, dontSendNotification);
	
	addAndMakeVisible(_slider);
	addAndMakeVisible(_label);
}

void UtilityKnob::resized() {
	_slider.setBounds(getLocalBounds());
}
AttachedComboBox::AttachedComboBox(juce::AudioProcessorValueTreeState &apvts,
								   juce::RangedAudioParameter const &param)
:	_comboBox(),
	_attachment(apvts, param.getParameterID(), _comboBox)
{
	addAndMakeVisible(_comboBox);
}
void AttachedComboBox::resized() {
	_comboBox.setBounds(getLocalBounds());
}

//==============================================================================


CsgAudioProcessorEditor::CsgAudioProcessorEditor (CsgAudioProcessor& p)
: 	AudioProcessorEditor (&p), processor (p)
,	tooltipWindow (this, 500)      // hover‐delay = 500 ms
,	presetPanel(processor.getPresetManager())
{
	juce::AudioProcessorValueTreeState &apvts = processor.getAPVTS();
	
	AudioProcessorParameterGroup const &paramTree = processor.getParameterTree();
	juce::Array<const AudioProcessorParameterGroup*> subgroups = paramTree.getSubgroups(false);


	for (auto const *paramGroup : subgroups){
		if (paramGroup->getID().equalsIgnoreCase(nvs::param::groupToID(nvs::param::GroupID_e::MAIN)))
		{
			for (AudioProcessorParameterGroup const *paramSubGroup : paramGroup->getSubgroups(false))
			{
				for (AudioProcessorParameter const *param : paramSubGroup->getParameters(false))
				{
					if (juce::RangedAudioParameter const *rap = dynamic_cast<juce::RangedAudioParameter const*>(param))
					{
						sliders.push_back(std::make_unique<ModulatedSlider>(apvts, *rap, paramSubGroup->getName()));
					}
				}
			}
		}
		else if (paramGroup->getID().equalsIgnoreCase(nvs::param::groupToID(nvs::param::GroupID_e::MODULATION))){
			// should be automatically attached by the modulatedSlider being constructed above
		}
		else if (paramGroup->getID().equalsIgnoreCase(nvs::param::groupToID(nvs::param::GroupID_e::UTILITY)))
		{
			for (auto const *param : paramGroup->getParameters(false))
			{
				std::cout << param->getName(20) << '\n';
				if (param->getName(20).contains(nvs::param::paramToName(nvs::param::PID_e::OVERSAMPLE_FACTOR)))
				{
					if (auto const *a = dynamic_cast<juce::AudioParameterChoice const*>(param)){
						auto acb = std::make_unique<AttachedComboBox>(apvts, *a);
						
						acb->_comboBox.addItemList (nvs::param::oversampleLabels, /*item IDs start at 1*/ 1);
						acb->_comboBox.setSelectedItemIndex (a->getIndex(), juce::dontSendNotification);
						acb->_comboBox.setTooltip ("Oversampling (Improves filter accuracy for high frequencies)");
						comboBoxes.push_back(std::move(acb));
					}
				}
				else if (juce::RangedAudioParameter const *rap = dynamic_cast<juce::RangedAudioParameter const*>(param))
				{
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
		s->setLookAndFeel(&notchLAF);
		addAndMakeVisible(s.get());
	}
	for (auto &k : knobs){
		k->setLookAndFeel(&notchLAF);
		addAndMakeVisible(k.get());
	}
	for (auto & cb : comboBoxes){
		addAndMakeVisible(cb.get());
	}
	addAndMakeVisible(presetPanel);
	addAndMakeVisible(visitSiteButton);
	visitSiteButton.setColour (juce::HyperlinkButton::textColourId, juce::Colour(Colours::deepskyblue).withAlpha(0.7f));
	visitSiteButton.setFont(Font("Palatino", 14.f, Font::plain), false);
	visitSiteButton.setJustificationType(Justification::bottomLeft);

	backgroundImage = ImageCache::getFromMemory (BinaryData::enclosure480_1_png,
												 BinaryData::enclosure480_1_pngSize);
	
	double const bg_w = backgroundImage.getWidth();
	double const bg_h = backgroundImage.getHeight();

	setResizable (true, true);
	setResizeLimits (bg_w / 2, bg_h / 2, bg_w * 2, bg_h * 2);

	auto const ratio = bg_w / bg_h;
	getConstrainer()->setFixedAspectRatio(ratio);
	getConstrainer()->setMinimumWidth(628);

	setSize (bg_w, bg_h);
}

//==============================================================================
void CsgAudioProcessorEditor::paint (Graphics& g)
{   // let's make this plugin feel like a DIY noise synth
	
	g.setImageResamplingQuality (Graphics::highResamplingQuality);
	g.drawImage (backgroundImage,
				 /*dest*/    0, 0, getWidth(), getHeight(),
				 /*source*/  0, 0, backgroundImage.getWidth(),
							 backgroundImage.getHeight(),
				 /*alpha*/  false);
	
	g.setColour (Colours::black.withAlpha (0.3f));
	g.fillRect (getLocalBounds());

	g.setColour(juce::Colour(Colours::snow).withAlpha(0.5f));
	g.setFont(juce::Font("Palatino", 14.f, Font::plain));
	juce::String displayString ("nvssynthesis csg version ");
	displayString.append(ProjectInfo::versionString, 9);
	auto x_offset = 300;
#ifdef JUCE_DEBUG
	displayString.append(" debug mode", 14);
	x_offset += 30;	// display must starter further left
#endif
	g.drawText(displayString, getWidth() - x_offset - 20, getHeight() - 40, x_offset, 40, Justification::bottomRight);

	g.setColour (findColour (juce::GroupComponent::outlineColourId));
	
	g.setColour(juce::Colour(juce::Colours::snow).withAlpha(0.2f));
	for (auto& area : groupAreas){
		g.drawRect (area.toNearestInt(), 1);  // 1px thick
	}
}

void CsgAudioProcessorEditor::resized()
{

	const auto topPad = 53;
	const int  pad    = 3;
	{	// place output gain knob
		knobs[0]->setBounds(getWidth() - topPad, 0, topPad, topPad);
	}
	{	// place oversampling combo box
		int const cbWidth = 80;
		int const cbPad = 16;
		comboBoxes[0]->setBounds(knobs[0]->getX() - (cbWidth + cbPad), 0 + cbPad, cbWidth, topPad - 2*cbPad);
	}
	

		
	
	// clear out last frame’s group‑areas
	groupAreas.clear();

	// carve off overall padding
	auto area = getLocalBounds()
					.withTrimmedLeft  (pad)
					.withTrimmedRight (pad)
					.withTrimmedBottom(pad * 7)
					.withTrimmedTop   (topPad)
					.toFloat();
	{
//		auto presetPanelBounds = getLocalBounds()
//			.withTrimmedLeft(pad)
//			.withTrimmedRight(pad*6 + knobs[0]->getWidth() + comboBoxes[0]->getWidth())
//			.withTrimmedTop(pad)
//			.withTrimmedBottom(pad*10 + area.getHeight());
		auto const cbb = comboBoxes[0]->getBounds();
		auto presetPanelBounds = cbb
			.withLeft(pad)
			.withRight(cbb.getX());
		presetPanel.setBounds(presetPanelBounds);
	}
	
	// split into equal‐height rows
	const float rowHeight = area.getHeight() * 0.5f;
	auto topArea = area.removeFromTop(rowHeight);
	auto bottomArea = area;

	// define rows by group substring
	const std::vector<juce::String> topGroups    { "FM",     "PM"        };
	const std::vector<juce::String> bottomGroups { "FILTER", "LFO", "ENV" };

	auto layoutRow = [&](juce::Rectangle<float> rowArea,
						 const std::vector<juce::String>& groups) {
		// weights per param group
		std::vector<float> weights;
		for (auto& groupName : groups) {
			if      (groupName == "FILTER") weights.push_back (1.2f);
			else if (groupName == "ENV")    weights.push_back (1.0f);
			else if (groupName == "LFO")    weights.push_back (0.7f);
			else                            weights.push_back (1.0f);
		}

		float totalWeight = std::accumulate (weights.begin(), weights.end(), 0.0f);

		// slice groups proportionally
		for (size_t i = 0; i < groups.size(); ++i) {
			auto& groupName = groups[i];
			float w = weights[i];

			// carve off the proportional slice
			float sliceW   = rowArea.getWidth() * (w / totalWeight);
			auto  groupArea = rowArea.removeFromLeft (sliceW);

			// remember for border‑drawing
			groupAreas.push_back (groupArea);

			// compute a skinnyWidth based on this slice
			const float typeWidthRatio = 0.17f;
			const float skinnyWidth    = groupArea.getWidth() * typeWidthRatio;

			juce::FlexBox flex;
			flex.flexDirection  = juce::FlexBox::Direction::row;
			flex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
			flex.alignItems     = juce::FlexBox::AlignItems::stretch;
			flex.flexWrap       = juce::FlexBox::Wrap::noWrap;

			// collect sliders in this group
			std::vector<ModulatedSlider*> groupSliders;
			for (auto& s : sliders) {
				if (s->getParamGroupName().contains (groupName)) {
					groupSliders.push_back (s.get());
				}
			}
			// add them, using the proportional skinnyWidth for TYPE
			for (auto* slider : groupSliders) {
				if (slider->getParamName().contains ("TYPE")) {	// filter type selector slider
					flex.items.add ( juce::FlexItem (*slider)
										 .withFlex     (0, 0)
										 .withMinWidth (skinnyWidth)
										 .withMaxWidth (skinnyWidth) );
				}
				else {
					flex.items.add ( juce::FlexItem (*slider)
										 .withFlex (1.0f, 1.0f) );
				}
			}

			// subtract current group’s weight so the next slice is still proportional
			totalWeight -= w;

			flex.performLayout (groupArea);
		}
	};

	layoutRow (topArea,    topGroups);
	layoutRow (bottomArea, bottomGroups);
	
	visitSiteButton.setBounds (pad, getHeight() - pad * 5,
							   178, pad * 5);
}

