/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"


CsgAudioProcessorEditor::CsgAudioProcessorEditor (CsgAudioProcessor& p)
: 	AudioProcessorEditor (&p), processor (p)
,	csgLAF()
,	tooltipWindow (this, 100)      // hover‐delay = 500 ms
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
						sliders.push_back(std::make_unique<nvs::gui::ModulatedSlider>(apvts, *rap, paramSubGroup->getName()));
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
				auto const pName = param->getName(20);
				if (pName.contains(nvs::param::paramToName(nvs::param::parameter_e::OVERSAMPLE_FACTOR)))
				{
					if (auto const *a = dynamic_cast<juce::AudioParameterChoice const*>(param)){
						auto acb = std::make_unique<AttachedComboBox>(apvts, *a);
						
						acb->_comboBox.addItemList (nvs::param::oversampleLabels, /*item IDs start at 1*/ 1);
						acb->_comboBox.setSelectedItemIndex (a->getIndex(), juce::dontSendNotification);
						acb->_comboBox.setTooltip ("Oversampling (Improves filter accuracy for high frequencies)");
						acb->setName(pName);
						comboBoxes.push_back(std::move(acb));
					}
				}
				else if (pName.contains(nvs::param::paramToName(nvs::param::parameter_e::FEEDBACK_CIRCUIT)))
				{
					if (auto const *a = dynamic_cast<juce::AudioParameterInt const*>(param)){
						auto acb = std::make_unique<AttachedComboBox>(apvts, *a);
						
						acb->_comboBox.addItemList (nvs::param::feedbackCircuitLabels, /*item IDs start at 1*/ 1);
						int const idx = *apvts.getRawParameterValue(nvs::param::paramToID(nvs::param::parameter_e::FEEDBACK_CIRCUIT));
						acb->_comboBox.setSelectedItemIndex (idx, juce::dontSendNotification);
						acb->_comboBox.setTooltip ("Selects the feedback circuit type for MEMORY");
						acb->setName(pName);
						comboBoxes.push_back(std::move(acb));
					}
				}
				else if (pName.contains(nvs::param::paramToName(nvs::param::parameter_e::FILTER_CHARACTER)))
				{
					if (auto const *a = dynamic_cast<juce::AudioParameterInt const *>(param)){
						auto acb = std::make_unique<AttachedComboBox>(apvts, *a);
						
						acb->_comboBox.addItemList(nvs::param::filterCharacterLabels, /*start at*/ 1);
						int const idx = *apvts.getRawParameterValue(nvs::param::paramToID(nvs::param::parameter_e::FILTER_CHARACTER));
						acb->_comboBox.setSelectedItemIndex(idx, juce::dontSendNotification);
						acb->_comboBox.setTooltip("Selects the state variable filter character");
						acb->setName(pName);
						comboBoxes.push_back(std::move(acb));
					}
				}
				else if (juce::RangedAudioParameter const *rap = dynamic_cast<juce::RangedAudioParameter const*>(param))
				{
					knobs.push_back(std::make_unique<nvs::gui::UtilityKnob>(apvts, *rap));
				}
			}
		}
		else {
			jassertfalse;
		}
	}
	
	std::vector<juce::String> disabledModulationParams = {
		nvs::param::paramToName(nvs::param::parameter_e::TYPE_L),
		nvs::param::paramToName(nvs::param::parameter_e::TYPE_R),
		nvs::param::paramToName(nvs::param::parameter_e::RISE),
		nvs::param::paramToName(nvs::param::parameter_e::FALL)
	};
	
	// make them visible
	for (auto &s : sliders){
		s->setLookAndFeel(&csgLAF);
		for (auto const & p : disabledModulationParams)
		{
			if (s->getParamName() == p){
				s->disableModulation();
			}
		}
		addAndMakeVisible(s.get());
	}
	for (auto &k : knobs){
		k->setLookAndFeel(&csgLAF);
		addAndMakeVisible(k.get());
	}
	for (auto & cb : comboBoxes){
		cb->setLookAndFeel(&csgLAF);
		addAndMakeVisible(cb.get());
	}
	presetPanel.setLookAndFeel(&csgLAF);
	addAndMakeVisible(presetPanel);
	
	tooltipWindow.setLookAndFeel(&csgLAF);
	
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
	getConstrainer()->setMinimumWidth(712);

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
				if 		(groupName == "FM") 	weights.push_back(5.0 / 9.0);
				else if (groupName == "PM") 	weights.push_back(4.0 / 9.0);
				else if	(groupName == "FILTER") weights.push_back (1.2f);
				else if (groupName == "ENV")    weights.push_back (0.8f);
				else if (groupName == "LFO")    weights.push_back (0.5f);
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
				const float skinnyWidth    = 46.f; //groupArea.getWidth() * 0.17f;

				juce::FlexBox flex;
				flex.flexDirection  = juce::FlexBox::Direction::row;
				flex.justifyContent = juce::FlexBox::JustifyContent::center;
				flex.alignItems     = juce::FlexBox::AlignItems::stretch;
				flex.flexWrap       = juce::FlexBox::Wrap::noWrap;

				// collect sliders in this group
				std::vector<nvs::gui::ModulatedSlider*> groupSliders;
				for (auto& s : sliders) {
					if (s->getParamGroupName().contains (groupName)) {
						groupSliders.push_back (s.get());
					}
				}
				// add them, using the proportional skinnyWidth for TYPE
				const float sliderPadding = groupArea.getWidth() * 0.0f; // adjust this value to control thickness
				for (auto* slider : groupSliders) {
					if (slider->getParamName().contains ("TYPE")) {	// filter type selector slider
						flex.items.add ( juce::FlexItem (*slider)
											 .withFlex     (1.0f, 10.0f)
											 .withMinWidth (skinnyWidth * 1.0f)
											 .withMaxWidth (skinnyWidth * 1.5f )
											 .withMargin(juce::FlexItem::Margin(0, sliderPadding* 0.2f, 0, sliderPadding * 0.2f))
										);
					}
					else {
						flex.items.add ( juce::FlexItem (*slider)
											.withFlex (2.0f, 0.0f)
											.withMinWidth(50.f)
											.withMargin ({0, sliderPadding*0.7f, 0, sliderPadding*0.7f})
										);
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
	294;
#endif
	nameAndVersionBounds = juce::Rectangle<int>(subArea.getRight() - x_offset - 20, subArea.getY(),
												x_offset, subArea.getHeight());
	nameAndVersionBounds.setCentre(nameAndVersionBounds.getCentreX(), subArea.getCentreY());
	

	std::optional<AttachedComboBox* const> oversampleBoxOpt = [this, subArea]() -> std::optional<AttachedComboBox* const>{
		auto it = std::find_if(comboBoxes.begin(), comboBoxes.end(),
			[](const std::unique_ptr<AttachedComboBox>& cb) {
				return cb->getName().contains(nvs::param::paramToName(nvs::param::parameter_e::FILTER_CHARACTER));
			});
		
		if (it != comboBoxes.end()) {
			AttachedComboBox* const box = it->get();
			box->setBounds(visitSiteButton.getRight() + 10, subArea.getY(),
									 80, subArea.getHeight());
			return box;
		}
		return std::nullopt;
	}();
	if (oversampleBoxOpt.has_value())
	{
		auto const osBox = oversampleBoxOpt.value();
		
		auto it = std::find_if(comboBoxes.begin(), comboBoxes.end(),
			[](const std::unique_ptr<AttachedComboBox>& cb) {
				return cb->getName().contains(nvs::param::paramToName(nvs::param::parameter_e::OVERSAMPLE_FACTOR));
			});
		
		if (it != comboBoxes.end()) {
			AttachedComboBox* const box = it->get();
			box->setBounds(osBox->getRight() + 10, subArea.getY(),
									 80, subArea.getHeight());
		}
	}

	{
//		else if (cb->getName().contains(nvs::param::paramToName(nvs::param::PID_e::FEEDBACK_CIRCUIT))){
		auto it = std::find_if(comboBoxes.begin(), comboBoxes.end(),
			[](const std::unique_ptr<AttachedComboBox>& cb) {
				return cb->getName().contains(nvs::param::paramToName(nvs::param::parameter_e::FEEDBACK_CIRCUIT));
			});
		
		if (it != comboBoxes.end()) {
			AttachedComboBox* const box = it->get();

			auto const presetPanelBounds = presetPanel.getBounds();
			auto const paramsTopLeft = paramsArea.getTopLeft();
			
			auto const x = paramsTopLeft.getX();// + pad;
			auto const y = presetPanelBounds.getBottom() + pad;
			auto const w = 100;
			auto const h = paramsTopLeft.getY() - presetPanelBounds.getBottom() - (2*pad);
			
			box->setBounds(x, y, w, h);
		}
	}
	
}

