//
//   Created by Russell Brown. Last edited on 10/12/25.
//
//   References:  https://ccrma.stanford.edu/~jos/fp/One_Pole.html
//                https://dspguide.com/ch19/2.htm 
//

#include "DelayPlugin/LowPass1P.h"
#include <cmath>

LowPass1P::LowPass1P(unsigned int sampleRate, float cutoffFreq) 
    : m_sampleRate{sampleRate}, m_b0{0.0f}, m_a1{0.0f}, m_y1{0.0f}
{
    setCutoff(cutoffFreq);
}

LowPass1P::~LowPass1P()
{
}

float LowPass1P::getNextSample(float x)
{
    /*  
     *  Implements the difference equation y[n] = b0 * x[n] - a1 * y[n-1]
     *
     *      x[n] is the current input sample.
     *      y[n] is the current output sample. 
     *      y[n-1] is the previous output sample. 
     *      b0 is the feedforward coefficient.
     *      a1 is the feedback coefficient.
     *
     *  b0 is set to equal 1 - |a1| and a1 is set to be negative. Each output is essentially 
     *  a weighted average of the current input sample and the previous output sample.
     *
     *  Increasing |a1| causes the filter to be more influenced by its current state (previous 
     *  output) and less influenced by its input. This makes it less responsive when its input 
     *  changes, resulting in a more-smoothed output signal. This also corresponds to a lower 
     *  cutoff frequency.
     *
     *  |a1| must be < 1 to prevent the output from blowing up.
     *
     */
    float y = m_b0 * x - m_a1 * m_y1;
    m_y1 = y; 

    return y;
}

void LowPass1P::clear()
{
    // Reset filter state 
    m_y1 = 0.0f;
}

void LowPass1P::setCutoff(float cutoffFreq)
{
    // Set the cutoff frequency in Hz. This is the frequency where the 
    // gain is reduced by 3dB.

    // Restrict cutoff frequency to be betweem 1 Hz and the Nyquist frequency. 
    // (min frequency needs to be greater than zero so that |a1| < 1. 
    // Nyquist frequency is the max frequency that can be represented.)
    float nyquist = m_sampleRate / 2.0f;
    
    if (cutoffFreq < 1.0f)
        cutoffFreq = 1.0f;
    
    else if (cutoffFreq > nyquist)
        cutoffFreq = nyquist;

    m_cutoffFreq = cutoffFreq;

    setCoefs();
}

void LowPass1P::setSampleRate(unsigned int sampleRate)
{
    m_sampleRate = sampleRate;

    setCoefs();
}

void LowPass1P::setCoefs()
{
    // Calculate filter coefficients to set the desired cutoff frequency. 
    
    // Convert cutoff in Hz to angular frequency (radians per second)
    const float TWO_PI = 6.28318530718f;
    float omega = TWO_PI * m_cutoffFreq;

    m_a1 = -std::exp(-omega / m_sampleRate);

    // Normalize
    m_b0 = 1.0f - std::abs(m_a1);
}
