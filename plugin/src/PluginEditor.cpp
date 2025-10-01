#include "DelayPlugin/PluginProcessor.h"
#include "DelayPlugin/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // int textBoxWidth = 50;

    // Delay Time
    addAndMakeVisible(delayTimeSlider);
    delayTimeSlider.setRange(1.0, 1000.0);
    delayTimeSlider.setValue(500.0);
    delayTimeSlider.setNumDecimalPlacesToDisplay(0);
    // delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, textBoxWidth, delayTimeSlider.getTextBoxHeight());
    addAndMakeVisible(delayTimeLabel);
    delayTimeLabel.setText("Time", juce::dontSendNotification);
    delayTimeLabel.attachToComponent(&delayTimeSlider, true);

    // Feedback
    addAndMakeVisible(feedbackSlider);
    feedbackSlider.setRange(0.0, 0.99);
    feedbackSlider.setValue(0.5);
    feedbackSlider.setNumDecimalPlacesToDisplay(3);
    // feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, textBoxWidth, feedbackSlider.getTextBoxHeight());
    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, true);

    // Mix
    addAndMakeVisible(mixSlider);
    mixSlider.setRange(0.0, 1.0);
    mixSlider.setValue(0.5);
    mixSlider.setNumDecimalPlacesToDisplay(3);
    // mixSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, textBoxWidth, mixSlider.getTextBoxHeight());
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, true);

    setSize (400, 300);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int leftMargin = getWidth() / 2 - 100;
    int sliderWidth = 200;
    int sliderHeight = 25;
    int pad = 10;

    // setBound(int x, int y, int width, int height)
    delayTimeSlider.setBounds(leftMargin, (getHeight() - sliderHeight) / 2 - pad - sliderHeight, sliderWidth, sliderHeight);
    feedbackSlider.setBounds(leftMargin, (getHeight() - sliderHeight) / 2, sliderWidth, sliderHeight);
    mixSlider.setBounds(leftMargin, (getHeight() - sliderHeight) / 2 + pad + sliderHeight, sliderWidth, sliderHeight);

}
