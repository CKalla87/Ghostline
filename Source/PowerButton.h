/*
  ==============================================================================

    PowerButton.h
    Power toggle button with image for GhostLine UI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class PowerButton : public juce::Button
{
public:
    PowerButton();

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;

private:
    juce::Image buttonImageOn;
    juce::Image buttonImageOff;
    static constexpr float cornerRadius = 16.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PowerButton)
};
