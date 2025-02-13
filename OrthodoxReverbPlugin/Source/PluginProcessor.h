/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
*/
class OrthodoxReverbPluginAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    OrthodoxReverbPluginAudioProcessor();
    ~OrthodoxReverbPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void OrthodoxReverbPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    juce::dsp::Convolution convolutionProcessor;

    const std::vector<juce::File> irFiles = {
        juce::File("C:/dev/OrthodoxReverb/examples/ir/Ampeg Classic B5 Left A 230 200 320.wav"),
        juce::File("C:/dev/OrthodoxReverb/examples/ir/Randall RT412 SM57 A 3 0 2.wav"),
        juce::File("C:/dev/OrthodoxReverb/examples/ir/Rocksta Reactions Fender Twin Reverb SM57 A 2 3 3 45.wav"),
        juce::File("C:/dev/OrthodoxReverb/examples/ir/Marshall 1960VB SM57 A -2 0 0 45.wav")
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrthodoxReverbPluginAudioProcessor)
};
