/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModernKnobComponent.h"
#include "SpectrumDisplayComponent.h"
#include "PowerButton.h"

//==============================================================================
class GhostlineAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    GhostlineAudioProcessorEditor (GhostlineAudioProcessor&);
    ~GhostlineAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GhostlineAudioProcessor& audioProcessor;

    // Layout constants
    static constexpr int editorWidth = 1200;
    static constexpr int editorHeight = 720;
    static constexpr int outerPadding = 28;
    static constexpr int mainRadius = 24;
    static constexpr int headerPaddingH = 32;
    static constexpr int headerPaddingV = 22;
    static constexpr int displayHeight = 300;
    static constexpr int displayMarginTop = 18;
    static constexpr int knobGap = 36;
    static constexpr int knobUnitWidth = 140;

    // Theme colours
    juce::Colour colourBlue   { 0xff60A5FA };
    juce::Colour colourGreen  { 0xff34D399 };
    juce::Colour colourPurple { 0xffA78BFA };

    PowerButton powerButton;
    SpectrumDisplayComponent spectrumDisplay;
    std::array<ModernKnobComponent*, 6> knobs {};
    ModernKnobComponent delayTimeKnob, feedbackKnob, wetKnob, dryKnob, modRateKnob, modDepthKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    void drawGradientTitle (juce::Graphics& g, juce::Rectangle<int> area);
    void drawBackgroundAndContainer (juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GhostlineAudioProcessorEditor)
};
