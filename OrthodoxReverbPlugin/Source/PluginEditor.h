/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include <memory>

//==============================================================================
/**
*/
class OrthodoxReverbPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OrthodoxReverbPluginAudioProcessorEditor (OrthodoxReverbPluginAudioProcessor&);
    ~OrthodoxReverbPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OrthodoxReverbPluginAudioProcessor& audioProcessor;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> irSelectorAttachment;

    juce::ComboBox irSelector;

    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    juce::Slider roomSizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrthodoxReverbPluginAudioProcessorEditor)
};
