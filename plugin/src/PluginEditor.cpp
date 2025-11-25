#include "DelayPlugin/PluginProcessor.h"
#include "DelayPlugin/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 600);
    setResizable(true, true);
    setResizeLimits(400, 300, 2000, 1500);

    juce::AudioProcessorValueTreeState& apvts = processorRef.getAPVTS();

    // Delay Time
    createSliderAndLabel(&delayTimeSlider, &delayTimeLabel, "Time", customLookAndFeel);
    delayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "DELAY_TIME", delayTimeSlider);

    // Feedback
    createSliderAndLabel(&feedbackSlider, &feedbackLabel, "Feedback", customLookAndFeel);
    feedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Feedback", feedbackSlider);

    // Mix
    createSliderAndLabel(&mixSlider, &mixLabel, "Mix", customLookAndFeel);
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "MIX", mixSlider);

    addAndMakeVisible(pingPongToggleButton);
    pingPongToggleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,
        "IS_PING_PONG_ON", pingPongToggleButton);

    pingPongLabel.setText("Ping Pong", juce::dontSendNotification);
    addAndMakeVisible(pingPongLabel);

    addAndMakeVisible(bypassToggleButton);
    bypassToggleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,
        "IS_BYPASS_ON", bypassToggleButton);

    bypassLabel.setText("Bypass", juce::dontSendNotification);
    addAndMakeVisible(bypassLabel);

    // Cutoff
    createSliderAndLabel(&loopFilterCutoffSlider, &loopFilterCutoffLabel, "Loop Filter", customLookAndFeel);
    loopFilterCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "LOOP_FILTER_CUTOFF", loopFilterCutoffSlider);

    // Filter type
    const auto* parameter = apvts.getParameter("LOOP_FILTER_TYPE");
    loopFilterTypeComboBox.addItemList(parameter->getAllValueStrings(), 1);

    addAndMakeVisible(loopFilterTypeComboBox);
    loopFilterTypeComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,
        "LOOP_FILTER_TYPE", loopFilterTypeComboBox);

    // Diffusion slider
    createSliderAndLabel(&diffusionSlider, &diffusionLabel, "Diffusion", customLookAndFeel);
    diffusionSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "DIFFUSION", diffusionSlider);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
= default;

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // Background image
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::newbg_jpg, BinaryData::newbg_jpgSize);
    g.drawImageAt(backgroundImage, 0, 0);
}

void AudioPluginAudioProcessorEditor::resized() {
    // Lay out the GUI components
    constexpr int sliderWidth = 80;
    constexpr int sliderHeight = 80;
    constexpr int labelWidth = 80;
    constexpr int labelHeight = 15;

    mixSlider.setBounds(static_cast<int>(getWidth() * 0.15), getHeight() / 2 - sliderHeight, sliderWidth, sliderHeight);
    mixLabel.setBounds(static_cast<int>(getWidth() * 0.15), getHeight() / 2, labelWidth, labelHeight);

    delayTimeSlider.setBounds(static_cast<int>(getWidth() * 0.85) - sliderWidth, getHeight() / 2 - sliderHeight, sliderWidth,
                              sliderHeight);
    delayTimeLabel.setBounds(static_cast<int>(getWidth() * 0.85) - sliderWidth, getHeight() / 2, labelWidth, labelHeight);

    feedbackSlider.setBounds(getWidth() / 2 - sliderWidth / 2,
        getHeight() / 2,
        sliderWidth, sliderHeight);
    feedbackLabel.setBounds(getWidth() / 2 - sliderWidth / 2,
        getHeight() / 2 + sliderHeight,
        labelWidth, labelHeight);

    constexpr int toggleWidth = 30;
    constexpr int toggleHeight = 20;

    pingPongToggleButton.setBounds(static_cast<int>(getWidth() * 0.15),
                                   static_cast<int>(0.9 * getHeight()),
                                   toggleWidth, toggleHeight);
    pingPongLabel.setBounds(static_cast<int>(getWidth() * 0.15),
                            static_cast<int>(0.9 * getHeight()) - 20,
                            labelWidth, labelHeight);

    bypassToggleButton.setBounds(static_cast<int>(getWidth() * 0.5 - toggleWidth),
                                 static_cast<int>(getHeight() * 0.9),
                                 toggleWidth, toggleHeight);
    bypassLabel.setBounds(static_cast<int>(getWidth() * 0.5 - toggleWidth),
                          static_cast<int>(getHeight() * 0.9) - 20,
                          labelWidth, labelHeight);

    loopFilterCutoffSlider.setBounds(static_cast<int>(getWidth() * 0.75),
                                     static_cast<int>(getHeight() * 0.9) - sliderHeight,
                                     sliderWidth, sliderHeight);
    loopFilterTypeComboBox.setBounds(static_cast<int>(getWidth() * 0.75),
                                     static_cast<int> (getHeight() * 0.9),
                                     30, 20);

    diffusionSlider.setBounds(getWidth() / 2 - sliderWidth / 2,
                              getHeight() / 3 - sliderHeight - 50,
                              sliderWidth, sliderHeight);
    diffusionLabel.setBounds(getWidth() / 2 - sliderWidth / 2,
                             getHeight() / 3 - sliderHeight,
                             sliderWidth, sliderHeight);
}