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

    for (int ch = 0; ch < getTotalNumOutputChannels(); ++ch)
        m_delayBuffers.emplace_back(bufferSize, 0.0f);

    // Initialize lowpass filters
    for (int ch = 0; ch < getTotalNumOutputChannels(); ++ch)
        m_lowPassFilters.emplace_back(static_cast<unsigned int>(sampleRate), 1.0f); // Initialize cutoff freq to 1 Hz

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

    // Get current slider values. These values are read once per block.
    float mix = *m_apvts.getRawParameterValue("MIX");
    float feedback = *m_apvts.getRawParameterValue("FEEDBACK");
    float delayTimeSeconds = *m_apvts.getRawParameterValue("DELAY_TIME") / 1000.0f;    

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& delayBuffer = m_delayBuffers[channel];
        auto& lowPass = m_lowPassFilters[channel];

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Apply smoothing to delay time slider value 
            float currentDelayTimeSeconds = lowPass.getNextSample(delayTimeSeconds);
            int delayInSamples = static_cast<int>(currentDelayTimeSeconds * m_sampleRate);
            
            // Input sample
            float in = channelData[i];

            // Get delayed output and apply feedback gain
            float delayed = feedback * delayBuffer[static_cast<int>(delayBuffer.size) - delayInSamples];

            // Mix incomming audio with delayed output and feed it back into the delay buffer
            delayBuffer.push(in + delayed);
            
            // Mix dry signal with wet signal             
            channelData[i] = (1.0f - mix) * in + mix * delayed;	    
        }
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

    return { params.begin(), params.end() };
}

juce::AudioProcessorValueTreeState& AudioPluginAudioProcessor::getAPVTS()
{
    return m_apvts;
}
