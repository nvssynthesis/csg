/*
  ==========================================================================

    This file was auto-generated!

    It contains the basic framework code for  a JUCE plugin processor.

  ==========================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
 
//==========================================================================
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

apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
	csg_synth.setCurrentPlaybackSampleRate(44100.f); 
    csg_synth.clearVoices();
    csg_synth.addVoice(new CSGVoice);   // MONOPHONIC BEAST.
    csg_synth.clearSounds();
    csg_synth.addSound(new CSGSound());
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
    //ignoreUnused(samplesPerBlock);
    oneOverBlockSize = 1.f / samplesPerBlock;
    lastSampleRate = sampleRate;
    csg_synth.setCurrentPlaybackSampleRate(lastSampleRate);

    // you need the extra parentheses to avoid a warning "Using the result of an assignment as a condition without parentheses"
    if ((csg_voice = dynamic_cast<CSGVoice*>(csg_synth.getVoice(0))))
    {
        csg_voice->unit.setSampleRate(lastSampleRate);
        csg_voice->svf.setSampleRate(lastSampleRate);
        csg_voice->svf.clear();
        csg_voice->env.setRise(0.01f);
        csg_voice->env.setFall(0.01f);
        csg_voice->lfo.setSampleRate(lastSampleRate);
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

    if ((csg_voice = dynamic_cast<CSGVoice*>(csg_synth.getVoice(0))))
    {
        if (Last_Self_FM != *apvts.getRawParameterValue("Self_FM"))
        {
            Last_Self_FM = *apvts.getRawParameterValue("Self_FM");
            csg_voice->unit.selfFM_target = Last_Self_FM;
        }
        if (Last_Memory != *apvts.getRawParameterValue("Memory"))
        {
            Last_Memory = *apvts.getRawParameterValue("Memory");
            csg_voice->unit.memory_target = Last_Memory;
        }
        if (Last_FM_Smooth != *apvts.getRawParameterValue("FM_Smooth"))
        {
            Last_FM_Smooth = *apvts.getRawParameterValue("FM_Smooth");
            csg_voice->unit.FM_smooth_target = Last_FM_Smooth;
        }
        if (Last_Bits_A != *apvts.getRawParameterValue("Bits_A"))
        {
            Last_Bits_A = *apvts.getRawParameterValue("Bits_A");
            csg_voice->unit.bits1_target = Last_Bits_A;
        }
        if (Last_PM_Preamp != *apvts.getRawParameterValue("PM_Preamp"))
        {
            Last_PM_Preamp = *apvts.getRawParameterValue("PM_Preamp");
            csg_voice->unit.PM_preamp_target = Last_PM_Preamp;
        }
        if (Last_PM_Smooth != *apvts.getRawParameterValue("PM_Smooth"))
        {
            Last_PM_Smooth = *apvts.getRawParameterValue("PM_Smooth");
            csg_voice->unit.PM_smooth_target = Last_PM_Smooth;
        }
        if (Last_Sin2Cos != *apvts.getRawParameterValue("Sin2Cos"))
        {
            Last_Sin2Cos = *apvts.getRawParameterValue("Sin2Cos");
            csg_voice->unit.PM_sin2cos_target = Last_Sin2Cos;
        }
        if (Last_Bits_B != *apvts.getRawParameterValue("Bits_B"))
        {
            Last_Bits_B = *apvts.getRawParameterValue("Bits_B");
            csg_voice->unit.bits2_target = Last_Bits_B;
        }
        if (Last_Drone != *apvts.getRawParameterValue("Drone"))
        {
            Last_Drone = *apvts.getRawParameterValue("Drone");
            csg_voice->voice_drone = Last_Drone;
        }
        if (Last_Rise != *apvts.getRawParameterValue("Rise"))
        {
            Last_Rise = *apvts.getRawParameterValue("Rise");
            csg_voice->voice_rise = Last_Rise;
        }
        if (Last_Fall != *apvts.getRawParameterValue("Fall"))
        {
            Last_Fall = *apvts.getRawParameterValue("Fall");
            csg_voice->voice_fall = Last_Fall;
        }
        if (Last_Cutoff != *apvts.getRawParameterValue("Cutoff"))
        {
            Last_Cutoff = *apvts.getRawParameterValue("Cutoff");
            csg_voice->voice_cutoff = Last_Cutoff;
        }
        if (Last_Resonance != *apvts.getRawParameterValue("Resonance"))
        {
            Last_Resonance = *apvts.getRawParameterValue("Resonance");
            csg_voice->voice_res = Last_Resonance;
        }
        if (Last_MODselfFM != *apvts.getRawParameterValue("Self_FM_MOD"))
        {
            Last_MODselfFM = *apvts.getRawParameterValue("Self_FM_MOD");
            csg_voice->MODselfFM = Last_MODselfFM;
        }
        if (Last_MODMemory != *apvts.getRawParameterValue("Memory_MOD"))
        {
            Last_MODMemory = *apvts.getRawParameterValue("Memory_MOD");
            csg_voice->MODMemory = Last_MODMemory;
        }
        if (Last_MODFMsmooth != *apvts.getRawParameterValue("FM_Smooth_MOD"))
        {
            Last_MODFMsmooth = *apvts.getRawParameterValue("FM_Smooth_MOD");
            csg_voice->MODFMsmooth = Last_MODFMsmooth;
        }
        if (Last_MODBits_A != *apvts.getRawParameterValue("Bits_A_MOD"))
        {
            Last_MODBits_A = *apvts.getRawParameterValue("Bits_A_MOD");
            csg_voice->MODBits_A = Last_MODBits_A;
        }
        if (Last_MODPM_preamp != *apvts.getRawParameterValue("PM_Preamp_MOD"))
        {
            Last_MODPM_preamp = *apvts.getRawParameterValue("PM_Preamp_MOD");
            csg_voice->MODPM_preamp = Last_MODPM_preamp;
        }
        if (Last_MODPMsmooth != *apvts.getRawParameterValue("PM_Smooth_MOD"))
        {
            Last_MODPMsmooth = *apvts.getRawParameterValue("PM_Smooth_MOD");
            csg_voice->MODPMsmooth = Last_MODPMsmooth;
        }
        if (Last_MODSin2Cos != *apvts.getRawParameterValue("Sin2Cos_MOD"))
        {
            Last_MODSin2Cos = *apvts.getRawParameterValue("Sin2Cos_MOD");
            csg_voice->MODSin2Cos = Last_MODSin2Cos;
        }
        if (Last_MODBits_B != *apvts.getRawParameterValue("Bits_B_MOD"))
        {
            Last_MODBits_B = *apvts.getRawParameterValue("Bits_B_MOD");
            csg_voice->MODBits_B = Last_MODBits_B;
        }
        if (Last_MODCutoff != *apvts.getRawParameterValue("Cutoff_MOD"))
        {
            Last_MODCutoff = *apvts.getRawParameterValue("Cutoff_MOD");
            csg_voice->MODCutoff = Last_MODCutoff;
        }
        if (Last_MODResonance != *apvts.getRawParameterValue("Resonance_MOD"))
        {
            Last_MODResonance = *apvts.getRawParameterValue("Resonance_MOD");
            csg_voice->MODResonance = Last_MODResonance;
        }
        if (Last_lfoSpeed != *apvts.getRawParameterValue("LFO_Speed"))
        {
            Last_lfoSpeed = *apvts.getRawParameterValue("LFO_Speed");
            csg_voice->last_lfo_freq = Last_lfoSpeed;
        }
        if (Last_lfoWave != *apvts.getRawParameterValue("LFO_Wave"))
        {
            Last_lfoWave = *apvts.getRawParameterValue("LFO_Wave");
            csg_voice->last_lfo_wave = Last_lfoWave;
        }
        if (Last_FilterTypeLeft != *apvts.getRawParameterValue("Filter Type Left"))
        {
            Last_FilterTypeLeft = *apvts.getRawParameterValue("Filter Type Left");
            csg_voice->filterTypeL = Last_FilterTypeLeft;
        }
        if (Last_FilterTypeRight != *apvts.getRawParameterValue("Filter Type Right"))
        {
            Last_FilterTypeRight = *apvts.getRawParameterValue("Filter Type Right");
            csg_voice->filterTypeR = Last_FilterTypeRight;
        }
    }
    //======================================================================
    csg_synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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
	juce::ValueTree stateTree = apvts.copyState();
	
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
			apvts.replaceState(stateTree);
	}
}

juce::AudioProcessorValueTreeState &CsgAudioProcessor::getAPVTS(){
	return apvts;
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(){
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	return layout;
}

	
//==========================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CsgAudioProcessor();
}
