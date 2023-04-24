/*
  ==============================================================================

    Voice.h
    Created: 20 Apr 2023 8:15:16pm
    Author:  Jason Horst

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

struct Voice
{
    float saw;
    int note;
    Oscillator osc;
    
    // Make sure the note and velocity are initilized to zero
    void reset()
    {
        saw = 0;
        note = 0;
        osc.reset();
    }
    
    float render()
    {
        float sample= osc.nextSample();
        saw = saw * 0.997f + sample;
        return saw;
    }
};
