/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrthodoxReverbPluginAudioProcessorEditor::OrthodoxReverbPluginAudioProcessorEditor (OrthodoxReverbPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // --- SETUP IR SELECTOR ---
    irSelector.addItem("IR 1", 1);
    irSelector.addItem("IR 2", 2);
    irSelector.addItem("IR 3", 3);
    irSelector.addItem("IR 4", 4);
    irSelector.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(irSelector);

    irSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "irSelection", irSelector);

    // --- SETUP MIX KNOB ---
    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    mixSlider.setRange(0.0, 100.0, 1.0);
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    addAndMakeVisible(mixSlider);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "blend", mixSlider
    );

    // --- SETUP GAIN KNOB ---
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider.setRange(0.0, 150.0, 1.0);
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    addAndMakeVisible(gainSlider);

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "gain", gainSlider
    );

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    repaint();
}

OrthodoxReverbPluginAudioProcessorEditor::~OrthodoxReverbPluginAudioProcessorEditor()
{
}

//==============================================================================
void OrthodoxReverbPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill the background with a dark grey color
    g.fillAll(juce::Colours::darkgrey);

    // Set the font and draw labels
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Select IR:", 10, 10, 200, 30, juce::Justification::left);
    g.drawText("Mix:", 250, 10, 100, 20, juce::Justification::centred);
    g.drawText("Gain:", 10, 130, 100, 20, juce::Justification::centred);
}

void OrthodoxReverbPluginAudioProcessorEditor::resized()
{
    irSelector.setBounds(10, 40, 200, 30);
    mixSlider.setBounds(250, 30, 100, 100);
    gainSlider.setBounds(10, 150, 100, 100);
}
