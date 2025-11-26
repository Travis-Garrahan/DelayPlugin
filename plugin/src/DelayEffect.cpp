#include "DelayPlugin/DSP/DelayEffect.h"
#include <algorithm>


DelayEffect::DelayEffect() : m_sampleRate{}, m_delayTime{}, m_feedback{}, 
    m_mix{}, m_isPingPongOn{}, m_lastIsPingPongOn{}, m_isBypassOn{}, 
    m_lastIsBypassOn{}, m_loopFilterType{}, m_lastLoopFilterType{}, 
    m_loopFilterCutoff{}, m_diffusion{}
{
    // Delay time smoothing filter will have a fixed cutoff frequency of 1 Hz.
    m_delayTimeLowPass.setCutoff(1.0f); 

    for (int channel = 0; channel < 2; channel++)
    {
        m_loopFilters.emplace_back(); 

        // Diffuser delay lengths based on Freeverb 
        // ccrma.stanford.edu/~jos/pasp/Freeverb.html
        m_diffusers.emplace_back(4, 
                        std::vector<unsigned int>{225, 556, 441, 341}, 
                            std::vector<float>{0.7f, 0.7f, 0.7f, 0.7f});
    }
}


DelayEffect::~DelayEffect()
{
}


void DelayEffect::initDelayBuffers()
{
    // Max delay in samples must be rounded up. This will be used to determine 
    // the size of the delay buffers.
    int maxDelaySamples = static_cast<int>(
                                std::ceil(MAX_DELAY_SECONDS * m_sampleRate)); 

    // Buffer size must be at least (maxDelaySamples + 1). CircularBuffer also
    // requires a size that's a power of 2.
    // Find next power of 2 greater than or equal to (maxDelaySamples + 1)
    int delayBufferSize = 1;
    while (delayBufferSize < maxDelaySamples + 1)
        delayBufferSize <<= 1;

    m_delayBuffers.clear();
    m_delayBuffers.reserve(2);
    for (int ch = 0; ch < 2; ++ch)
        m_delayBuffers.emplace_back(delayBufferSize, 0.0f);
}


void DelayEffect::prepareToPlay(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_delayTimeLowPass.setSampleRate(static_cast<float>(sampleRate));
    
    for (auto& filter : m_loopFilters)
    {
        filter.setSampleRate(static_cast<float>(sampleRate));
    }

    initDelayBuffers();
}


void DelayEffect::releaseResources()
{
    clear();
    m_delayTimeLowPass.clear();
}

void DelayEffect::setParametersFromAPVTS(
        juce::AudioProcessorValueTreeState& apvts)
{
    // Get current parameter values. These values are read once per block.
    m_mix              = *apvts.getRawParameterValue("MIX");
    m_feedback         = *apvts.getRawParameterValue("FEEDBACK");
    m_delayTime        = *apvts.getRawParameterValue("DELAY_TIME");    
    m_isPingPongOn     = *apvts.getRawParameterValue("IS_PING_PONG_ON");
    m_isBypassOn       = *apvts.getRawParameterValue("IS_BYPASS_ON");
    m_loopFilterCutoff = *apvts.getRawParameterValue("LOOP_FILTER_CUTOFF");
    m_diffusion        = *apvts.getRawParameterValue("DIFFUSION");

    auto* loopFilterTypePtr = dynamic_cast<juce::AudioParameterChoice*>(
                                    apvts.getParameter("LOOP_FILTER_TYPE"));
    
    m_loopFilterType = loopFilterTypePtr->getIndex();
}


void DelayEffect::update()
{
    // Check if toggle values changed. Clear delay buffers if so
    if (m_isBypassOn != m_lastIsBypassOn)
    {
        clear();
        m_lastIsBypassOn = m_isBypassOn;
    }
 
    if (m_isPingPongOn != m_lastIsPingPongOn)
    {
        clear();
        m_lastIsPingPongOn = m_isPingPongOn;
    }

    // Check if filter type changed. The filter state will also be cleared
    if (m_loopFilterType != m_lastLoopFilterType)
    {
        switch (m_loopFilterType)
        {
            case 0:
                for (auto& filter : m_loopFilters)
                {
                    filter.setFilterType(FilterType::lowPass);
                }
                break;
            case 1:
                for (auto& filter : m_loopFilters)
                {
                    filter.setFilterType(FilterType::highPass);
                }
                break;
            default:
                // Do nothing
                break;
        }

        m_lastLoopFilterType = m_loopFilterType;
    }

    for (auto& filter : m_loopFilters)
    {
        filter.setCutoff(m_loopFilterCutoff);
    }
}


// Assumes 2 channels
void DelayEffect::processAudioBuffer(juce::AudioBuffer<float>& buffer)
{
    if (m_isBypassOn == true)
        return;

    // Loop through each sample (outer loop is through samples instead of
    // channels so that parameter smoothing only happens once.)
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Apply smoothing to delay time slider value.
        float currentDelayTimeSeconds = m_delayTimeLowPass.getNextSample(
                                            m_delayTime) * 0.001f;

        // Get the current delay in samples. This will be the read index for the 
        // circular buffers.
        int delaySamples = static_cast<int>(
                                    currentDelayTimeSeconds * m_sampleRate);

        // Vectors for temporarily storing the current sample in each channel
        std::vector<float> inputData(2);
        std::vector<float> tempData(2);
        
        for (int channel = 0; channel < 2; channel++)
        {
            // Get the current input sample
            inputData[channel] = buffer.getWritePointer(channel)[sample];

            // Get delayed output and apply feedback gain
            tempData[channel] = m_feedback 
                                    * m_delayBuffers[channel][delaySamples];

            // Apply filter to delay output (value of 2 means no filtering)
            if (m_loopFilterType != 2)
            {
                tempData[channel] = m_loopFilters[channel].getNextSample(
                                                            tempData[channel]);
            }

            // Apply diffusion. The diffusion amount is controlled by 
            // cross-fading between the diffuser input and output
            tempData[channel] = (1.0f - m_diffusion) * tempData[channel]
                + m_diffusion * m_diffusers[channel].getNextSample(
                                                            tempData[channel]);
        }

        // Determine feedback configuration. This occurs outside of the 
        // previous loop because the two channels will not be independent if 
        // ping pong is enabled.
        if (m_isPingPongOn == true)
        {
            // Mix left and right channels to mono.
            float inputMono = (inputData[0] + inputData[1]) * 0.5f;

            // Feed left and right delay buffers into eachother. 
            // Incomming audio will be fed into the left delay.
            m_delayBuffers[0].push(inputMono + tempData[1]);
            m_delayBuffers[1].push(tempData[0]);
        }
        else
        {
            // Independent feedback loop for each channel. 
            for (int channel = 0; channel < 2; channel++)
            {
                m_delayBuffers[channel].push(
                                    inputData[channel] + tempData[channel]);
            }
        }
        
        // Get output audio for each channel. Mix dry signal with wet signal             
        for (int channel = 0; channel < 2; channel++)
        {
            auto* channelData = buffer.getWritePointer(channel);
            channelData[sample] = (1.0f - m_mix) * inputData[channel] 
                                            + m_mix * tempData[channel];
        }
    }
}


void DelayEffect::clear()
{
    for (int channel = 0; channel < 2; channel++)
    {
        m_delayBuffers[channel].clear();
        m_loopFilters[channel].clear();
        m_diffusers[channel].clear();
    }
}
