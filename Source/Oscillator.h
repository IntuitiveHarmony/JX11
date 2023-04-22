/*
  ==============================================================================

    Oscillator.h
    Created: 21 Apr 2023 10:07:23pm
    Author:  Jason Horst

  ==============================================================================
*/

#pragma once

const float TWO_PI = 6.2831853071795864f;

class Oscillator
{
public:
    float amplitude;
    float freq;
    float sampleRate;
    float phaseOffset;
    int sampleIndex;
    
    void reset()
    {
        sampleIndex = 0;
    }
    
    float nextSample()
    {
        // This is the formula for a sine wave  =>  amplitude * sin(2π * time * frequency / sampleRate + phase)
        // with the sampleIndex put in for time variable.
        float output = amplitude * std::sin(TWO_PI * sampleIndex * freq / sampleRate + phaseOffset);
        
        sampleIndex += 1;
        return output;
    }
};
