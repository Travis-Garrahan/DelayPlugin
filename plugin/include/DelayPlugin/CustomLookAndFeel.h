//
// Created by Travis Garrahan on 11/10/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"


class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    juce::Typeface::Ptr customTypeface {juce::Typeface::createSystemTypefaceFor(BinaryData::Manbow_Clear_otf, BinaryData::Manbow_Clear_otfSize)};
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider &slider) override;

private:
    std::unique_ptr<juce::Drawable> knobDrawable;
};


