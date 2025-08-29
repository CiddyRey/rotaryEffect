/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RotaryEffectAudioProcessorEditor::RotaryEffectAudioProcessorEditor(RotaryEffectAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);

    // === Gain Knob ===
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gainSlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "gain", gainSlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

    // === Depth Knob ===
    depthSlider.setSliderStyle(juce::Slider::Rotary);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(depthSlider);
    depthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "depth", depthSlider);

    depthLabel.setText("Depth", juce::dontSendNotification);
    depthLabel.setJustificationType(juce::Justification::centred);
    depthLabel.attachToComponent(&depthSlider, false);
    addAndMakeVisible(depthLabel);

    // === Rate Knob ===
    rateSlider.setSliderStyle(juce::Slider::Rotary);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(rateSlider);
    rateSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "rate", rateSlider);

    rateLabel.setText("Rate", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification::centred);
    rateLabel.attachToComponent(&rateSlider, false);
    addAndMakeVisible(rateLabel);
}

RotaryEffectAudioProcessorEditor::~RotaryEffectAudioProcessorEditor()
{
}

//==============================================================================
void RotaryEffectAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void RotaryEffectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // Divide into 3 columns for the knobs
    auto knobWidth = area.getWidth() / 3;

    gainSlider.setBounds(area.removeFromLeft(knobWidth).reduced(20));
    depthSlider.setBounds(area.removeFromLeft(knobWidth).reduced(20));
    rateSlider.setBounds(area.removeFromLeft(knobWidth).reduced(20));
}

