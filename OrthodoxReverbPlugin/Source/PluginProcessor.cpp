/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OrthodoxReverbPluginAudioProcessor::OrthodoxReverbPluginAudioProcessor()
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

OrthodoxReverbPluginAudioProcessor::~OrthodoxReverbPluginAudioProcessor()
{
}

//==============================================================================
const juce::String OrthodoxReverbPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrthodoxReverbPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OrthodoxReverbPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OrthodoxReverbPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OrthodoxReverbPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrthodoxReverbPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OrthodoxReverbPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrthodoxReverbPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OrthodoxReverbPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void OrthodoxReverbPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OrthodoxReverbPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    convolutionProcessor.prepare(spec);

    // Load an IR file
    juce::File irFile("C:/dev/OrthodoxReverb/examples/Impulse-Response-Deian-2.wav");
    if (irFile.existsAsFile()) {
        convolutionProcessor.loadImpulseResponse(irFile,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::yes,
            1024);
    }
}

void OrthodoxReverbPluginAudioProcessor::releaseResources()
{
    convolutionProcessor.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrthodoxReverbPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void OrthodoxReverbPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    convolutionProcessor.process(context);
}

//==============================================================================
bool OrthodoxReverbPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OrthodoxReverbPluginAudioProcessor::createEditor()
{
    return new OrthodoxReverbPluginAudioProcessorEditor (*this);
}

//==============================================================================
void OrthodoxReverbPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OrthodoxReverbPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrthodoxReverbPluginAudioProcessor();
}
