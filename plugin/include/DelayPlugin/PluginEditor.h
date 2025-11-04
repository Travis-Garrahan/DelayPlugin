#pragma once

#include "PluginProcessor.h"

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

    juce::Slider loopFilterCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loopFilterCutoffSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
