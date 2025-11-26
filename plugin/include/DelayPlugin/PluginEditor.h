#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

//==============================================================================
class RasterComponent final : public juce::Component
{
public:
    explicit RasterComponent (AudioPluginAudioProcessor&);
    ~RasterComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    CustomLookAndFeel customLookAndFeel;
    juce::Image backgroundImage;

    // helper function to create slider with label
    void createSliderAndLabel(juce::Slider *slider, juce::Label *label, const juce::String &labelText, CustomLookAndFeel &_customLookAndFeel)
    {
        FontOptions fontOptions(24);

        slider->setLookAndFeel(&_customLookAndFeel);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider->setNumDecimalPlacesToDisplay(3);
        slider->setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        addAndMakeVisible(slider);

        label->setText(labelText, juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        label->setFont(Font(fontOptions));
        addAndMakeVisible(label);

    }

    // helper function to position slider and labels
    static void placeSliderWithLabel(juce::Slider* slider, juce::Label* label,
        int x, int y,
        int sliderWidth,
        int sliderHeight,
        int spacing = 0,
        const juce::String& labelText = "")
    {
        slider->setBounds(x, y, sliderWidth, sliderHeight);
        label->setBounds(x - 10,
            y + sliderHeight - spacing,
            sliderWidth + 20,
            sliderHeight);
    }

    // Mix
    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
    juce::Label mixLabel;

    // Delay
    juce::Slider delayTimeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttachment;
    juce::Label delayTimeLabel;

    // Feedback
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;

    // Loop filter
    juce::Slider loopFilterCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loopFilterCutoffSliderAttachment;
    juce::Label loopFilterCutoffLabel;
    // Loop filter type
    juce::ComboBox loopFilterTypeComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> loopFilterTypeComboBoxAttachment;

    // Diffusion
    juce::Slider diffusionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diffusionSliderAttachment;
    juce::Label diffusionLabel;

    // Ping Pong Toggle
    juce::ToggleButton pingPongToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> pingPongToggleButtonAttachment;
    juce::Label pingPongLabel;

    // Bypass
    juce::ToggleButton bypassToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassToggleButtonAttachment;
    juce::Label bypassLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RasterComponent)
};

class WrappedAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    WrappedAudioProcessorEditor (AudioPluginAudioProcessor&);
    void resized() override;
private:
    static constexpr int originalWidth { 800 };
    static constexpr int originalHeight { 600 };

    RasterComponent rasterComponent;
    juce::ApplicationProperties applicationProperties;
};