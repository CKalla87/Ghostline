/*
  ==============================================================================

    SpectrumDisplayComponent.cpp

  ==============================================================================
*/

#include "SpectrumDisplayComponent.h"

//==============================================================================
SpectrumDisplayComponent::SpectrumDisplayComponent()
{
    setOpaque (false);
}

void SpectrumDisplayComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Slate gradient background: slate-800/50 to slate-900/50
    juce::ColourGradient bgGrad (
        juce::Colour (0x801e293b), bounds.getX(), bounds.getY(),
        juce::Colour (0x800f172a), bounds.getX(), bounds.getBottom(),
        false
    );
    g.setGradientFill (bgGrad);
    g.fillRoundedRectangle (bounds, cornerRadius);
    
    // Faint horizontal grid lines
    g.setColour (juce::Colour (0x08ffffff));
    float lineSpacing = bounds.getHeight() / (float) (gridLineCount + 1);
    for (int i = 1; i <= gridLineCount; ++i)
    {
        float y = bounds.getY() + lineSpacing * (float) i;
        g.drawHorizontalLine ((int) y, bounds.getX() + 2, bounds.getRight() - 2);
    }

    // Outline around the graph screen
    g.setColour (juce::Colour (0x1affffff));
    g.drawRoundedRectangle (bounds, cornerRadius, 1.0f);
}
