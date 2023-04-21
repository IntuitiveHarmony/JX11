/*
  ==============================================================================

    Synth.cpp
    Created: 20 Apr 2023 8:16:24pm
    Author:  Jason Horst

  ==============================================================================
*/

#include "Synth.h"

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources(double sampleRate_, int /*samples per block*/)
{
    sampleRate = static_cast<float>(sampleRate_);
}

void Synth::deallocateResources()
{
    
}

void Synth::reset()
{
    
}

void Synth::render(float **outputBuffers, int sampleCount)
{
    
}

void Synth::midiMesage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    
}
