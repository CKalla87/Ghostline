/*
  ==============================================================================

    ModernKnobComponent.cpp

  ==============================================================================
*/

#include "ModernKnobComponent.h"

//==============================================================================
ModernKnobComponent::ModernKnobComponent()
{
    setOpaque (false);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    slider.setRange (0.0, 1.0, 0.001);
    slider.setValue (0.5);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::trackColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    slider.setLookAndFeel (&laf);
    slider.onValueChange = [this] { updateValueText(); };
    addAndMakeVisible (slider);
    
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour (0x99ffffff));
    label.setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    label.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setFont (juce::Font (11.0f, juce::Font::bold));
    addAndMakeVisible (label);
    
    valuePill.setJustificationType (juce::Justification::centred);
    valuePill.setFont (juce::Font (13.0f, juce::Font::bold));
    valuePill.setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    valuePill.setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (valuePill);
}

ModernKnobComponent::~ModernKnobComponent()
{
    // Must clear before laf is destroyed - prevents use-after-free when host closes plugin
    slider.setLookAndFeel (nullptr);
}

void ModernKnobComponent::setLabelText (const juce::String& text)
{
    label.setText (text.toUpperCase(), juce::dontSendNotification);
}

void ModernKnobComponent::setThemeColour (juce::Colour colour)
{
    laf.setThemeColour (colour);
    valuePill.setColour (juce::Label::textColourId, colour);
    repaint();
}

void ModernKnobComponent::setValueFormatter (std::function<juce::String(float)> formatter)
{
    valueFormatter = std::move (formatter);
    updateValueText();
}

void ModernKnobComponent::updateValueText()
{
    if (valueFormatter)
        valuePill.setText (valueFormatter ((float) slider.getValue()), juce::dontSendNotification);
}

void ModernKnobComponent::paint (juce::Graphics& g)
{
    // Value pill background and border - drawn in paint so we control the pill shape
    auto pillBounds = valuePill.getBounds().toFloat();
    if (pillBounds.getWidth() > 0 && pillBounds.getHeight() > 0)
    {
        float radius = pillBounds.getHeight() / 2.0f;
        g.setColour (juce::Colour (0x0dffffff));
        g.fillRoundedRectangle (pillBounds, radius);
        g.setColour (juce::Colour (0x1affffff));
        g.drawRoundedRectangle (pillBounds, radius, 1.0f);
    }
}

void ModernKnobComponent::resized()
{
    const int knobSize = 120;
    const int labelHeight = 18;
    const int pillHeight = 24;
    const int spacing = 6;
    
    auto area = getLocalBounds();
    
    slider.setBounds (area.withHeight (knobSize).reduced ((area.getWidth() - knobSize) / 2, 0));
    
    auto labelArea = area.withTrimmedTop (knobSize + spacing)
                         .withHeight (labelHeight);
    label.setBounds (labelArea);
    
    auto pillArea = labelArea.withTrimmedTop (labelHeight + spacing)
                             .withHeight (pillHeight)
                             .withSizeKeepingCentre (juce::jmax (60, label.getText().length() * 8), pillHeight);
    valuePill.setBounds (pillArea);
}
