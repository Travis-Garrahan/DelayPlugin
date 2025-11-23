///
///     @file DelayLine.h
///     @brief Interpolating delay line using all-pass interpolation.
///     @author Russell Brown
///     @date November 22, 2025
///
///     Interpolating delay line using first-order all-pass interpolation.
///

#ifndef DELAY_LINE_H
#define DELAY_LINE_H

#include "CircularBuffer.h"
#include <concepts>
#include <cmath>
#include <algorithm>


template <std::floating_point FloatType>
class DelayLine
{
private:
    CircularBuffer<FloatType> m_delayBuffer;
    FloatType m_sampleRate;

    unsigned int m_maxDelaySamples;
    unsigned int m_delaySamples;    // Integral delay
    FloatType m_frac;               // Fractional delay
    
    FloatType m_x1;                 // Memory of past input/output
    FloatType m_y1;                 // to interpolation filter

public:
    DelayLine();
    FloatType read();
    void write(FloatType x);
    void setDelay(FloatType delayMs);
    void setSampleRate(FloatType sampleRate);
    void setMaxDelay(FloatType maxDelayMs);
    void clear();
};


template <std::floating_point FloatType>
DelayLine<FloatType>::DelayLine() : m_delayBuffer(1024, FloatType(0)), 
    m_sampleRate{}, m_maxDelaySamples{}, m_delaySamples{}, m_frac{}, 
        m_x1{}, m_y1{}
{
}


template <std::floating_point FloatType>
void DelayLine<FloatType>::setDelay(FloatType delayMs)
{
    FloatType delaySamplesFloat = m_sampleRate * delayMs / FloatType(1000);

    // Separate the delay amount into integer and fractional parts
    unsigned int delaySamplesInt = static_cast<unsigned int>(delaySamplesFloat);

    if (delaySamplesInt < m_maxDelaySamples)
    {
        m_delaySamples = delaySamplesInt;
        m_frac = delaySamplesFloat - static_cast<float>(m_delaySamples);
    }

    else
    {
        m_delaySamples = m_maxDelaySamples;
        m_frac = FloatType(0);
    }
}


template <std::floating_point FloatType>
void DelayLine<FloatType>::setSampleRate(FloatType sampleRate)
{
    m_sampleRate = sampleRate;
}


template <std::floating_point FloatType>
void DelayLine<FloatType>::setMaxDelay(FloatType maxDelayMs)
{
    m_maxDelaySamples = static_cast<unsigned int>(
                    std::ceil(m_sampleRate * maxDelayMs / FloatType(1000)));

    // Find next power of 2 for buffer size
    unsigned int delayBufferSize = 1;
    while (delayBufferSize < (m_maxDelaySamples + 1))
        delayBufferSize <<= 1;

    m_delayBuffer.resize(static_cast<size_t>(delayBufferSize));
}


template <std::floating_point FloatType>
void DelayLine<FloatType>::write(FloatType x)
{
    m_delayBuffer.push(x);
}


template <std::floating_point FloatType>
FloatType DelayLine<FloatType>::read()
{    
    float x = m_delayBuffer[m_delaySamples];

    // Interpolate
    float y = m_frac * (x - m_y1) + m_x1;
    m_x1 = x;
    m_y1 = y;
    return y;
}


template <std::floating_point FloatType>
void DelayLine<FloatType>::clear()
{
    m_delayBuffer.clear();
    m_x1 = FloatType(0);
    m_y1 = FloatType(0);
}

#endif // DELAY_LINE_H
