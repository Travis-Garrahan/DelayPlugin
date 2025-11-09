//
//   Created by Russell Brown. Last edited on 11/9/25.
//
//   References:  https://ccrma.stanford.edu/~jos/fp/One_Pole.html
//                https://dspguide.com/ch19/2.htm 
//                https://msp.ucsd.edu/techniques/latest/book-html/node140.html
//

#ifndef ONE_POLE_H
#define ONE_POLE_H

#include <cmath>
#include <concepts>
#include <algorithm>


enum class FilterType { lowPass, highPass };


/* Single-pole low-pass/high-pass filter */
template<std::floating_point FloatType>
class OnePole
{
public:
    OnePole(FilterType filterType = FilterType::lowPass, FloatType sampleRate = FloatType(44100), 
                    FloatType cutoffFreq = FloatType(1000));
    ~OnePole();
    FloatType getNextSample(FloatType x);
    void clear();
    void setCutoff(FloatType cutoffFreq);
    void setSampleRate(FloatType sampleRate);
    void useApproxCutoff(bool useApprox);
    void setFilterType(FilterType filterType);

private:
    FloatType m_b0;
    FloatType m_b1;
    FloatType m_a1;
    FloatType m_x1; 
    FloatType m_y1; 
    FloatType m_sampleRate;
    FloatType m_cutoffFreq;
    bool m_useApprox;
    FilterType m_filterType;
    void setCoefs();
};


template<std::floating_point FloatType>
OnePole<FloatType>::OnePole(FilterType filterType, FloatType sampleRate, FloatType cutoffFreq) 
    : m_filterType{filterType}, m_sampleRate{sampleRate}, m_b0{}, m_b1{}, m_a1{}, 
      m_x1{}, m_y1{}, m_useApprox{true}
{
    setCutoff(cutoffFreq);
}


template<std::floating_point FloatType>
OnePole<FloatType>::~OnePole()
{
}


template<std::floating_point FloatType>
FloatType OnePole<FloatType>::getNextSample(FloatType x)
{
/*  
 *  Implements the difference equation 
 *
 *      y[n] = b0 * x[n] + b1 * x[n-1] - a1 * y[n-1]
 *
 *      x[n] is the current input sample.
 *      x[n-1] is the previous input sample.
 *      y[n] is the current output sample. 
 *      y[n-1] is the previous output sample. 
 *      b0 and b1 are the feedforward coefficients.
 *      a1 is the feedback coefficient.
 *
 *  For a low-pass filter, b1 is set to 0, so the difference equation is simply
 *
 *      y[n] = b0 * x[n] - a1 * y[n-1]
 *
 *  a1 is made to be negative, so this is essentially a weighted average of the last 
 *  output sample and the current input sample. The b1 term is needed for the high-pass 
 *  filter so that the amplitude at DC (0 Hz) is 0.
 *
 *  |a1| must be < 1 to prevent the output from blowing up.
 *
 */
    FloatType y = m_b0 * x + m_b1 * m_x1 - m_a1 * m_y1;
    m_x1 = x;
    m_y1 = y; 

    return y;
}


template<std::floating_point FloatType>
void OnePole<FloatType>::clear()
{
    // Reset filter state 
    m_x1 = FloatType(0);
    m_y1 = FloatType(0);
}


template<std::floating_point FloatType>
void OnePole<FloatType>::setCutoff(FloatType cutoffFreq)
{
    // Set the cutoff frequency in Hz. This is the frequency where the 
    // gain is reduced by 3dB.

    // Restrict cutoff frequency to be betweem 1 Hz and the Nyquist frequency. 
    // (min frequency needs to be greater than zero so that |a1| < 1. 
    // Nyquist frequency is the max frequency that can be represented.)
    FloatType nyquist = m_sampleRate / FloatType(2);

    m_cutoffFreq = std::clamp(cutoffFreq, FloatType(1), nyquist);

    setCoefs();
}


template<std::floating_point FloatType>
void OnePole<FloatType>::setSampleRate(FloatType sampleRate)
{
    m_sampleRate = sampleRate;

    setCoefs();
}


template<std::floating_point FloatType>
void OnePole<FloatType>::setCoefs()
{
    // Calculate filter coefficients to set the desired cutoff frequency. 
    
    // Convert cutoff in Hz to angular frequency (radians per second)
    const FloatType TWO_PI = FloatType(6.28318530718);
    FloatType omega = TWO_PI * m_cutoffFreq;

    // Calculate the feedback (a1) coefficient, optionally using a fast approximation.
    if (m_useApprox)
    {
        // First order Taylor approximation (avoids using std::exp). Innacurate 
        // for very high frequencies. 
        m_a1 = omega / m_sampleRate - FloatType(1);

        // Need to do an additional check to keep a1 between 0 and -1
        m_a1 = std::clamp(m_a1, FloatType(-1), FloatType(0));
    }

    else
    {
        m_a1 = -std::exp(-omega / m_sampleRate);
    }

    // Set feedforward coefficients (b0 and b1) depending on the filter type.
    if (m_filterType == FilterType::lowPass)
    {
        m_b0 = FloatType(1) - std::abs(m_a1);
        m_b1 = FloatType(0);
    }

    else if (m_filterType == FilterType::highPass)
    {
        m_b0 = (FloatType(1) - m_a1) / FloatType(2);
        m_b1 = -m_b0;
    }
}


template<std::floating_point FloatType>
void OnePole<FloatType>::useApproxCutoff(bool useApprox)
{
    m_useApprox = useApprox;    
    setCoefs();
}

template<std::floating_point FloatType>
void OnePole<FloatType>::setFilterType(FilterType filterType)
{
    m_filterType = filterType;
    setCoefs();
    clear();
}

#endif // ONE_POLE_H
