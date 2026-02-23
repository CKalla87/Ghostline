/*
  ==============================================================================

    SpectrumDisplayComponent.h
    Display panel with grid lines for GhostLine UI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class SpectrumDisplayComponent : public juce::Component
{
public:
    SpectrumDisplayComponent();
    
    void paint (juce::Graphics& g) override;

private:
    static constexpr float cornerRadius = 16.0f;
    static constexpr int gridLineCount = 10;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDisplayComponent)
};
