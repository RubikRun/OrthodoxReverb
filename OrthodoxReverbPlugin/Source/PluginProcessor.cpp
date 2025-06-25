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
    , parameters(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterInt>("irSelection", "IR Selection", 0, 3, 0),
            std::make_unique<juce::AudioParameterFloat>("blend", "Blend", 0.0f, 100.0f, 70.0f),
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 150.0f, 100.0f),
            std::make_unique<juce::AudioParameterFloat>("roomSize", "Room Size", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f)
        })
#endif
{
    parameters.addParameterListener("irSelection", this);
    parameters.addParameterListener("blend", this);
    parameters.addParameterListener("gain", this);
    parameters.addParameterListener("roomSize", this);
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

    // Load IR file
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    // Create a memory stream from the BinaryData of IR file
    std::unique_ptr<juce::InputStream> inputStream = std::make_unique<juce::MemoryInputStream>( irFiles[0], irFilesSizes[0], false);
    // Use format manager to create reader
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(inputStream)));
    if (reader != nullptr)
    {
        this->originalIR.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
        reader->read(&originalIR, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
    }

    // call convolution.loadimpulseResponse() on the processed IR
    updateProcessedIR(*parameters.getRawParameterValue("roomSize"), getSampleRate());

    smoothedBlend.reset(sampleRate, 0.05); // Smooth over 50ms
    smoothedBlend.setCurrentAndTargetValue(1.0f); // default to 100% wet

    smoothedGain.reset(sampleRate, 0.05); // Smooth over 50ms
    smoothedGain.setCurrentAndTargetValue(1.0f); // default 100% = neutral gain
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

void OrthodoxReverbPluginAudioProcessor::updateProcessedIR(float roomSize, double sampleRate)
{
    if (originalIR.getNumSamples() == 0)
    {
        return;
    }

    int numChannels = originalIR.getNumChannels();
    int originalNumSamples = originalIR.getNumSamples();

    // 1. Compute pre-delay samples
    int preDelaySamples = static_cast<int>((roomSize * maxPreDelayMs / 1000.0f) * sampleRate);

    // 2. Determine early/tail split point
    int tailStartSample = static_cast<int>((tailSplitMs / 1000.0f) * sampleRate);
    tailStartSample = std::min(tailStartSample, originalNumSamples); // Clamp

    // 3. Compute tail length
    int tailLength = originalNumSamples - tailStartSample;
    if (tailLength <= 0) tailLength = 1;

    // 4. Compute how many times to repeat tail
    int tailRepeats = 1 + static_cast<int>(roomSize * (maxTailCopies - 1));

    // 5. Total length of new IR
    int newNumSamples = preDelaySamples + tailStartSample + tailLength * tailRepeats;

    juce::AudioBuffer<float> stretchedIR(numChannels, newNumSamples);
    stretchedIR.clear();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        // A. Insert pre-delay
        int writePos = preDelaySamples;

        // B. Copy early reflections
        stretchedIR.copyFrom(ch, writePos, originalIR.getReadPointer(ch), tailStartSample);
        writePos += tailStartSample;

        // C. Copy & fade tail multiple times
        for (int r = 0; r < tailRepeats; ++r)
        {
            float fade = 1.0f - (float)r / float(tailRepeats); // Linear fade
            for (int s = 0; s < tailLength; ++s)
            {
                float tailSample = originalIR.getSample(ch, tailStartSample + s);
                stretchedIR.addSample(ch, writePos + s, tailSample * fade);
            }
            writePos += tailLength;
        }
    }

    // 6. Load into convolution
    convolutionProcessor.loadImpulseResponse(std::move(stretchedIR),
        sampleRate,
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no,
        juce::dsp::Convolution::Normalise::yes);
}

void OrthodoxReverbPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Save the original dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Update the IR file only when the slider is moved(doing it trough parameterChanged is slow and laggy)
    float roomSize = *parameters.getRawParameterValue("roomSize");
    if (roomSize != previousRoomSize)
    {
        updateProcessedIR(roomSize, getSampleRate());
        previousRoomSize = roomSize;
    }

    // Apply convolution to wet buffer
    juce::dsp::AudioBlock<float> wetBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);

    convolutionProcessor.process(wetContext);

    // End convolution


    // Apply the blend and gain
    const float targetBlend = parameters.getRawParameterValue("blend")->load() / 100.0f;
    smoothedBlend.setTargetValue(targetBlend);

    const float knobGainNorm = parameters.getRawParameterValue("gain")->load() / 100.0f;
    // Apply a power function (x^3) to the gain because human hearing is logarithmic.
    // (a linear gain sounds awkward - changes on the lower end are very noticable compared to changes on the higher end)
    // (it'd be more accurate to apply an exponential function but it's more effecient with a power function, and sounds basically the same)
    const float targetGain = knobGainNorm * knobGainNorm * knobGainNorm;
    smoothedGain.setTargetValue(targetGain);

    // Blend dry and wet buffers
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* output = buffer.getWritePointer(channel);
        const float* dryData = dryBuffer.getReadPointer(channel);
        const float* wetData = buffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float gain = smoothedGain.getNextValue();
            const float wetMix = smoothedBlend.getNextValue();
            const float dryMix = 1.0f - wetMix;
            output[sample] = ((wetMix * wetData[sample]) + (dryMix * dryData[sample])) * gain;
        }
    }
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
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void OrthodoxReverbPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        parameters.replaceState(tree);
}

void OrthodoxReverbPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "irSelection")
    {
        const int selectedIR = static_cast<int>(newValue);
        // Load the IR into a buffer
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(juce::File(irFiles[selectedIR])));
        if (reader)
        {
            this->originalIR.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
            reader->read(&originalIR, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        }
        // call convolution.loadimpulseResponse() on the processed IR
        updateProcessedIR(*parameters.getRawParameterValue("roomSize"), getSampleRate());
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrthodoxReverbPluginAudioProcessor();
}
