/*
  ==============================================================================

    Oscillator.h
    Created: 21 Apr 2023 10:07:23pm
    Author:  Jason Horst

  ==============================================================================
*/

#pragma once

const float PI_OVER_4 = 0.7853981633974483f;
const float PI = 3.1415926535897932f;
const float TWO_PI = 6.2831853071795864f;

class Oscillator
{
public:
    float period = 0.0f;
    float amplitude = 1.0f;
    
    void reset()
    {
        inc = 0.0f;
        phase = 0.0f;
        sin1 = 0.0f;
        dsin = 0.0f;
    }
    
    float nextSample()
    {
        float output = 0.0f;
        
        phase += inc;   // Update the phase. In the first half of the sinc function, inc is positive and phase is incremented.
                        // When drawing the second half, inc is negative and phase is decremented.
        
        if (phase <= PI_OVER_4) { // start new Impulse. The oscillator enters this if statement once per period, and also immediately when a new note starts.
            
            float halfPeriod = period / 2.0f; // Find where the midpoint will be between the peak that was just finished and the next one.
            phaseMax = std::floor(0.5 + halfPeriod) - 0.5f; // The phaseMax variable holds the position of the midpoint between the two impulse peaks.  Floor reduces aliasing
            
            inc = phaseMax / halfPeriod;
            phase = -phase;
            
            
            sin0 = amplitude * std::sin(phase);
            sin1 = amplitude * std::sin(phase - inc);
            dsin = 2.0f * std::cos(inc);
            
            if (phase*phase > 1e-9) { // Square the phase to make sure the number is large enough and take remove negative values (second half of phase)
                output = sin0 / phase;
            } else {
                output = amplitude;
            }
        } else { // The current sample is somewhere between the previous peak and the next. This is where the oscillator spends most of its time.
            if (phase > phaseMax) { // Phase counter goes past the half-way point, set phase to the maximum and invert the increment inc, so that now the oscillator will begin outputting the sinc function backwards
                phase = phaseMax + phaseMax - phase;
            }
            float sinp = dsin * sin0 - sin1;
            sin1 = sin0;
            sin0 = sinp;
            
            output = sinp / phase;
        }
        return output;
        
    }
    
    
private:
    float inc;
    float phase;
    float phaseMax;
    float sin0;
    float sin1;
    float dsin;
};
