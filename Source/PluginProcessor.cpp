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
selfFM_AmountRange(0.f, 1.f),
memoryRange(0.f, 31.f),
PM_preamp_range(0.f, 8.f),
sin2cosRange(0.f, 1.f),
smoothingRange(20.f, 20000.f),
BitsRange(1.01f, 2048.f),
slewRange(0.1f, 10000.f),
droneRange(0.f, 1.f),
cutoffRange(20.f, 22000.f),
resRange(1.f, 13.f),
lfoSpeedRange(0.1f, 10.f),
lfoWaveRange(0.f, 3.f), // 4 waves
parameters(*this,
           nullptr,
           "PARAMETERS",
           {std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Self_FM", 1),
                "Self_FM",
                selfFM_AmountRange,
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Memory", 1),
                "Memory",
                memoryRange,
                1.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("FM_Smooth", 1),
                "FM_Smooth",
                smoothingRange,
                2000.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Bits_A", 1),
                "Bits_A",
                BitsRange,
                2048.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("PM_Preamp", 1),
                "PM_Preamp",
                PM_preamp_range,
                20000.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("PM_Smooth", 1),
                "PM_Smooth",
                smoothingRange,
                2000.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Sin2Cos", 1),
                "Sin2Cos",
                sin2cosRange,
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Bits_B", 1),
                "Bits_B",
                BitsRange,
                2048.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Drone", 1),
                "Drone",
                droneRange,
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Rise", 1),
                "Rise",
                slewRange,
                0.01f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Fall", 1),
                "Fall",
                slewRange,
                0.01f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Cutoff", 1),
                "Cutoff",
                cutoffRange,
                20000.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Resonance", 1),
                "Resonance",
                resRange,
                1.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("LFO_Speed", 1),
                "LFO_Speed",
                lfoSpeedRange,
                1.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("LFO_Wave", 1),
                "LFO_Wave",
                lfoWaveRange,
                0.f),
       //===========LFO MODULATION AMOUNTS===========================
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Self_FM_MOD", 1),
                "Self_FM_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Memory_MOD", 1),
                "Memory_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("FM_Smooth_MOD", 1),
                "FM_Smooth_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Bits_A_MOD", 1),
                "Bits_A_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("PM_Preamp_MOD", 1),
                "PM_Preamp_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("PM_Smooth_MOD", 1),
                "PM_Smooth_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Sin2Cos_MOD", 1),
                "Sin2Cos_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Bits_B_MOD", 1),
                "Bits_B_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Cutoff_MOD", 1),
                "Cutoff_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterFloat>
               (juce::ParameterID("Resonance_MOD", 1),
                "Resonance_MOD",
                selfFM_AmountRange, // using just because it's [0..1]
                0.f),
           std::make_unique<AudioParameterInt>
               (juce::ParameterID("Filter Type Left", 1),
                "Filter Type Left",
                0, 3, 0),
           std::make_unique<AudioParameterInt>
               (juce::ParameterID("Filter Type Right", 1),
                "Filter Type Right",
                0, 3, 0)
           }
           )
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
        if (Last_Self_FM != *parameters.getRawParameterValue("Self_FM"))
        {
            Last_Self_FM = *parameters.getRawParameterValue("Self_FM");
            csg_voice->unit.selfFM_target = Last_Self_FM;
        }
        if (Last_Memory != *parameters.getRawParameterValue("Memory"))
        {
            Last_Memory = *parameters.getRawParameterValue("Memory");
            csg_voice->unit.memory_target = Last_Memory;
        }
        if (Last_FM_Smooth != *parameters.getRawParameterValue("FM_Smooth"))
        {
            Last_FM_Smooth = *parameters.getRawParameterValue("FM_Smooth");
            csg_voice->unit.FM_smooth_target = Last_FM_Smooth;
        }
        if (Last_Bits_A != *parameters.getRawParameterValue("Bits_A"))
        {
            Last_Bits_A = *parameters.getRawParameterValue("Bits_A");
            csg_voice->unit.bits1_target = Last_Bits_A;
        }
        if (Last_PM_Preamp != *parameters.getRawParameterValue("PM_Preamp"))
        {
            Last_PM_Preamp = *parameters.getRawParameterValue("PM_Preamp");
            csg_voice->unit.PM_preamp_target = Last_PM_Preamp;
        }
        if (Last_PM_Smooth != *parameters.getRawParameterValue("PM_Smooth"))
        {
            Last_PM_Smooth = *parameters.getRawParameterValue("PM_Smooth");
            csg_voice->unit.PM_smooth_target = Last_PM_Smooth;
        }
        if (Last_Sin2Cos != *parameters.getRawParameterValue("Sin2Cos"))
        {
            Last_Sin2Cos = *parameters.getRawParameterValue("Sin2Cos");
            csg_voice->unit.PM_sin2cos_target = Last_Sin2Cos;
        }
        if (Last_Bits_B != *parameters.getRawParameterValue("Bits_B"))
        {
            Last_Bits_B = *parameters.getRawParameterValue("Bits_B");
            csg_voice->unit.bits2_target = Last_Bits_B;
        }
        if (Last_Drone != *parameters.getRawParameterValue("Drone"))
        {
            Last_Drone = *parameters.getRawParameterValue("Drone");
            csg_voice->voice_drone = Last_Drone;
        }
        if (Last_Rise != *parameters.getRawParameterValue("Rise"))
        {
            Last_Rise = *parameters.getRawParameterValue("Rise");
            csg_voice->voice_rise = Last_Rise;
        }
        if (Last_Fall != *parameters.getRawParameterValue("Fall"))
        {
            Last_Fall = *parameters.getRawParameterValue("Fall");
            csg_voice->voice_fall = Last_Fall;
        }
        if (Last_Cutoff != *parameters.getRawParameterValue("Cutoff"))
        {
            Last_Cutoff = *parameters.getRawParameterValue("Cutoff");
            csg_voice->voice_cutoff = Last_Cutoff;
        }
        if (Last_Resonance != *parameters.getRawParameterValue("Resonance"))
        {
            Last_Resonance = *parameters.getRawParameterValue("Resonance");
            csg_voice->voice_res = Last_Resonance;
        }
        if (Last_MODselfFM != *parameters.getRawParameterValue("Self_FM_MOD"))
        {
            Last_MODselfFM = *parameters.getRawParameterValue("Self_FM_MOD");
            csg_voice->MODselfFM = Last_MODselfFM;
        }
        if (Last_MODMemory != *parameters.getRawParameterValue("Memory_MOD"))
        {
            Last_MODMemory = *parameters.getRawParameterValue("Memory_MOD");
            csg_voice->MODMemory = Last_MODMemory;
        }
        if (Last_MODFMsmooth != *parameters.getRawParameterValue("FM_Smooth_MOD"))
        {
            Last_MODFMsmooth = *parameters.getRawParameterValue("FM_Smooth_MOD");
            csg_voice->MODFMsmooth = Last_MODFMsmooth;
        }
        if (Last_MODBits_A != *parameters.getRawParameterValue("Bits_A_MOD"))
        {
            Last_MODBits_A = *parameters.getRawParameterValue("Bits_A_MOD");
            csg_voice->MODBits_A = Last_MODBits_A;
        }
        if (Last_MODPM_preamp != *parameters.getRawParameterValue("PM_Preamp_MOD"))
        {
            Last_MODPM_preamp = *parameters.getRawParameterValue("PM_Preamp_MOD");
            csg_voice->MODPM_preamp = Last_MODPM_preamp;
        }
        if (Last_MODPMsmooth != *parameters.getRawParameterValue("PM_Smooth_MOD"))
        {
            Last_MODPMsmooth = *parameters.getRawParameterValue("PM_Smooth_MOD");
            csg_voice->MODPMsmooth = Last_MODPMsmooth;
        }
        if (Last_MODSin2Cos != *parameters.getRawParameterValue("Sin2Cos_MOD"))
        {
            Last_MODSin2Cos = *parameters.getRawParameterValue("Sin2Cos_MOD");
            csg_voice->MODSin2Cos = Last_MODSin2Cos;
        }
        if (Last_MODBits_B != *parameters.getRawParameterValue("Bits_B_MOD"))
        {
            Last_MODBits_B = *parameters.getRawParameterValue("Bits_B_MOD");
            csg_voice->MODBits_B = Last_MODBits_B;
        }
        if (Last_MODCutoff != *parameters.getRawParameterValue("Cutoff_MOD"))
        {
            Last_MODCutoff = *parameters.getRawParameterValue("Cutoff_MOD");
            csg_voice->MODCutoff = Last_MODCutoff;
        }
        if (Last_MODResonance != *parameters.getRawParameterValue("Resonance_MOD"))
        {
            Last_MODResonance = *parameters.getRawParameterValue("Resonance_MOD");
            csg_voice->MODResonance = Last_MODResonance;
        }
        if (Last_lfoSpeed != *parameters.getRawParameterValue("LFO_Speed"))
        {
            Last_lfoSpeed = *parameters.getRawParameterValue("LFO_Speed");
            csg_voice->last_lfo_freq = Last_lfoSpeed;
        }
        if (Last_lfoWave != *parameters.getRawParameterValue("LFO_Wave"))
        {
            Last_lfoWave = *parameters.getRawParameterValue("LFO_Wave");
            csg_voice->last_lfo_wave = Last_lfoWave;
        }
        if (Last_FilterTypeLeft != *parameters.getRawParameterValue("Filter Type Left"))
        {
            Last_FilterTypeLeft = *parameters.getRawParameterValue("Filter Type Left");
            csg_voice->filterTypeL = Last_FilterTypeLeft;
        }
        if (Last_FilterTypeRight != *parameters.getRawParameterValue("Filter Type Right"))
        {
            Last_FilterTypeRight = *parameters.getRawParameterValue("Filter Type Right");
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
	juce::ValueTree stateTree = parameters.copyState();
	
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
			parameters.replaceState(stateTree);
	}
}

//==========================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CsgAudioProcessor();
}
