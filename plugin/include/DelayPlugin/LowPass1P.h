//
//   Created by Russell Brown. Last edited on 11/4/25.
//
//   References:  https://ccrma.stanford.edu/~jos/fp/One_Pole.html
//                https://dspguide.com/ch19/2.htm 
//                https://msp.ucsd.edu/techniques/latest/book-html/node140.html
//

#ifndef LOW_PASS_1P_H
#define LOW_PASS_1P_H

#include <cmath>
#include <concepts>


// One-pole lowpass filter
template<std::floating_point FloatType>
class LowPass1P
{
public:
    LowPass1P(unsigned int sampleRate = 44100, FloatType cutoffFreq = FloatType(1000));
    ~LowPass1P();
    FloatType getNextSample(FloatType x);
    void clear();
    void setCutoff(FloatType cutoffFreq);
    void setSampleRate(unsigned int sampleRate);
    void useApproxCutoff(bool useApprox);

private:
    FloatType m_b0;
    FloatType m_a1;
    FloatType m_y1;
    unsigned int m_sampleRate;
    FloatType m_cutoffFreq;
    bool m_useApprox;
    void setCoefs();
};


template<std::floating_point FloatType>
LowPass1P<FloatType>::LowPass1P(unsigned int sampleRate, FloatType cutoffFreq) 
    : m_sampleRate{sampleRate}, m_b0{}, m_a1{}, m_y1{}
{
    setCutoff(cutoffFreq);
}


template<std::floating_point FloatType>
LowPass1P<FloatType>::~LowPass1P()
{
}


template<std::floating_point FloatType>
FloatType LowPass1P<FloatType>::getNextSample(FloatType x)
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
    FloatType y = m_b0 * x - m_a1 * m_y1;
    m_y1 = y; 

    return y;
}


template<std::floating_point FloatType>
void LowPass1P<FloatType>::clear()
{
    // Reset filter state 
    m_y1 = FloatType(0);
}


template<std::floating_point FloatType>
void LowPass1P<FloatType>::setCutoff(FloatType cutoffFreq)
{
    // Set the cutoff frequency in Hz. This is the frequency where the 
    // gain is reduced by 3dB.

    // Restrict cutoff frequency to be betweem 1 Hz and the Nyquist frequency. 
    // (min frequency needs to be greater than zero so that |a1| < 1. 
    // Nyquist frequency is the max frequency that can be represented.)
    FloatType nyquist = m_sampleRate / FloatType(2);
    
    if (cutoffFreq < FloatType(1))
        cutoffFreq = FloatType(1);
    
    else if (cutoffFreq > nyquist)
        cutoffFreq = nyquist;

    m_cutoffFreq = cutoffFreq;

    setCoefs();
}


template<std::floating_point FloatType>
void LowPass1P<FloatType>::setSampleRate(unsigned int sampleRate)
{
    m_sampleRate = sampleRate;

    setCoefs();
}


template<std::floating_point FloatType>
void LowPass1P<FloatType>::setCoefs()
{
    // Calculate filter coefficients to set the desired cutoff frequency. 
    
    // Convert cutoff in Hz to angular frequency (radians per second)
    const FloatType TWO_PI = FloatType(6.28318530718);
    FloatType omega = TWO_PI * m_cutoffFreq;
    
    if (m_useApprox)
    {
        // Taylor approximation that avoids using std::exp. Innacurate for very high 
        // frequencies, but good enough for most audio purposes. 
        m_a1 = omega / m_sampleRate - FloatType(1);

        // Need to do an additional check to keep a1 between 0 and -1
        if (m_a1 < FloatType(-1))
            m_a1 = FloatType(-1);

        else if (m_a1 > FloatType(0))
            m_a1 = FloatType(0);
    }
    else
    {
        m_a1 = -std::exp(-omega / m_sampleRate);
    }

    // Normalize
    m_b0 = FloatType(1) - std::abs(m_a1);
}


template<std::floating_point FloatType>
void LowPass1P<FloatType>::useApproxCutoff(bool useApprox)
{
    m_useApprox = useApprox;    
}

#endif // LOW_PASS_1P_H
