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
    setSize (800, 600);
    setBufferedToImage (true);

    // Delay Time slider
    delayTimeSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    delayTimeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    delayTimeSlider.setTextBoxIsEditable (false);
    delayTimeSlider.setTextValueSuffix (" s");
    delayTimeSlider.setOpaque (false);
    delayTimeSlider.setColour (juce::Slider::rotarySliderFillColourId, ghostCyan);
    delayTimeSlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostPurple);
    delayTimeSlider.setColour (juce::Slider::thumbColourId, ghostGreen);
    delayTimeSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    delayTimeSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    delayTimeSlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    delayTimeSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&delayTimeSlider);
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DELAYTIME", delayTimeSlider);
    
    delayTimeLabel.setText ("Delay Time", juce::dontSendNotification);
    delayTimeLabel.setJustificationType (juce::Justification::centred);
    delayTimeLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&delayTimeLabel);

    // Feedback slider
    feedbackSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackSlider.setTextBoxIsEditable (false);
    feedbackSlider.setOpaque (false);
    feedbackSlider.setColour (juce::Slider::rotarySliderFillColourId, ghostGreen);
    feedbackSlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostCyan);
    feedbackSlider.setColour (juce::Slider::thumbColourId, ghostOrange);
    feedbackSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    feedbackSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    feedbackSlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    feedbackSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&feedbackSlider);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "FEEDBACK", feedbackSlider);
    
    feedbackLabel.setText ("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType (juce::Justification::centred);
    feedbackLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&feedbackLabel);

    // Wet slider
    wetSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    wetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    wetSlider.setTextBoxIsEditable (false);
    wetSlider.setOpaque (false);
    wetSlider.setColour (juce::Slider::rotarySliderFillColourId, ghostCyan);
    wetSlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostPurple);
    wetSlider.setColour (juce::Slider::thumbColourId, ghostGreen);
    wetSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    wetSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    wetSlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    wetSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&wetSlider);
    wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "WET", wetSlider);
    
    wetLabel.setText ("Wet", juce::dontSendNotification);
    wetLabel.setJustificationType (juce::Justification::centred);
    wetLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&wetLabel);

    // Dry slider
    drySlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    drySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    drySlider.setTextBoxIsEditable (false);
    drySlider.setOpaque (false);
    drySlider.setColour (juce::Slider::rotarySliderFillColourId, ghostPurple);
    drySlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostCyan);
    drySlider.setColour (juce::Slider::thumbColourId, ghostOrange);
    drySlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    drySlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    drySlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    drySlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&drySlider);
    dryAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DRY", drySlider);
    
    dryLabel.setText ("Dry", juce::dontSendNotification);
    dryLabel.setJustificationType (juce::Justification::centred);
    dryLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&dryLabel);

    // Modulation Rate slider
    modRateSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    modRateSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    modRateSlider.setTextBoxIsEditable (false);
    modRateSlider.setOpaque (false);
    modRateSlider.setColour (juce::Slider::rotarySliderFillColourId, ghostOrange);
    modRateSlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostGreen);
    modRateSlider.setColour (juce::Slider::thumbColourId, ghostCyan);
    modRateSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    modRateSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    modRateSlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    modRateSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&modRateSlider);
    modRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "MODRATE", modRateSlider);
    
    modRateLabel.setText ("Mod Rate", juce::dontSendNotification);
    modRateLabel.setJustificationType (juce::Justification::centred);
    modRateLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&modRateLabel);

    // Modulation Depth slider
    modDepthSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    modDepthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    modDepthSlider.setTextBoxIsEditable (false);
    modDepthSlider.setOpaque (false);
    modDepthSlider.setColour (juce::Slider::rotarySliderFillColourId, ghostGreen);
    modDepthSlider.setColour (juce::Slider::rotarySliderOutlineColourId, ghostOrange);
    modDepthSlider.setColour (juce::Slider::thumbColourId, ghostCyan);
    modDepthSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    modDepthSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    modDepthSlider.setColour (juce::Slider::textBoxHighlightColourId, juce::Colours::transparentBlack);
    modDepthSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (&modDepthSlider);
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "MODDEPTH", modDepthSlider);
    
    modDepthLabel.setText ("Mod Depth", juce::dontSendNotification);
    modDepthLabel.setJustificationType (juce::Justification::centred);
    modDepthLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&modDepthLabel);
    
    // Initialize background elements with fixed positions
    initializeBackground();
}

