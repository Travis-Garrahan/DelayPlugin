//
//   Created by Russell Brown. Last edited on 10/12/25.
//

#ifndef LOW_PASS_1P_H
#define LOW_PASS_1P_H

// One-pole lowpass filter
class LowPass1P
{
public:
    LowPass1P(unsigned int sampleRate = 44100, float cutoffFreq = 1000);
    ~LowPass1P();
    float getNextSample(float x);
    void clear();
    void setCutoff(float cutoffFreq);
    void setSampleRate(unsigned int sampleRate);

private:
    float m_b0;
    float m_a1;
    float m_y1;
    unsigned int m_sampleRate;
    float m_cutoffFreq;
    void setCoefs();
};
#endif // LOW_PASS_1P_H
