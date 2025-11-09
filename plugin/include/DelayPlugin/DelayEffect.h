#ifndef DELAY_EFFECT_H
#define DELAY_EFFECT_H

#include "CircularBuffer.h"
#include "OnePole.h"
#include <juce_audio_processors/juce_audio_processors.h>

class DelayEffect
{
private:
    static constexpr float MAX_DELAY_SECONDS = 2.0f;
    
    double m_sampleRate; 
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

    std::vector<CircularBuffer<float>> m_delayBuffers;
    OnePole<float> m_delayTimeLowPass; 

    OnePole<float> m_loopFilterLeft;
    OnePole<float> m_loopFilterRight;
    
    void initDelayBuffers();
    void clearDelayBuffers();

public:
    DelayEffect();
    ~DelayEffect();
    void prepareToPlay(double sampleRate);
    void releaseResources();
    void setParametersFromAPVTS(juce::AudioProcessorValueTreeState& apvts);
    void update();
    void processAudioBuffer(juce::AudioBuffer<float>& buffer);
};
#endif // DELAY_EFFECT_H
