/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GhostlineAudioProcessorEditor::GhostlineAudioProcessorEditor (GhostlineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setBufferedToImage (true);

    // Power button
    addAndMakeVisible (powerButton);

    // Spectrum display
    addAndMakeVisible (spectrumDisplay);

    // Knobs - must initialize before setSize, since setSize triggers resized() which uses knobs[]
    knobs[0] = &delayTimeKnob;
    knobs[1] = &feedbackKnob;
    knobs[2] = &wetKnob;
    knobs[3] = &dryKnob;
    knobs[4] = &modRateKnob;
    knobs[5] = &modDepthKnob;

    for (auto* k : knobs)
        addAndMakeVisible (k);

    setSize (editorWidth, editorHeight);  // Triggers resized() - must be last so knobs are valid

    delayTimeKnob.setLabelText ("Delay Time");
    delayTimeKnob.setThemeColour (colourBlue);
    delayTimeKnob.setValueFormatter ([] (float v) {
        return juce::String (juce::roundToInt (v * 1000.0f)) + "ms";
    });
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DELAYTIME", delayTimeKnob.getSlider());

    feedbackKnob.setLabelText ("Feedback");
    feedbackKnob.setThemeColour (colourBlue);
    feedbackKnob.setValueFormatter ([] (float v) {
        return juce::String (juce::roundToInt (v * 100.0f)) + "%";
    });
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "FEEDBACK", feedbackKnob.getSlider());

    wetKnob.setLabelText ("Wet");
    wetKnob.setThemeColour (colourGreen);
    wetKnob.setValueFormatter ([] (float v) {
        return juce::String (juce::roundToInt (v * 100.0f)) + "%";
    });
    wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "WET", wetKnob.getSlider());

    dryKnob.setLabelText ("Dry");
    dryKnob.setThemeColour (colourGreen);
    dryKnob.setValueFormatter ([] (float v) {
        return juce::String (juce::roundToInt (v * 100.0f)) + "%";
    });
    dryAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DRY", dryKnob.getSlider());

    modRateKnob.setLabelText ("Mod Rate");
    modRateKnob.setThemeColour (colourPurple);
    modRateKnob.setValueFormatter ([] (float v) {
        return juce::String (v * 10.0f, 1) + "Hz";
    });
    modRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MODRATE", modRateKnob.getSlider());

    modDepthKnob.setLabelText ("Mod Depth");
    modDepthKnob.setThemeColour (colourPurple);
    modDepthKnob.setValueFormatter ([] (float v) {
        return juce::String (juce::roundToInt (v * 100.0f)) + "%";
    });
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MODDEPTH", modDepthKnob.getSlider());
}

GhostlineAudioProcessorEditor::~GhostlineAudioProcessorEditor()
{
    // Detach from processor before components are destroyed (reduces callback races on teardown)
    delayTimeAttachment.reset();
    feedbackAttachment.reset();
    wetAttachment.reset();
    dryAttachment.reset();
    modRateAttachment.reset();
    modDepthAttachment.reset();
}

//==============================================================================
void GhostlineAudioProcessorEditor::drawBackgroundAndContainer (juce::Graphics& g)
{
    auto fullBounds = getLocalBounds().toFloat();

    // 1. Outer background - diagonal gradient deep navy
    juce::ColourGradient outerGrad (
        juce::Colour (0xf20f172a), fullBounds.getX(), fullBounds.getY(),
        juce::Colour (0xf21e293b), fullBounds.getRight(), fullBounds.getBottom(),
        false
    );
    g.setGradientFill (outerGrad);
    g.fillAll();

    // 2. Main container - rounded rect with glass feel
    auto containerBounds = fullBounds.reduced ((float) outerPadding);
    juce::ColourGradient containerGrad (
        juce::Colour (0xf21e293b), containerBounds.getX(), containerBounds.getY(),
        juce::Colour (0xf20f172a), containerBounds.getX(), containerBounds.getBottom(),
        false
    );
    g.setGradientFill (containerGrad);
    g.fillRoundedRectangle (containerBounds, (float) mainRadius);

    // Border
    g.setColour (juce::Colour (0x1affffff));
    g.drawRoundedRectangle (containerBounds, (float) mainRadius, 1.0f);
}

void GhostlineAudioProcessorEditor::drawGradientTitle (juce::Graphics& g, juce::Rectangle<int> area)
{
    if (area.isEmpty())
        return;

    juce::Font titleFont (36.0f, juce::Font::bold);
    g.setFont (titleFont);

    juce::GlyphArrangement glyphs;
    glyphs.addFittedText (titleFont, "GhostLine",
                          (float) area.getX(), (float) area.getY(),
                          (float) area.getWidth(), (float) area.getHeight(),
                          juce::Justification::centredLeft, 1);

    juce::Path textPath;
    glyphs.createPath (textPath);

    auto pathBounds = textPath.getBounds();
    if (pathBounds.isEmpty())
        return;
    juce::ColourGradient grad (
        juce::Colour (0xff60A5FA), pathBounds.getX(), pathBounds.getCentreY(),
        juce::Colour (0xff67E8F9), pathBounds.getRight(), pathBounds.getCentreY(),
        false
    );
    g.setGradientFill (grad);
    g.fillPath (textPath);
}

void GhostlineAudioProcessorEditor::paint (juce::Graphics& g)
{
    drawBackgroundAndContainer (g);

    auto containerBounds = getLocalBounds().reduced (outerPadding);
    auto headerArea = containerBounds.removeFromTop (headerPaddingV * 2 + 50);

    // Header content area
    auto headerContent = headerArea.reduced (headerPaddingH, headerPaddingV);

    // Title + subtitle (left)
    auto titleArea = headerContent.removeFromLeft (280);
    drawGradientTitle (g, titleArea.withHeight (42));
    g.setFont (14.0f);
    g.setColour (juce::Colour (0x80ffffff));
    g.drawText ("Space Echo Delay", titleArea.getX(), titleArea.getY() + 38,
                titleArea.getWidth(), 20, juce::Justification::centredLeft);

    // Power button is placed in resized()
}

void GhostlineAudioProcessorEditor::resized()
{
    auto fullBounds = getLocalBounds();
    auto containerBounds = fullBounds.reduced (outerPadding);

    // Header row
    auto headerArea = containerBounds.removeFromTop (headerPaddingV * 2 + 50);
    auto headerContent = headerArea.reduced (headerPaddingH, headerPaddingV);

    // Power button - right side
    constexpr int powerSize = 88;
    powerButton.setBounds (headerContent.getRight() - powerSize, headerContent.getY(),
                           powerSize, powerSize);

    // Display panel
    containerBounds.removeFromTop (displayMarginTop);
    auto displayArea = containerBounds.removeFromTop (displayHeight);
    spectrumDisplay.setBounds (displayArea.reduced (headerPaddingH, 0));

    // Knob row - evenly spaced
    auto knobRowArea = containerBounds.reduced (headerPaddingH, 20);
    int totalKnobWidth = 6 * knobUnitWidth + 5 * knobGap;
    int startX = knobRowArea.getX() + (knobRowArea.getWidth() - totalKnobWidth) / 2 + knobUnitWidth / 2;

    for (int i = 0; i < 6; ++i)
    {
        if (knobs[i] != nullptr)
        {
            int cx = startX + i * (knobUnitWidth + knobGap);
            knobs[i]->setBounds (cx - knobUnitWidth / 2, knobRowArea.getY(),
                                 knobUnitWidth, knobRowArea.getHeight());
        }
    }
}
