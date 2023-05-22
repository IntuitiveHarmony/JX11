/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11AudioProcessor::JX11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

JX11AudioProcessor::~JX11AudioProcessor()
{
}

//==============================================================================
const juce::String JX11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JX11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JX11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JX11AudioProcessor::getProgramName (int index)
{
    return {};
}

void JX11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JX11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

void JX11AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    synth.deallocateResources();
}

void JX11AudioProcessor::reset()
{
    synth.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JX11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// ~~ The process block is given two objects, one contains the audio and the other, midi mesages
void JX11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // ~~ This keeps really small floats from becoming a `denormal` data type
    // ~~ Denormals are slower and so small we just treat them as `0`
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Read noise parameters
    const juce::String& paramID = ParameterID::noise.getParamID();
    float noiseMix = apvts.getRawParameterValue(paramID)->load() / 100.0f;
    noiseMix *= noiseMix;
    synth.noiseMix = noiseMix * 0.06f;
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // ~~ 70
    splitBufferByEvents(buffer, midiMessages);
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

// ~~ This is where we handle the MIDI input for our plugin ~~
void JX11AudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    // pass midi messsage to the synth class
    synth.midiMessage(data0, data1, data2);
    
    // This will print the midi messages to the console
    char s[16];
    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    DBG(s);
    // print the raw values of data0, data1, and data2
    // printf("data0 = %u, data1 = %u, data2 = %u\n", data0, data1, data2);
    // or use the DBG() macro
    DBG("data0 = " << data0 << ", data1 = " << data1 << ", data2 = " << data2);
}

void JX11AudioProcessor::render(
juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    float* outputBuffers[2] = { nullptr, nullptr };
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

// ~68 define splitBufferByEvents~
void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    
    int bufferOffset = 0; // Initialize a variable to keep track of the current position in the audio buffer
    
    for (const auto metadata : midiMessages) { // Iterate over each MIDI event in the midiMessages buffer
        // render the audio that happens before this event (if any)
        // Calculate the number of audio samples between the current MIDI event
        // and the previous one (or the beginning of the buffer, if this is the first event)
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) { // If there are audio samples between the previous MIDI event and the current one
            // Call the render() function to process the audio samples in this segment
            render(buffer, samplesThisSegment, bufferOffset);
            // Update the bufferOffset variable to reflect the current position in the audio buffer
            bufferOffset += samplesThisSegment;
        }
        
        // handle the event. Ignore MIDI messages such as sysex
        // If this is a valid MIDI message (i.e., its size is 1, 2, or 3 bytes)
        if (metadata.numBytes <= 3) { // If this is a valid MIDI message (i.e., its size is 1, 2, or 3 bytes)
            // Extract the first data byte (if present) from the MIDI message
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            // Extract the second data byte (if present) from the MIDI message
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            // Call the handleMIDI() function to process the MIDI event
            handleMIDI(metadata.data[0], data1, data2);
        }
    }
    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    // Calculate the number of audio samples remaining in the buffer after the last MIDI event
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) { // If there are any remaining audio samples
        render(buffer, samplesLastSegment, bufferOffset);
    }
    midiMessages.clear();
}



//==============================================================================
bool JX11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11AudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 1050);
    return editor;
}

//==============================================================================
void JX11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JX11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//==============================================================================

// ~~ This will instansiate and define the parameters (user choices) layout for the synth
juce::AudioProcessorValueTreeState::ParameterLayout JX11AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Helpers used to construct the APVTS for the parameters:
    layout.add(std::make_unique<juce::AudioParameterChoice>( // Parameter Choice has 4 arguments
                                                            ParameterID::polyMode, // Parameter Identifier
                                                            "Polyphony", // Human readable name for the DAW
                                                            juce::StringArray { "Mono", "Poly" }, // Choices for the parameter
                                                            1 // Index of default choice
                                                            ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>( // Parameter Float has 5 arguments
                                                            ParameterID::oscTune,
                                                            "Osc Tune",
                                                            juce::NormalisableRange<float>(-24.0, 24.0f, 1.0f),
                                                            -12.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("semi") // label for the parameter
                                                            ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::oscFine,
                                                            "Osc Fine",
                                                            juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3, true), // .3 is a skew that makes the center of the slider "wider" giving uer more definition closer to 0
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("cent")
                                                            ));
    
    // This allows us to display something more custom in the UI
    auto oscMixStringFromValue = [](float value, int)
    {
        char s[16] = { 0 };
        snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
        return juce::String(s);
    };
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           ParameterID::oscMix,
                                                           "Osc Mix",
                                                           juce::NormalisableRange<float>(0.0f, 100.0f), // no 'step' value
                                                           0.0f,
                                                           juce::AudioParameterFloatAttributes()
                                                                .withLabel("%")
                                                                .withStringFromValueFunction(oscMixStringFromValue) // custom output to UI
                                                           ));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
                                                            ParameterID::glideMode,
                                                            "Glide Mode",
                                                            juce::StringArray { "Off", "Legato", "Always" },
                                                            0
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::glideRate,
                                                            "Glide Rate",
                                                            juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
                                                            35.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::glideBend,
                                                            "Glide Bend",
                                                            juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("semi")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterFreq,
                                                            "Filter Freq",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                                                            100.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterReso,
                                                            "Filter Reso",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            15.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterEnv,
                                                            "Filter Env",
                                                            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                            50.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterLFO,
                                                            "Filter LFO",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    auto filterVelocityStringFromValue = [](float value, int)
    {
        if (value < -90.0f)
            return juce::String("OFF");
        else
            return juce::String(value);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterVelocity,
                                                            "Velocity",
                                                            juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes()
                                                                .withLabel("%")
                                                                .withStringFromValueFunction(filterVelocityStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterAttack,
                                                            "Filter Attack",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterDecay,
                                                            "Filter Decay",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            30.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterSustain,
                                                            "Filter Sustain",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::filterRelease,
                                                            "Filter Release",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            25.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::envAttack,
                                                            "Env Attack",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::envDecay,
                                                            "Env Decay",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            50.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::envSustain,
                                                            "Env Sustain",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            100.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::envRelease,
                                                            "Env Release",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            30.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    auto lfoRateStringFromValue = [](float value, int)
    {
        float lfoHz = std::exp(7.0f * value - 4.0f);
        return juce::String(lfoHz, 3);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::lfoRate,
                                                            "LFO Rate",
                                                            juce::NormalisableRange<float>(),
                                                            0.81f,
                                                            juce::AudioParameterFloatAttributes()
                                                                .withLabel("Hz")
                                                                .withStringFromValueFunction(lfoRateStringFromValue)
                                                            ));

    auto vibratoStringFromValue = [](float value, int)
    {
        if (value < 0.0f)
            return "PWM " + juce::String(-value, 1);
        else
            return juce::String(value, 1);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::vibrato,
                                                            "Vibrato",
                                                            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes()
                                                                .withLabel("%")
                                                                .withStringFromValueFunction(vibratoStringFromValue)
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::noise,
                                                            "Noise",
                                                            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("%")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::octave,
                                                            "Octave",
                                                            juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
                                                            0.0f
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::tuning,
                                                            "Tuning",
                                                            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("cent")
                                                            ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                            ParameterID::outputLevel,
                                                            "Output Level",
                                                            juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
                                                            0.0f,
                                                            juce::AudioParameterFloatAttributes().withLabel("dB")
                                                            ));
    
    return layout;
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}

