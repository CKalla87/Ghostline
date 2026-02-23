/*
  ==============================================================================

    GhostLineLookAndFeel.cpp

  ==============================================================================
*/

#include "GhostLineLookAndFeel.h"

//==============================================================================
GhostLineLookAndFeel::GhostLineLookAndFeel()
    : themeColour (juce::Colour (0xff60A5FA))
{
}

void GhostLineLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                            int x, int y, int width, int height,
                                            float sliderPos,
                                            float rotaryStartAngle, float rotaryEndAngle,
                                            juce::Slider& slider)
{
    if (width <= 0 || height <= 0)
        return;

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    auto centre = bounds.getCentre();
    auto fullRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    bool isEnabled = slider.isEnabled();

    // 1) Normalized position (0.0 = min, 1.0 = max)
    float norm = juce::jlimit (0.0f, 1.0f, sliderPos);

    // 2) Define angles - same coordinate system for arc and needle
    const float startAngle = rotaryStartAngle;
    const float endAngle   = rotaryEndAngle;
    const float sweep      = endAngle - startAngle;

    // 3) Progress angle: where the colored arc ends (and needle points)
    float progressAngle = startAngle + norm * sweep;

    float radius = fullRadius - 4.0f;

    // 4) Track (full arc) - very faint white
    juce::Path trackPath;
    trackPath.addCentredArc (centre.x, centre.y, radius, radius,
                             0.0f, startAngle, endAngle, true);
    g.setColour (juce::Colour (0x0dffffff)); // ~5% white
    g.strokePath (trackPath, juce::PathStrokeType (trackStrokeWidth,
                                                   juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    // 5) Colored progress arc - ONLY if norm > 0
    if (norm > 0.0f)
    {
        juce::Path progressPath;
        progressPath.addCentredArc (centre.x, centre.y, radius, radius,
                                    0.0f, startAngle, progressAngle, true);

        // Progress glow layers
        for (int i = 4; i >= 1; --i)
        {
            float strokeW = trackStrokeWidth + (float) i * 2.0f;
            g.setColour (themeColour.withAlpha (0.15f - i * 0.03f));
            g.strokePath (progressPath, juce::PathStrokeType (strokeW,
                                                              juce::PathStrokeType::curved,
                                                              juce::PathStrokeType::rounded));
        }
        g.setColour (themeColour);
        g.strokePath (progressPath, juce::PathStrokeType (trackStrokeWidth,
                                                          juce::PathStrokeType::curved,
                                                          juce::PathStrokeType::rounded));
    }

    // Knob face - circular gradient gray-800 -> gray-900
    float faceRadius = knobFaceDiameter / 2.0f;
    if (faceRadius > radius - 10.0f)
        faceRadius = radius - 10.0f;

    juce::ColourGradient faceGrad (
        juce::Colour (0xff1f2937), centre.x, centre.y - faceRadius,
        juce::Colour (0xff111827), centre.x, centre.y + faceRadius,
        false
    );
    g.setGradientFill (faceGrad);
    g.fillEllipse (centre.x - faceRadius, centre.y - faceRadius,
                   faceRadius * 2.0f, faceRadius * 2.0f);

    // Border
    g.setColour (juce::Colour (0x1affffff));
    g.drawEllipse (centre.x - faceRadius, centre.y - faceRadius,
                   faceRadius * 2.0f, faceRadius * 2.0f, 1.0f);

    // 6) Needle - points to progressAngle (same angle as arc end)
    // At norm==0: progressAngle==startAngle, needle at far-left bottom
    // At norm==1: progressAngle==endAngle, needle at far-right bottom
    float needleRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.42f;
    float inner = needleRadius * 0.15f;
    float outer = needleRadius * 0.95f;

    juce::Point<float> p1 (centre.x + std::cos (progressAngle) * inner,
                           centre.y + std::sin (progressAngle) * inner);
    juce::Point<float> p2 (centre.x + std::cos (progressAngle) * outer,
                           centre.y + std::sin (progressAngle) * outer);

    // Needle glow
    g.setColour (themeColour.withAlpha (0.5f));
    g.drawLine (p1.x, p1.y, p2.x, p2.y, 2.5f);
    g.setColour (themeColour.withAlpha (isEnabled ? 0.9f : 0.4f));
    g.drawLine (p1.x, p1.y, p2.x, p2.y, 2.0f);

    // Center dot
    float dotRadius = 3.0f;
    g.setColour (themeColour.withAlpha (0.6f));
    g.fillEllipse (centre.x - dotRadius - 1, centre.y - dotRadius - 1,
                   (dotRadius + 1) * 2, (dotRadius + 1) * 2);
    g.setColour (themeColour);
    g.fillEllipse (centre.x - dotRadius, centre.y - dotRadius,
                   dotRadius * 2, dotRadius * 2);
}
