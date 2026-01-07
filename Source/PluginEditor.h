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
class GhostlineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GhostlineAudioProcessorEditor (GhostlineAudioProcessor&);
    ~GhostlineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void initializeBackground();
    GhostlineAudioProcessor& audioProcessor;

    // Spooky spacey theme colors
    juce::Colour spookyBlack = juce::Colour::fromFloatRGBA (0.05f, 0.03f, 0.08f, 1.0f);  // Dark purple-black
    juce::Colour ghostGreen = juce::Colour::fromFloatRGBA (0.2f, 0.9f, 0.4f, 1.0f);  // Eerie green
    juce::Colour ghostCyan = juce::Colour::fromFloatRGBA (0.0f, 0.8f, 0.9f, 1.0f);  // Spooky cyan
    juce::Colour ghostPurple = juce::Colour::fromFloatRGBA (0.6f, 0.2f, 0.8f, 1.0f);  // Mysterious purple
    juce::Colour ghostOrange = juce::Colour::fromFloatRGBA (1.0f, 0.4f, 0.1f, 1.0f);  // Eerie orange
    juce::Colour textColor = ghostCyan;

    // Controls
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider wetSlider;
    juce::Slider drySlider;
    juce::Slider modRateSlider;
    juce::Slider modDepthSlider;
    
    juce::Label delayTimeLabel;
    juce::Label feedbackLabel;
    juce::Label wetLabel;
    juce::Label dryLabel;
    juce::Label modRateLabel;
    juce::Label modDepthLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    // Cached background elements to prevent movement on repaint
    struct FogCircle
    {
        float x, y, radius;
    };
    struct OrbCircle
    {
        float x, y, radius;
    };
    std::vector<FogCircle> fogCircles;
    std::vector<OrbCircle> orbCircles;
    bool backgroundInitialized = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GhostlineAudioProcessorEditor)
};

