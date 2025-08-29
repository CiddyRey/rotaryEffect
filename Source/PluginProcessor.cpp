/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RotaryEffectAudioProcessor::RotaryEffectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

RotaryEffectAudioProcessor::~RotaryEffectAudioProcessor()
{
}

//==============================================================================
// Define parameters here
juce::AudioProcessorValueTreeState::ParameterLayout RotaryEffectAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain",   // parameter ID
        "Gain",   // parameter name
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f      // default
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "depth",
        "Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "rate",
        "Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f), // slower to faster sweep
        2.0f
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String RotaryEffectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RotaryEffectAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool RotaryEffectAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool RotaryEffectAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double RotaryEffectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RotaryEffectAudioProcessor::getNumPrograms() { return 1; }
int RotaryEffectAudioProcessor::getCurrentProgram() { return 0; }
void RotaryEffectAudioProcessor::setCurrentProgram(int) {}
const juce::String RotaryEffectAudioProcessor::getProgramName(int) { return {}; }
void RotaryEffectAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void RotaryEffectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void RotaryEffectAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RotaryEffectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void RotaryEffectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // === Parameter values ===
    auto gainValue = apvts.getRawParameterValue("gain")->load();
    auto depthValue = apvts.getRawParameterValue("depth")->load();
    auto rateValue = apvts.getRawParameterValue("rate")->load();

    // === LFO phase advance per sample ===
    float phaseIncrement = (2.0f * juce::MathConstants<float>::pi * rateValue) / getSampleRate();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Compute LFO (sine wave between -1 and 1)
            float lfo = std::sin(lfoPhase);

            // For stereo effect: invert phase between L and R
            float mod = (channel == 0) ? lfo : -lfo;

            // Depth controls modulation strength
            float modGain = gainValue * (1.0f - depthValue + depthValue * (0.5f * (1.0f + mod)));

            channelData[sample] *= modGain;

            // Advance LFO phase
            lfoPhase += phaseIncrement;
            if (lfoPhase >= juce::MathConstants<float>::twoPi)
                lfoPhase -= juce::MathConstants<float>::twoPi;
        }
    }

    // Depth & Rate can later be used for modulation (LFO for rotary effect)
}

//==============================================================================
bool RotaryEffectAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* RotaryEffectAudioProcessor::createEditor()
{
    return new RotaryEffectAudioProcessorEditor(*this);
}

//==============================================================================
void RotaryEffectAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RotaryEffectAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RotaryEffectAudioProcessor();
}

