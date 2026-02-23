/*
  ==============================================================================

    ModernKnobComponent.h
    Custom knob with label and value pill for GhostLine UI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GhostLineLookAndFeel.h"

//==============================================================================
class ModernKnobComponent : public juce::Component
{
public:
    ModernKnobComponent();
    ~ModernKnobComponent() override;
    
    juce::Slider& getSlider() { return slider; }
    const juce::Slider& getSlider() const { return slider; }
    
    void setLabelText (const juce::String& text);
    void setThemeColour (juce::Colour colour);
    void setValueFormatter (std::function<juce::String(float)> formatter);
    
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void updateValueText();
    
    juce::Slider slider;
    juce::Label label;
    juce::Label valuePill;
    GhostLineLookAndFeel laf;
    std::function<juce::String(float)> valueFormatter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModernKnobComponent)
};
