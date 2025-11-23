#include "DelayPlugin/DSP/DelayEffect.h"
#include <algorithm>

DelayEffect::DelayEffect() : m_sampleRate{}, m_delayTime{}, m_feedback{}, 
    m_mix{}, m_isPingPongOn{}, m_lastIsPingPongOn{}, m_isBypassOn{}, 
    m_lastIsBypassOn{}, m_loopFilterType{}, m_lastLoopFilterType{}, 
    m_loopFilterCutoff{}, m_diffusion{},
    // Diffuser delay lengths based on Freeverb 
    // ccrma.stanford.edu/~jos/pasp/Freeverb.html
    m_diffuserLeft(4, {225, 556, 441, 341}, {0.7f, 0.7f, 0.7f, 0.7f}), 
    m_diffuserRight(4, {225, 556, 441, 341}, {0.7f, 0.7f, 0.7f, 0.7f})
{
    // Delay time smoothing filter will have a fixed cutoff frequency of 1 Hz.
    m_delayTimeLowPass.setCutoff(1.0f); 
}


DelayEffect::~DelayEffect()
{
}


void DelayEffect::prepareToPlay(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_delayTimeLowPass.setSampleRate(static_cast<float>(sampleRate));
    
    m_loopFilterLeft.setSampleRate(static_cast<float>(sampleRate));
    m_loopFilterRight.setSampleRate(static_cast<float>(sampleRate));

    m_delayLineLeft.setSampleRate(static_cast<float>(sampleRate));
    m_delayLineRight.setSampleRate(static_cast<float>(sampleRate));

    m_delayLineLeft.setMaxDelay(MAX_DELAY_SECONDS * 1000.f);
    m_delayLineRight.setMaxDelay(MAX_DELAY_SECONDS * 1000.f);
}


void DelayEffect::releaseResources()
{
    clear();
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
    m_loopFilterCutoff = *apvts.getRawParameterValue("LOOP_FILTER_CUTOFF");
    m_diffusion = *apvts.getRawParameterValue("DIFFUSION");

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
                m_loopFilterLeft.setFilterType(FilterType::lowPass);
                m_loopFilterRight.setFilterType(FilterType::lowPass);
                break;
            case 1:
                m_loopFilterLeft.setFilterType(FilterType::highPass);
                m_loopFilterRight.setFilterType(FilterType::highPass);
                break;
            default:
                // Do nothing
                break;
        }

        m_lastLoopFilterType = m_loopFilterType;
    }

    m_loopFilterLeft.setCutoff(m_loopFilterCutoff);
    m_loopFilterRight.setCutoff(m_loopFilterCutoff);
}


void DelayEffect::processAudioBuffer(juce::AudioBuffer<float>& buffer)
{
    if (m_isBypassOn == true)
        return;
    
    // Get left and right audio buffers. Each contains the input data, which 
    // is processed by overwriting it
    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    // Loop through each sample
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        // Get the current input sample for each channel
        float inputLeft = channelDataLeft[i];
        float inputRight = channelDataRight[i];

        // Apply smoothing to delay time slider value.
        float currentDelayMs = m_delayTimeLowPass.getNextSample(m_delayTime);

        m_delayLineLeft.setDelay(currentDelayMs);
        m_delayLineRight.setDelay(currentDelayMs);

        // Get delayed outputs and apply feedback gain.
        float delayedLeft = m_feedback * m_delayLineLeft.read();
        float delayedRight = m_feedback * m_delayLineRight.read();

        // Apply filter to delay output
        if (m_loopFilterType != 2)
        {
            delayedLeft = m_loopFilterLeft.getNextSample(delayedLeft);
            delayedRight = m_loopFilterRight.getNextSample(delayedRight);
        }

        // Apply diffusion. The diffusion amount is controlled by cross-fading 
        // between the diffuser input and output
        delayedLeft = (1.0f - m_diffusion) * delayedLeft 
            + m_diffusion * m_diffuserLeft.getNextSample(delayedLeft);
        
        delayedRight = (1.0f - m_diffusion) * delayedRight 
            + m_diffusion * m_diffuserRight.getNextSample(delayedRight);

        // Determine feedback configuration 
        if (m_isPingPongOn == true)
        {
            // Mix left and right channels to mono.
            float inputMono = (inputLeft + inputRight) / 2.0f;

            // Feed left and right delay buffers into eachother. 
            // Incomming audio will be fed into the left delay.
            m_delayLineLeft.write(inputMono + delayedRight);
            m_delayLineRight.write(delayedLeft);
        }
        else
        {
            // Independent feedback loop for each channel. 
            m_delayLineLeft.write(inputLeft + delayedLeft);
            m_delayLineRight.write(inputRight + delayedRight);
        }
        
        // Get output audio for each channel. Mix dry signal with wet signal             
        float outputLeft = (1.0f - m_mix) * inputLeft + m_mix * delayedLeft;	    
        float outputRight = (1.0f - m_mix) * inputRight + m_mix * delayedRight;

        // Write output to the audio buffer
        channelDataLeft[i] = outputLeft;
        channelDataRight[i] = outputRight;
    }
}


void DelayEffect::clear()
{
    m_delayLineLeft.clear();
    m_delayLineRight.clear();

    m_diffuserLeft.clear();
    m_diffuserRight.clear();

    m_loopFilterLeft.clear();
    m_loopFilterRight.clear();
}
