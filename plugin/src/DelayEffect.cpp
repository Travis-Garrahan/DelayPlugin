#include "DelayPlugin/DelayEffect.h"

DelayEffect::DelayEffect() : m_sampleRate{}, m_delayTime{}, m_feedback{}, 
    m_mix{}, m_isPingPongOn{}, m_lastIsPingPongOn{}, m_isBypassOn{}, m_lastIsBypassOn{}
{
    // Delay time smoothing filter will have a fixed cutoff frequency of 1 Hz.
    m_delayTimeLowPass.setCutoff(1.0f); 
}


DelayEffect::~DelayEffect()
{
}


void DelayEffect::initDelayBuffers()
{
    int maxDelaySamples = static_cast<int>(MAX_DELAY_SECONDS * m_sampleRate);

    // Find next power of 2 for buffer size
    int delayBufferSize = 1;
    while (delayBufferSize < maxDelaySamples)
        delayBufferSize <<= 1;

    m_delayBuffers.clear();
    m_delayBuffers.reserve(2);
    for (int ch = 0; ch < 2; ++ch)
        m_delayBuffers.emplace_back(delayBufferSize, 0.0f);
}


void DelayEffect::prepareToPlay(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_delayTimeLowPass.setSampleRate(static_cast<unsigned int>(sampleRate));
    initDelayBuffers();
}


void DelayEffect::releaseResources()
{
    clearDelayBuffers();
    m_delayTimeLowPass.clear();
}

void DelayEffect::setParametersFromAPVTS(juce::AudioProcessorValueTreeState& apvts)
{
    // Get current parameter values. These values are read once per block.
    m_mix = *apvts.getRawParameterValue("MIX");
    m_feedback = *apvts.getRawParameterValue("FEEDBACK");
    m_delayTime = *apvts.getRawParameterValue("DELAY_TIME");    
    m_isPingPongOn = *apvts.getRawParameterValue("IS_PING_PONG_ON");
    m_isBypassOn = *apvts.getRawParameterValue("IS_BYPASS_ON");
}


void DelayEffect::update()
{
    // Check if toggle values changed. Clear delay buffers if so
    if (m_isBypassOn != m_lastIsBypassOn)
    {
        clearDelayBuffers();
        m_lastIsBypassOn = m_isBypassOn;
    }
 
    if (m_isPingPongOn != m_lastIsPingPongOn)
    {
        clearDelayBuffers();
        m_lastIsPingPongOn = m_isPingPongOn;
    }
}


void DelayEffect::processAudioBuffer(juce::AudioBuffer<float>& buffer)
{
    if (m_isBypassOn == true)
        return;

    auto& delayBufferLeft = m_delayBuffers[0]; 
    auto& delayBufferRight = m_delayBuffers[1]; 

    // Get left and right audio buffers. Each contains the input data, which is processed by overwriting it
    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    // Loop through each sample
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        // Get the current input sample for each channel
        float inputLeft = channelDataLeft[i];
        float inputRight = channelDataRight[i];
        
        // Apply smoothing to delay time slider value.
        float currentDelayTimeSeconds = m_delayTimeLowPass.getNextSample(m_delayTime) / 1000.0f;
        
        // Get read index from current delay time.
        int delayInSamples = static_cast<int>(currentDelayTimeSeconds * m_sampleRate);
        int readIndex = static_cast<int>(delayBufferLeft.size) - delayInSamples;

        // Get delayed outputs and apply feedback gain.
        float delayedLeft = m_feedback * delayBufferLeft[readIndex];
        float delayedRight = m_feedback * delayBufferRight[readIndex];

        // Determine feedback configuration 
        if (m_isPingPongOn == true)
        {
            // Mix left and right channels down to mono.
            float inputMono = (inputLeft + inputRight) / 2.0f;

            // Feed left and right delay buffers into eachother. Incomming audio will be fed into the left delay.
            delayBufferLeft.push(inputMono + delayedRight);
            delayBufferRight.push(delayedLeft);
        }
        else
        {
            // Independent feedback loop for each channel. 
            delayBufferLeft.push(inputLeft + delayedLeft);
            delayBufferRight.push(inputRight + delayedRight);
        }
        
        // Get output audio for each channel. Mix dry signal with wet signal             
        float outputLeft = (1.0f - m_mix) * inputLeft + m_mix * delayedLeft;	    
        float outputRight = (1.0f - m_mix) * inputRight + m_mix * delayedRight;

        // Write output to the audio buffer
        channelDataLeft[i] = outputLeft;
        channelDataRight[i] = outputRight;
    }
}


void DelayEffect::clearDelayBuffers()
{
    for (auto& delayBuffer : m_delayBuffers)
        delayBuffer.clear();
}
