//
// Created by Travis Garrahan on 11/10/25.
//

#include "DelayPlugin/CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    knobDrawable = juce::Drawable::createFromImageData(BinaryData::knob_svg, BinaryData::knob_svgSize);
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
                                    float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    if(knobDrawable != nullptr)
    {
        const float rotation = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        g.addTransform(juce::AffineTransform::rotation(rotation,
                                                       static_cast<float>(x) + static_cast<float>(width) * 0.5f,
                                                       static_cast<float>(y) + static_cast<float>(height) * 0.5f));

        knobDrawable->drawWithin(g, juce::Rectangle<float>(static_cast<float>(x),
                                                           static_cast<float>(y),
                                                           static_cast<float>(width),
                                                           static_cast<float>(height)),
                                 juce::RectanglePlacement::centred, 1.0f);
    }

}
