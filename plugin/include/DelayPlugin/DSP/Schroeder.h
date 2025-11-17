///
///     @file Schroeder.h
///     @brief Schroeder all-pass section.
///     @author Russell Brown
///     @date November 17, 2025
///
///     This class implements a Schroeder all-pass filter, originally 
///     described in "Colorless Artificial Reverberation" by Manfred 
///     Schroeder. This structure is useful for adding diffusion to an
///     input signal. Several filters are typically added in series to 
///     create high-density echoes, characteristic of the late reflections
///     that occur in reverb.
///
///     @see Diffuser
///
///     Reference: ccrma.stanford.edu/~jos/pasp/Allpass_Two_Combs.html
///

#ifndef SCHROEDER_H
#define SCHROEDER_H

#include "CircularBuffer.h"
#include "IAudioFilter.h"
#include <concepts>
#include <juce_core/juce_core.h>
#include <stdexcept>
#include <memory>


/**
 * @class Schroeder
 *
 * @brief Schroeder all-pass section.
 *
 * @implements IAudioFilter
 */
template<std::floating_point FloatType>
class Schroeder : public IAudioFilter<FloatType>
{
private:
    FloatType m_gain;
    unsigned int m_delayInSamples;
    CircularBuffer<FloatType> m_delayBuffer;

public:
    Schroeder(unsigned int delayInSamples, FloatType gain);
    ~Schroeder();
    FloatType getNextSample(FloatType x) override;
    void setGain(FloatType gain);     
    void setDelaySamples(unsigned int delayInSamples);
    void clear();
};


// Constructor
/**
 * Construct a Schroeder object.
 *
 * @param delayInSamples    The delay length in samples. Requires a value of at
 *                          least 1.
 *
 * @param gain              The feedback/feedforward gain. Requires a value 
 *                          between 0 and 1. 
 */
template<std::floating_point FloatType>
Schroeder<FloatType>::Schroeder(unsigned int delayInSamples, FloatType gain) 
    : m_gain{gain}, m_delayInSamples{delayInSamples}, 
      m_delayBuffer(juce::nextPowerOfTwo(delayInSamples), FloatType(0))
      
//      m_delayBuffer(new CircularBuffer<FloatType>(
//                            juce::nextPowerOfTwo(delayInSamples), 
//                            FloatType(0)))
{
    if ( (gain < FloatType(0)) || (gain > FloatType(1)) )
        throw std::invalid_argument("gain must be between 0 and 1");

    if (delayInSamples < 1)
        throw std::invalid_argument("delayInSamples must be at least 1");
}


/**
 * Destructor
 */
template<std::floating_point FloatType>
Schroeder<FloatType>::~Schroeder()
{
}


/**
 * Process an audio sample.
 *
 * @param x    The input sample to be processed.
 *
 * @return     Returns the processed output sample.
 */
template<std::floating_point FloatType>
FloatType Schroeder<FloatType>::getNextSample(FloatType x)
{
    unsigned int readIndex = static_cast<unsigned int>(
                                m_delayBuffer.size) - m_delayInSamples;
    
    FloatType delayed = m_delayBuffer[readIndex];
    FloatType mixed = x + m_gain * delayed;
    FloatType y = -m_gain * mixed + delayed;

    m_delayBuffer.push(mixed);

    return y;
}


/**
 * Set the gain coefficient of the all-pass filter.
 *
 * @param gain    The gain coefficient.
 */
template <std::floating_point FloatType>
void Schroeder<FloatType>::setGain(FloatType gain)     
{
    if ( (gain < FloatType(0)) || (gain > FloatType(1)) )
        throw std::invalid_argument("gain must be between 0 and 1");

    m_gain = gain;
}


/**
 * Set the delay length in samples.
 *
 * @param delayInSamples    The all-pass delay length in samples.
 */
template <std::floating_point FloatType>
void Schroeder<FloatType>::setDelaySamples(unsigned int delayInSamples)
{
    // FIXME: Needs implementation 
}


/**
 * Clear the delay buffer.
 */
template <std::floating_point FloatType>
void Schroeder<FloatType>::clear()
{
    m_delayBuffer->clear();
}

#endif // SCHROEDER_H
