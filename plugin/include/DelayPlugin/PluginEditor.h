#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "CustomLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    CustomLookAndFeel customLookAndFeel;
    juce::Image backgroundImage;

    // Mix
    juce::Slider mixSlider;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;

    // Delay
    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttachment;

    // Feedback
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;

    // Loop filter
    juce::Slider loopFilterCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loopFilterCutoffSliderAttachment;
    // Loop filter type
    juce::ComboBox loopFilterTypeComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> loopFilterTypeComboBoxAttachment;

    // Diffusion
    juce::Slider diffusionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diffusionSliderAttachment;

    // Ping Pong Toggle
    juce::ToggleButton pingPongToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> pingPongToggleButtonAttachment;

    // Bypass
    juce::ToggleButton bypassToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassToggleButtonAttachment;
    juce::Label bypassLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};