#include "DelayPlugin/PluginProcessor.h"
#include "DelayPlugin/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize(400, 300);
    //setSize(400, 500);

    juce::AudioProcessorValueTreeState& apvts = processorRef.getAPVTS();

    // Delay Time
    delayTimeSlider.setNumDecimalPlacesToDisplay(0);
    delayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(delayTimeSlider);

    // Attach the delay Time slider to the AudioProcessorValueTreeState
    delayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "DELAY_TIME", delayTimeSlider);

    delayTimeLabel.setText("Time", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayTimeLabel);
    //delayTimeLabel.attachToComponent(&delayTimeSlider, true);

    // Feedback
    feedbackSlider.setNumDecimalPlacesToDisplay(3);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(feedbackSlider);

    feedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "FEEDBACK", feedbackSlider);

    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackLabel);

    // Mix

    mixSlider.setNumDecimalPlacesToDisplay(3);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(mixSlider);

    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "MIX", mixSlider);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
    //mixLabel.attachToComponent(&mixSlider, true);

    addAndMakeVisible(pingPongToggleButton);
    pingPongToggleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,
        "IS_PING_PONG_ON", pingPongToggleButton);

    pingPongLabel.setText("Ping Pong", juce::dontSendNotification);
    //pingPongLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(pingPongLabel);


    addAndMakeVisible(bypassToggleButton);
    bypassToggleButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,
        "IS_BYPASS_ON", bypassToggleButton);

    bypassLabel.setText("Bypass", juce::dontSendNotification);
    addAndMakeVisible(bypassLabel);


    // Cutoff
    loopFilterCutoffSlider.setNumDecimalPlacesToDisplay(0);
    loopFilterCutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    loopFilterCutoffSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //loopFilterCutoffSlider.setSkewFactorFromMidPoint(500);
    addAndMakeVisible(loopFilterCutoffSlider);
    loopFilterCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, 
        "LOOP_FILTER_CUTOFF", loopFilterCutoffSlider);

    // Filter type
    auto* parameter = apvts.getParameter("LOOP_FILTER_TYPE");
    loopFilterTypeComboBox.addItemList(parameter->getAllValueStrings(), 1);

    addAndMakeVisible(loopFilterTypeComboBox);
    loopFilterTypeComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, 
        "LOOP_FILTER_TYPE", loopFilterTypeComboBox);

    // Diffusion slider
    diffusionSlider.setNumDecimalPlacesToDisplay(0);
    diffusionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    diffusionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(diffusionSlider);
    diffusionSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,
        "DIFFUSION", diffusionSlider);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // Background image
    const int backgroundImageDataSize = BinaryData::background_jpgSize;
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::background_jpg, backgroundImageDataSize);
    g.drawImageAt(backgroundImage, 0, 0);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Lay out the GUI components
    int sliderWidth = 80;
    int sliderHeight = 80;
    int labelWidth = 80;
    int labelHeight = 15;

    // setBound(int x, int y, int width, int height)
    mixSlider.setBounds( (int)(getWidth() * 0.15), getHeight() / 2 - sliderHeight, sliderWidth, sliderHeight);
    mixLabel.setBounds( (int)(getWidth() * 0.15), getHeight() / 2, labelWidth, labelHeight);
    
    delayTimeSlider.setBounds((int)(getWidth() * 0.85) - sliderWidth, getHeight() / 2 - sliderHeight, sliderWidth, sliderHeight);
    delayTimeLabel.setBounds((int)(getWidth() * 0.85) - sliderWidth, getHeight() / 2, labelWidth, labelHeight);

    feedbackSlider.setBounds(getWidth() / 2 - sliderWidth / 2, getHeight() / 2, sliderWidth, sliderHeight);
    feedbackLabel.setBounds(getWidth() / 2 - sliderWidth / 2, getHeight() / 2 + sliderHeight, labelWidth, labelHeight);


    int toggleWidth = 30;
    int toggleHeight = 20;

    //pingPongToggleButton.setBounds(0, (int)(0.9 * getHeight()), 20, 20);
    pingPongToggleButton.setBounds((int)(getWidth() * 0.15), (int)(0.9 * getHeight()), toggleWidth, toggleHeight);
    pingPongLabel.setBounds((int)(getWidth() * 0.15), (int)(0.9 * getHeight())- 20, labelWidth, labelHeight);

    bypassToggleButton.setBounds((int)(getWidth() * 0.5 - toggleWidth), (int)(getHeight() * 0.9), toggleWidth, toggleHeight);
    bypassLabel.setBounds((int)(getWidth() * 0.5 - toggleWidth), (int)(getHeight() * 0.9) - 20, labelWidth, labelHeight);

    loopFilterCutoffSlider.setBounds((int)(getWidth() * 0.75), (int)(getHeight() * 0.9) - sliderHeight, sliderWidth, sliderHeight);

    loopFilterTypeComboBox.setBounds((int)(getWidth() * 0.75), (int)(getHeight() * 0.9), 30, 20);

    diffusionSlider.setBounds(static_cast<int>(getWidth() / 2 - sliderWidth / 2), static_cast<int>(getHeight() / 2 - sliderHeight), sliderWidth, sliderHeight);
}

