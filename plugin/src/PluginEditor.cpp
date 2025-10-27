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

    // Delay Time
    delayTimeSlider.setNumDecimalPlacesToDisplay(0);
    delayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(delayTimeSlider);

    // Attach the delay Time slider to the AudioProcessorValueTreeState
    delayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, 
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

    feedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, 
        "FEEDBACK", feedbackSlider);

    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(feedbackLabel);

    // Mix

    mixSlider.setNumDecimalPlacesToDisplay(3);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(mixSlider);
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, 
        "MIX", mixSlider);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
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
}
