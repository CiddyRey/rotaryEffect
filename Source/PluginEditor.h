/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RotaryEffectAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    RotaryEffectAudioProcessorEditor(RotaryEffectAudioProcessor&);
    ~RotaryEffectAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    RotaryEffectAudioProcessor& audioProcessor;

    // === Sliders (rotary knobs) ===
    juce::Slider gainSlider;
    juce::Slider depthSlider;
    juce::Slider rateSlider;

    // === Labels ===
    juce::Label gainLabel;
    juce::Label depthLabel;
    juce::Label rateLabel;

    // === Attachments ===
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryEffectAudioProcessorEditor)
};

