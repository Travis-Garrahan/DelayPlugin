#include "DelayPlugin/PluginProcessor.h"
#include "DelayPlugin/PluginEditor.h"

//==============================================================================
RasterComponent::RasterComponent (AudioPluginAudioProcessor& p)
    :  processorRef (p)
{
    juce::ignoreUnused (processorRef);

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

RasterComponent::~RasterComponent()
= default;

//==============================================================================
void RasterComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // Background image
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::newbg_jpg, BinaryData::newbg_jpgSize);
    g.drawImageAt(backgroundImage, 0, 0);
}

void RasterComponent::resized() {
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

// Wrapper Implementation
WrappedAudioProcessorEditor::WrappedAudioProcessorEditor(AudioPluginAudioProcessor& p) :
    AudioProcessorEditor(p),
    rasterComponent(p)
{
    addAndMakeVisible(rasterComponent);

    // these are needed to save window size
    juce::PropertiesFile::Options options;
    options.applicationName = ProjectInfo::projectName;
    options.commonToAllUsers = true;
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(options);


    /*
    // Access the component's constrainer, an object that'll impose restrictions
    // on a components size or position.
    //
    // Because getConstrainer returns a raw pointer, we use an if statement to make
    // sure we're not accessing a nullptr
    */
    if (auto* constrainer = getConstrainer())
    {
        constrainer->setFixedAspectRatio(static_cast<double> (originalWidth) / static_cast<double> (originalHeight));
        constrainer->setSizeLimits(originalWidth / 4,
            originalHeight / 4,
            originalWidth * 1.5,
            originalHeight * 1.5);
    }

    auto sizeRatio{1.0};
    if (auto* properties = applicationProperties.getCommonSettings(true))
    {
        sizeRatio = properties->getDoubleValue("sizeRatio", 1.0);
    }

    setResizable(true, true);
    setSize(static_cast<int>(originalWidth * sizeRatio),
        static_cast<int>(originalHeight * sizeRatio));
}

void WrappedAudioProcessorEditor::resized()
{
    const auto scaleFactor = static_cast<float> (getWidth()) / originalWidth;
    if (auto* properties = applicationProperties.getCommonSettings(true))
    {
        properties->setValue("sizeRatio", scaleFactor);
    }

    rasterComponent.setTransform(juce::AffineTransform::scale(scaleFactor, scaleFactor));
    rasterComponent.setBounds(0, 0, originalWidth, originalHeight);
}
