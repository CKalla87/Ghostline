/*
  ==============================================================================

    PowerButton.cpp

  ==============================================================================
*/

#include "PowerButton.h"
#include "BinaryData.h"

//==============================================================================
PowerButton::PowerButton()
    : Button ("Power")
{
    setClickingTogglesState (true);
    setToggleState (true, juce::dontSendNotification);
    setOpaque (false);

    buttonImageOn = juce::ImageCache::getFromMemory (BinaryData::power_button_on_png,
                                                     BinaryData::power_button_on_pngSize);
}

void PowerButton::paintButton (juce::Graphics& g, bool hovered, bool pressed)
{
    auto bounds = getLocalBounds().toFloat();
    bool isOn = getToggleState();

    juce::Path clipPath;
    clipPath.addRoundedRectangle (bounds, cornerRadius);
    g.reduceClipRegion (clipPath);

    if (! buttonImageOn.isValid())
    {
        g.setColour (isOn ? juce::Colour (0xff1e3a8a) : juce::Colour (0xff1e293b));
        g.fillRoundedRectangle (bounds, cornerRadius);
        return;
    }

    if (isOn)
    {
        float alpha = pressed ? 0.85f : 1.0f;
        g.setOpacity (alpha);
        g.drawImage (buttonImageOn, bounds, juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination);
    }
    else
    {
        // OFF state: solid dark background first (no transparency bleed)
        g.setOpacity (1.0f);
        g.setColour (juce::Colour (0xff1e293b));
        g.fillRoundedRectangle (bounds, cornerRadius);

        // Draw dimmed image on top
        g.setOpacity (0.5f);
        g.drawImage (buttonImageOn, bounds, juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination);
    }
}
