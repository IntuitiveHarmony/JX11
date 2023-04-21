/*
  ==============================================================================

    Synth.h
    Created: 20 Apr 2023 8:16:24pm
    Author:  Jason Horst

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"

// we made a mirror of JX11AudioProcessor in a way
// it has similar methods to: reset state, render the current block and handle MIDI
class Synth
{
public:
    Synth();
    
    // "prepareToPlay"
    void allocateResources(double sampleRate, int samplesPerBlock);
    // "releaseResourses"
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);

private:
    float sampleRate;
    Voice voice;
    
    void noteOn(int note, int velocity);
    void noteOff(int note);
    
};
