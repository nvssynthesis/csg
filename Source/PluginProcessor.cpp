/*
  ==========================================================================

    This file was auto-generated!

    It contains the basic framework code for  a JUCE plugin processor.

  ==========================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==========================================================================
void initAPVTS(juce::AudioProcessorValueTreeState &apvts){
	if (!apvts.state.hasProperty("version")){
		apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);
	}
	if (!apvts.state.hasProperty(nvs::service::PresetManager::presetNameProperty)){
		apvts.state.setProperty(nvs::service::PresetManager::presetNameProperty, "", nullptr);
	}
}

CsgAudioProcessor::CsgAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif

params(*this),
sharedState(params.apvts)
{
	initAPVTS(params.apvts);
	
	presetManager = std::make_unique<nvs::service::PresetManager>(params.apvts);	// need to pass fully initialized apvts

	initModParams();
	
	using namespace nvs::csg;
	csgSynth.setCurrentPlaybackSampleRate(44100.f);
    csgSynth.clearVoices();
    csgSynth.addVoice(new nvs::csg::CSGVoice(&sharedState));   // MONOPHONIC BEAST.
    csgSynth.clearSounds();
    csgSynth.addSound(new nvs::csg::CSGSound());
}

CsgAudioProcessor::~CsgAudioProcessor()
{
}

//==========================================================================
const String CsgAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CsgAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CsgAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CsgAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CsgAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CsgAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CsgAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CsgAudioProcessor::setCurrentProgram (int index)
{
}

const String CsgAudioProcessor::getProgramName (int index)
{
    return {};
}

void CsgAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==========================================================================
void CsgAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    csgSynth.setCurrentPlaybackSampleRate(sampleRate);

	if (auto* csg_voice = dynamic_cast<nvs::csg::CSGVoice*>(csgSynth.getVoice(0))) {
		csg_voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void CsgAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CsgAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CsgAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    buffer.clear ();
	//======================================================================
    csgSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	//======================================================================
#ifdef DBG
	for (auto i = 0; i < buffer.getNumChannels(); ++i){
		for (auto j = 0; j < buffer.getNumSamples(); ++j){
			auto *wp = buffer.getWritePointer(i, j);
			*wp = juce::jlimit(-1.f, 1.f, *wp);
		}
	}
#endif
}

//==========================================================================
bool CsgAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CsgAudioProcessor::createEditor()
{
    return new CsgAudioProcessorEditor (*this);
}

//==========================================================================
void CsgAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	juce::ValueTree stateTree = params.apvts.copyState();
	
	std::unique_ptr<juce::XmlElement> xml = stateTree.createXml();
	
	if (xml != nullptr)
		juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

void CsgAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xml = juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes);
	
	if (xml != nullptr)
	{
		juce::ValueTree stateTree = juce::ValueTree::fromXml(*xml);
				
		if (stateTree.isValid())
			params.apvts.replaceState(stateTree);
			initAPVTS(params.apvts);
	}
}

juce::AudioProcessorValueTreeState &CsgAudioProcessor::getAPVTS(){
	return params.apvts;
}


	
//==========================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CsgAudioProcessor();
}
