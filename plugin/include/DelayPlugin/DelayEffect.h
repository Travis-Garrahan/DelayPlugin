#ifndef DELAY_EFFECT_H
#define DELAY_EFFECT_H

#include "CircularBuffer.h"
#include "LowPass1P.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>

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

    float m_loopFilterCutoff;

    std::vector<CircularBuffer<float>> m_delayBuffers;
    LowPass1P<float> m_delayTimeLowPass; 

    LowPass1P<float> m_lowPassLeft;
    LowPass1P<float> m_lowPassRight;
    
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
