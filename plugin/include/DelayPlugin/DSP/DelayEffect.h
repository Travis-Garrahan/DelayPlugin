#ifndef DELAY_EFFECT_H
#define DELAY_EFFECT_H

#include "CircularBuffer.h"
#include "OnePole.h"
#include "Diffuser.h"
#include <juce_audio_processors/juce_audio_processors.h>


class DelayEffect
{
private:
    static constexpr float MAX_DELAY_SECONDS = 2.0f;
    
    float m_sampleRate; 
    float m_delayTime;
    float m_feedback;
    float m_mix;
    bool m_isPingPongOn;
    bool m_lastIsPingPongOn;
    bool m_isBypassOn;
    bool m_lastIsBypassOn;

    int m_loopFilterType;
    int m_lastLoopFilterType;
    float m_loopFilterCutoff;
    
    float m_diffusion;

    std::vector<CircularBuffer<float>> m_delayBuffers;
    std::vector<OnePole<float>> m_loopFilters;
    std::vector<Diffuser<float>> m_diffusers;

    OnePole<float> m_delayTimeLowPass; 
    
    void clear();
    
public:
    DelayEffect();
    ~DelayEffect();
    void prepareToPlay(float sampleRate);
    void releaseResources();
    void setParametersFromAPVTS(juce::AudioProcessorValueTreeState& apvts);
    void update();
    void processAudioBuffer(juce::AudioBuffer<float>& buffer);
};
#endif // DELAY_EFFECT_H
