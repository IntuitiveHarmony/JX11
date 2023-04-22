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
    int note;
    Oscillator osc;
    
    // Make sure the note and velocity are initilized to zero
    void reset()
    {
        note = 0;
        osc.reset();
    }
    
    float render()
    {
        return osc.nextSample();
    }
};
