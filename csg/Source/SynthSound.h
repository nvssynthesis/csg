/*
 ==============================================================================
 
 SynthSound.h
 Created: 28 Nov 2018 6:17:02pm
 Author:  Nicholas Solem
 
 ==============================================================================
 */
// Based entirely on TheAudioProgrammer's Juce Synthesizer tutorial.
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class CSGSound    :   public SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber)  override
    {
        return true;
    }
    bool appliesToChannel(int midiChannel)  override
    {
        return true;
    }
};

