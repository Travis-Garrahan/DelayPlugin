///
///     @file Diffuser.h
///     @brief Multi-stage all-pass diffuser.
///     @author Russell Brown
///     @date November 17, 2025
///
///     This class implements a multi-stage diffuser, useful for adding  
///     high-density echoes to an input audio signal. It consists of a 
///     user-specified number of Schroeder all-pass filters arranged in 
///     series. 
///
///     @see Schroeder
///

#ifndef DIFFUSER_H
#define DIFFUSER_H

#include "Schroeder.h"
#include "IAudioFilter.h"
#include <concepts>
#include <vector>
#include <stdexcept>

/**
 * @class Diffuser
 * 
 * @brief Multi-stage all-pass diffuser.
 *
 * @implements IAudioFilter
 */ 
template<std::floating_point FloatType>
class Diffuser : public IAudioFilter<FloatType>
{
private:
    std::vector<Schroeder<FloatType>> allPassSections;

public:
    Diffuser(unsigned int numStages, const std::vector<unsigned int>& delaySizes, 
                const std::vector<FloatType>& gains); 
    ~Diffuser();
    FloatType getNextSample(FloatType x) override;
    void clear();
};

// TODO: Add methods setDelaySizes and setGains


// Constructor
/**
 * Construct a diffuser object.
 *
 * @param numStages     The number of Schroeder all-pass sections.
 *
 * @param delayLengths  A vector containing the delay lengths in samples for 
 *                      each all-pass section.
 *                      
 * @param gains         A vector containing the gain coefficients for each 
 *                      all-pass section.
 */
template<std::floating_point FloatType>
Diffuser<FloatType>::Diffuser(unsigned int numStages, 
        const std::vector<unsigned int>& delayLengths, 
        const std::vector<FloatType>& gains)
{
    if (delayLengths.size() != numStages) 
        throw std::invalid_argument("delaySizes.size() must match numStages");

    if (gains.size() != numStages)
        throw std::invalid_argument("gains.size() must match numStages");

    for (unsigned int i = 0; i < numStages; i++)
        allPassSections.push_back( Schroeder<FloatType>(delayLengths[i], 
                                                        gains[i] ));
}


template<std::floating_point FloatType>
Diffuser<FloatType>::~Diffuser()
{
}


/** 
 * Process an audio sample.
 *
 * @param x    The input sample.
 *
 * @return     The output sample.
 */
template<std::floating_point FloatType>
FloatType Diffuser<FloatType>::getNextSample(FloatType x) 
{
    FloatType y = x;

    // Apply each all-pass filter in series
    for (Schroeder<FloatType>& allPass : allPassSections)
        y = allPass.getNextSample(y);

    return y;
}


/**
 * Clear each all-pass filter's delay buffer.
 */
template<std::floating_point FloatType>
void Diffuser<FloatType>::clear()
{
    for (Schroeder<FloatType>& allPass : allPassSections)
        allPass.clear();
}

#endif // DIFFUSER_H
