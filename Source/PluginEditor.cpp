/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor (JX11AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
}

//==============================================================================
void JX11AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!\n\nThis is one of my first steps\n into making Audio and MIDI plug ins\n\nAbove is the code that grabs each incoming 3 byte MIDI message from the host\n\nTo the right is data from my MIDI controller\nbeing captured as I play it live\n\nEach line represents information such as:\nnote on/off, note value, velocity, channel and timestamp\n\nThanks to Matthijs Hollemans and his book\nCode your Own Synth Plug-ins\n\nHow can I be learning so much\n and still have no idea what I'm doing? :p", getLocalBounds(), juce::Justification::centred, 1);
}

void JX11AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
