#include "DelayPlugin/PluginProcessor.h"
#include "DelayPlugin/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), m_apvts (*this, nullptr, "Parameters", createParameters())
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialization before playback 
    
    m_sampleRate = sampleRate;

    // Initialize delay buffers
    const float MAX_DELAY_SECONDS = 2.0f;
    int maxDelaySamples = static_cast<int>(MAX_DELAY_SECONDS * sampleRate);

    // Find next power of 2 for buffer size		
    int bufferSize = 1;
    while (bufferSize < maxDelaySamples) 
        bufferSize <<= 1;
    
    m_delayBuffers.clear();
    m_delayBuffers.reserve(static_cast<size_t>(getTotalNumOutputChannels()));

    // One circular buffer for each channel
    for (int ch = 0; ch < getTotalNumOutputChannels(); ++ch)
        m_delayBuffers.emplace_back(bufferSize, 0.0f);
    
    // Initialize lowpass filter
    m_lowPass.setSampleRate(static_cast<unsigned int>(sampleRate));
    m_lowPass.setCutoff(1.0f); // 1 Hz

    juce::ignoreUnused(samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    for (auto& buffer : m_delayBuffers)
    {
        buffer.clear();
    }

    m_lowPass.clear();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Must have 2 channels for ping-pong delay.
    if (buffer.getNumChannels() < 2)
        return;

    // Get current parameter values. These values are read once per block.
    float mix = *m_apvts.getRawParameterValue("MIX");
    float feedback = *m_apvts.getRawParameterValue("FEEDBACK");
    float delayTimeSeconds = *m_apvts.getRawParameterValue("DELAY_TIME") / 1000.0f;    
    bool isPingPongEnabled = *m_apvts.getRawParameterValue("IS_PING_PONG_ENABLED");
    bool isEffectEnabled = *m_apvts.getRawParameterValue("IS_EFFECT_ENABLED");

    // If effect is disabled, set the wet mix to zero. 
    if (isEffectEnabled == false)
        mix = 0.0f;
    
    // Get left and right audio buffers. Each buffer contains the input data. 
    // Data is processed by overwriting it.
    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    // Get left and right delay buffers.
    auto& delayBufferLeft = m_delayBuffers[0];
    auto& delayBufferRight = m_delayBuffers[1];

    // Lowpass filter for smoothing the delay time input value. This is not stereo-dependent 
    // (left and right delay lines should have the same delay time), so we only need one of these.
    auto& lowPass = m_lowPass; 

    // Loop through each sample
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        // Apply smoothing to delay time slider value. Convert to delay in samples. 
        float currentDelayTimeSeconds = lowPass.getNextSample(delayTimeSeconds);
        int delayInSamples = static_cast<int>(currentDelayTimeSeconds * m_sampleRate);
        
        // Get the current input sample for each channel
        float inLeft = channelDataLeft[i];
        float inRight = channelDataRight[i];
        
        // Get delayed outputs and apply feedback gain
        float delayedLeft = feedback * delayBufferLeft[static_cast<int>(delayBufferLeft.size) - delayInSamples];
        float delayedRight = feedback * delayBufferRight[static_cast<int>(delayBufferRight.size) - delayInSamples];

        // Write to each delay buffer to implement the feedback loop.
        // 
        // 2 possible configurations: 
        //
        //   Normal:      Left delay output is fed back into the LEFT delay input, and right delay output is fed back into 
        //                RIGHT delay input. The left and right channels have their own independent feedback loops. 
        //
        //   Ping pong:   Left delay output is fed into the RIGHT delay input and vice versa. The left and right delays are 
        //                coupled/fed back into one another. This creates the effect of successive delayed outputs bouncing 
        //                between the left and right channels. The input data is first mixed down to mono so that audio from 
        //                both channels enters the feedback loop.
        //
        if (isPingPongEnabled == true)
        {
            // Mix left and right channels down to mono
            float inMono = (inLeft + inRight) / 2.0f;

            // Feed left and right delay buffers into eachother.
            // Incomming audio will be fed into the left delay.
            delayBufferLeft.push(inMono + delayedRight);
            delayBufferRight.push(delayedLeft);
        }
	    else
        {
            // Normal delay mode: Independent feedback loop for each channel.
            // Mix incomming audio with delayed output and feed it back into 
            // the delay buffer. 
            delayBufferLeft.push(inLeft + delayedLeft);
            delayBufferRight.push(inRight + delayedRight);
        }
        
        // Get output audio for each channel. Mix dry signal with wet signal             
        channelDataLeft[i] = (1.0f - mix) * inLeft + mix * delayedLeft;	    
        channelDataRight[i] = (1.0f - mix) * inRight + mix * delayedRight;
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    // List of ranged audio parameters 
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params; 

    // Args: String parameterID, String parameterName, float minValue, float maxValue, float defaultValue
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY_TIME", "Time", 1.f, 1000.f, 500.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "Feedback", 0.f, 0.99f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.f, 1.f, 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>("IS_PING_PONG_ENABLED", "Ping Pong", false));
    // NOTE: Maybe use juce::AudioParameterChoice instead of bool. Could use "STEREO_MODE" with options 
    // "Normal", "Ping Pong", or "Stereo"

    params.push_back(std::make_unique<juce::AudioParameterBool>("IS_EFFECT_ENABLED", "Effect Enabled", true));

    return { params.begin(), params.end() };
}

juce::AudioProcessorValueTreeState& AudioPluginAudioProcessor::getAPVTS()
{
    return m_apvts;
}