GhostlineAudioProcessorEditor::~GhostlineAudioProcessorEditor()
{
}

//==============================================================================
void GhostlineAudioProcessorEditor::initializeBackground()
{
    // Generate fixed positions for background elements
    juce::Random r (12345); // Fixed seed for consistent positions
    
    fogCircles.clear();
    for (int i = 0; i < 30; ++i)
    {
        FogCircle fog;
        fog.x = r.nextFloat() * 800.0f;
        fog.y = r.nextFloat() * 600.0f;
        fog.radius = 60.0f + r.nextFloat() * 100.0f;
        fogCircles.push_back (fog);
    }
    
    orbCircles.clear();
    for (int i = 0; i < 10; ++i)
    {
        OrbCircle orb;
        orb.x = r.nextFloat() * 800.0f;
        orb.y = r.nextFloat() * 600.0f;
        orb.radius = 20.0f + r.nextFloat() * 40.0f;
        orbCircles.push_back (orb);
    }
    
    backgroundInitialized = true;
}

void GhostlineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Spooky dark background
    g.fillAll (spookyBlack);
    
    // Reinitialize if size changed
    if (!backgroundInitialized || fogCircles.empty() || orbCircles.empty())
    {
        initializeBackground();
    }
    
    // Draw spooky mist/fog effect with cached positions
    g.setColour (ghostCyan.withAlpha (0.2f));
    for (const auto& fog : fogCircles)
    {
        g.fillEllipse (fog.x - fog.radius/2, fog.y - fog.radius/2, fog.radius, fog.radius);
    }
    
    // Draw ghostly orbs with cached positions
    g.setColour (ghostGreen.withAlpha (0.4f));
    for (const auto& orb : orbCircles)
    {
        g.fillEllipse (orb.x - orb.radius/2, orb.y - orb.radius/2, orb.radius, orb.radius);
    }
    
    // Draw title with spooky glow
    g.setFont (juce::Font (56.0f, juce::Font::bold));
    
    // Green glow behind text (slightly more blur)
    g.setColour (ghostGreen.withAlpha (0.5f));
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            if (i != 0 || j != 0)
            {
                g.drawText ("GHOSTLINE", getWidth() / 2 - 180 + i, 30 + j, 360, 70,
                           juce::Justification::centred, false);
            }
        }
    }
    
    // Cyan glow (slightly more blur)
    g.setColour (ghostCyan.withAlpha (0.35f));
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            if (i != 0 || j != 0)
            {
                g.drawText ("GHOSTLINE", getWidth() / 2 - 180 + i, 30 + j, 360, 70,
                           juce::Justification::centred, false);
            }
        }
    }
    
    // Main title
    juce::ColourGradient titleGradient (ghostCyan, getWidth() / 2 - 180, 30,
                                       ghostGreen, getWidth() / 2 - 180, 100,
                                       false);
    g.setGradientFill (titleGradient);
    g.drawText ("GHOSTLINE", getWidth() / 2 - 180, 30, 360, 70,
               juce::Justification::centred, false);
    
    // Subtitle
    g.setFont (20.0f);
    g.setColour (ghostPurple.withAlpha (0.9f));
    g.drawText ("Spooky Space Echo", getWidth() / 2 - 150, 95, 300, 30,
               juce::Justification::centred, false);
}

void GhostlineAudioProcessorEditor::resized()
{
    const int knobSize = 100;
    const int labelHeight = 25;
    const int startY = 150;
    const int spacing = (getWidth() - 100 - (knobSize * 6)) / 5;
    
    int x = 50;
    
    // Delay Time
    delayTimeSlider.setBounds (x, startY, knobSize, knobSize);
    delayTimeLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Feedback
    feedbackSlider.setBounds (x, startY, knobSize, knobSize);
    feedbackLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Wet
    wetSlider.setBounds (x, startY, knobSize, knobSize);
    wetLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Dry
    drySlider.setBounds (x, startY, knobSize, knobSize);
    dryLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Mod Rate
    modRateSlider.setBounds (x, startY, knobSize, knobSize);
    modRateLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Mod Depth
    modDepthSlider.setBounds (x, startY, knobSize, knobSize);
    modDepthLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
}

