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

    const std::vector<const char*> irFiles = {
        BinaryData::Ampeg_Classic_B5_Left_A_230_200_320_wav,
        BinaryData::Randall_RT412_SM57_A_3_0_2_wav,
        BinaryData::Rocksta_Reactions_Fender_Twin_Reverb_SM57_A_2_3_3_45_wav,
        BinaryData::Marshall_1960VB_SM57_A_2_0_0_45_wav
    };

    const std::vector<int> irFilesSizes = {
        BinaryData::Ampeg_Classic_B5_Left_A_230_200_320_wavSize,
        BinaryData::Randall_RT412_SM57_A_3_0_2_wavSize,
        BinaryData::Rocksta_Reactions_Fender_Twin_Reverb_SM57_A_2_3_3_45_wavSize,
        BinaryData::Marshall_1960VB_SM57_A_2_0_0_45_wavSize
    };

    juce::SmoothedValue<float> smoothedBlend;
    juce::SmoothedValue<float> smoothedGain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrthodoxReverbPluginAudioProcessor)
};
