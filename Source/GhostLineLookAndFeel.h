/*
  ==============================================================================

    GhostLineLookAndFeel.h
    Custom LookAndFeel for GhostLine Space Echo Delay UI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class GhostLineLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GhostLineLookAndFeel();
    
    void drawRotarySlider (juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;
    
    void setThemeColour (juce::Colour colour) { themeColour = colour; }
    juce::Colour getThemeColour() const { return themeColour; }

private:
    juce::Colour themeColour;
    
    static constexpr float startAngleDegrees = -135.0f;
    static constexpr float sweepAngleDegrees = 270.0f;
    static constexpr float trackStrokeWidth = 3.0f;
    static constexpr float knobFaceDiameter = 96.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GhostLineLookAndFeel)
};
