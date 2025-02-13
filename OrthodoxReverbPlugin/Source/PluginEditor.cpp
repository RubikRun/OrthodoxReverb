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
    irSelector.addItem("IR 1", 1);
    irSelector.addItem("IR 2", 2);
    irSelector.addItem("IR 3", 3);
    irSelector.addItem("IR 4", 4);
    irSelector.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(irSelector);

    irSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "irSelection", irSelector);

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

    // Set the font and draw a label for the dropdown
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Select IR:", 10, 10, 200, 30, juce::Justification::left);
}

void OrthodoxReverbPluginAudioProcessorEditor::resized()
{
    irSelector.setBounds(10, 40, 200, 30);
}
