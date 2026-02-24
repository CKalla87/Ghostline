/*
  ==============================================================================

    SpectrumDisplayComponent.cpp

  ==============================================================================
*/

#include "SpectrumDisplayComponent.h"

//==============================================================================
SpectrumDisplayComponent::SpectrumDisplayComponent (GhostlineAudioProcessor& proc)
    : processor (proc)
{
    setOpaque (false);
    fftData.resize (fftSize * 2, 0.0f);
    scopeData.resize (scopeSize, 0.0f);
    startTimerHz (30);
}

void SpectrumDisplayComponent::timerCallback()
{
    if (processor.pullNextFftBlock (fftData.data(), static_cast<int> (fftData.size())))
    {
        drawNextFrameOfSpectrum();
        repaint();
    }
}

void SpectrumDisplayComponent::drawNextFrameOfSpectrum()
{
    window.multiplyWithWindowingTable (fftData.data(), fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());

    const float mindB = -72.0f;
    const float maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)
    {
        const float skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float) i / (float) scopeSize) * 0.2f);
        const int fftDataIndex = juce::jlimit (0, fftSize / 2,
            (int) (skewedProportionX * (float) fftSize * 0.5f));
        const float level = juce::jmap (
            juce::jlimit (mindB, maxdB,
                juce::Decibels::gainToDecibels (fftData[fftDataIndex])
                    - juce::Decibels::gainToDecibels ((float) fftSize)),
            mindB, maxdB, 0.0f, 1.0f);
        scopeData[static_cast<size_t>(i)] = level;
    }
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
    const float lineSpacing = bounds.getHeight() / (float) (gridLineCount + 1);
    for (int i = 1; i <= gridLineCount; ++i)
    {
        const float y = bounds.getY() + lineSpacing * (float) i;
        g.drawHorizontalLine ((int) y, (int) (bounds.getX() + 2), (int) (bounds.getRight() - 2));
    }

    // Spectrum line
    const float width = bounds.getWidth();
    const float height = bounds.getHeight();
    const float padding = 4.0f;
    const float drawWidth = width - padding * 2.0f;
    const float drawHeight = height - padding * 2.0f;
    const float drawX = bounds.getX() + padding;
    const float drawY = bounds.getY() + padding;

    g.setColour (spectrumColour);
    for (int i = 1; i < scopeSize; ++i)
    {
        const float x1 = drawX + juce::jmap ((float) (i - 1), 0.0f, (float) (scopeSize - 1), 0.0f, drawWidth);
        const float y1 = drawY + juce::jmap (scopeData[static_cast<size_t>(i - 1)], 0.0f, 1.0f, drawHeight, 0.0f);
        const float x2 = drawX + juce::jmap ((float) i, 0.0f, (float) (scopeSize - 1), 0.0f, drawWidth);
        const float y2 = drawY + juce::jmap (scopeData[static_cast<size_t>(i)], 0.0f, 1.0f, drawHeight, 0.0f);
        g.drawLine (x1, y1, x2, y2, 1.5f);
    }

    // Outline around the graph screen
    g.setColour (juce::Colour (0x1affffff));
    g.drawRoundedRectangle (bounds, cornerRadius, 1.0f);
}
