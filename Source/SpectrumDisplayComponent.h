/*
  ==============================================================================

    SpectrumDisplayComponent.h
    Display panel with spectrum analyser for GhostLine UI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class SpectrumDisplayComponent : public juce::Component,
                                 public juce::Timer
{
public:
    explicit SpectrumDisplayComponent (GhostlineAudioProcessor& proc);
    
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

private:
    void drawNextFrameOfSpectrum();

    GhostlineAudioProcessor& processor;

    enum {
        fftOrder = 11,
        fftSize = 1 << fftOrder,
        scopeSize = 512
    };

    juce::dsp::FFT forwardFFT { fftOrder };
    juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::vector<float> fftData;
    std::vector<float> scopeData;

    static constexpr float cornerRadius = 16.0f;
    static constexpr int gridLineCount = 10;

    juce::Colour spectrumColour { 0xff60A5FA };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDisplayComponent)
};
