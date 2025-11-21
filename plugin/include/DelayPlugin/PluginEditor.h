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
    CustomLookAndFeel customLookAndFeel;

    juce::Image backgroundImage;

    AudioPluginAudioProcessor& processorRef;
    
    juce::Slider delayTimeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttachment;
    juce::Label delayTimeLabel;
    
    juce::Slider feedbackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;
    juce::Label feedbackLabel;

    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
    juce::Label mixLabel;

    juce::ToggleButton pingPongToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> pingPongToggleButtonAttachment;
    juce::Label pingPongLabel;

    juce::ToggleButton bypassToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassToggleButtonAttachment;
    juce::Label bypassLabel;

    // Loop filter cutoff
    juce::Slider loopFilterCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loopFilterCutoffSliderAttachment;

    // Loop filter type
    juce::ComboBox loopFilterTypeComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> loopFilterTypeComboBoxAttachment;

    // Diffusion slider
    juce::Slider diffusionSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> diffusionSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
