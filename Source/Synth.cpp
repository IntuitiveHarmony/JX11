/*
  ==============================================================================

    Synth.cpp
    Created: 20 Apr 2023 8:16:24pm
    Author:  Jason Horst

  ==============================================================================
*/

#include "Synth.h"
#include "Utils.h"

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

// Make sure the note and velocity are initilized to zero
void Synth::reset()
{
    voice.reset();
    noiseGen.reset();
}

// This is where the sound is made 
void Synth::render(float **outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
    // 1
    for (int sample = 0; sample < sampleCount; ++sample) {
        float noise = noiseGen.nextValue();
        
        float output = 0.0f;
        if (voice.note > 0) {
            output = voice.render();
        }
        
        outputBufferLeft[sample] = output;
        if (outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
    }
    
    protectYourEars(outputBufferLeft, sampleCount);
    protectYourEars(outputBufferRight, sampleCount);
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) {
        // note off, call the note off method
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
            
        // note on, call the note on method
        case 0x90: {
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note, velo);
            } else {
                noteOff(note);
            }
            break;
        }
    }
}

void Synth::noteOn(int note, int velocity)
{
    voice.note = note;
    voice.osc.amplitude = (velocity / 127.0f) * 0.5f;
    voice.osc.freq = 261.63f;
    voice.osc.sampleRate = sampleRate;
    voice.osc.phaseOffset = 0.0f;
    voice.osc.reset();
}

void Synth::noteOff(int note)
{
    if (voice.note == note) {
        voice.note = 0;
    }
}
