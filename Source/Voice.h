/*
  ==============================================================================

    Voice.h
    Created: 20 Apr 2023 8:15:16pm
    Author:  Jason Horst

  ==============================================================================
*/

#pragma once

struct Voice
{
    int note;
    int velocity;
    
    // Make sure the note and velocity are initilized to zero
    void reset()
    {
        note = 0;
        velocity = 0;
    }
};
