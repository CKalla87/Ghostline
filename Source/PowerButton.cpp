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
    buttonImageOff = juce::ImageCache::getFromMemory (BinaryData::power_button_off_png,
                                                      BinaryData::power_button_off_pngSize);
}

void PowerButton::paintButton (juce::Graphics& g, bool hovered, bool pressed)
{
    auto bounds = getLocalBounds().toFloat();
    bool isOn = getToggleState();

    juce::Path clipPath;
    clipPath.addRoundedRectangle (bounds, cornerRadius);
    g.reduceClipRegion (clipPath);

    juce::Image& img = isOn ? buttonImageOn : buttonImageOff;
    if (! img.isValid())
    {
        g.setColour (isOn ? juce::Colour (0xff1e3a8a) : juce::Colour (0xff1e293b));
        g.fillRoundedRectangle (bounds, cornerRadius);
        return;
    }

    float alpha = pressed ? 0.85f : 1.0f;
    g.setOpacity (alpha);
    g.drawImage (img, bounds, juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination);
}
