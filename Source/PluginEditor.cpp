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
#ifdef JUCE_DEBUG
	displayString.append(" debug mode", 14);
#endif
	g.drawText(displayString, nameAndVersionBounds, Justification::bottomRight);

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
	
	// clear out last frame’s group‑areas
	groupAreas.clear();
	auto const area = getLocalBounds()
					.withTrimmedLeft  (pad)
					.withTrimmedRight (pad)
					.withTrimmedBottom(pad)
					.withTrimmedTop   (pad)
					.toFloat();
	
	{
		auto presetPanelBounds = area.withHeight((topPad * 0.6) - pad).withRight(knobs[0]->getBounds().getX() - pad).toNearestInt();
		presetPanel.setBounds(presetPanelBounds);
	}
	
	// carve off overall padding
	auto const paramsArea = area.withTrimmedTop(topPad).withTrimmedBottom(pad * 8);
	{
		// split into equal‐height rows
		const float rowHeight = paramsArea.getHeight() * 0.5f;
		
		auto bottomArea = paramsArea;
		auto topArea = bottomArea.removeFromTop(rowHeight);	// returns the section that was removed!

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
	}
	
	auto const subArea = area.withTrimmedTop(paramsArea.getHeight() + 2*pad + topPad);
	
	visitSiteButton.setBounds (subArea.getX(), subArea.getY(),
							   178, subArea.getHeight());
	visitSiteButton.setCentrePosition(visitSiteButton.getBounds().getCentreX(), subArea.getCentreY());
	auto const x_offset =
#ifdef JUCE_DEBUG
	330;
#else
	30;
#endif
	nameAndVersionBounds = juce::Rectangle<int>(subArea.getRight() - x_offset - 20, subArea.getY(),
												x_offset, subArea.getHeight());
	nameAndVersionBounds.setCentre(nameAndVersionBounds.getCentreX(), subArea.getCentreY());
	
	
	
	{	// place oversampling combo box
		comboBoxes[0]->setBounds(visitSiteButton.getRight() + 10, subArea.getY(),
														80, subArea.getHeight());
	}
}

