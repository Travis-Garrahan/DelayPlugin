#ifndef DELAY_EFFECT_H
#define DELAY_EFFECT_H

#include "DelayLine.h"
#include "OnePole.h"
#include "Diffuser.h"
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
    
    float m_diffusion;

    DelayLine<float> m_delayLineLeft;
    DelayLine<float> m_delayLineRight;
    OnePole<float> m_delayTimeLowPass; 

    OnePole<float> m_loopFilterLeft;
    OnePole<float> m_loopFilterRight;
    
    Diffuser<float> m_diffuserLeft;
    Diffuser<float> m_diffuserRight;

    void clear();
    
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
