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
    std::vector<Schroeder<FloatType>> m_allPassSections;

public:
    Diffuser(unsigned int numStages, const std::vector<unsigned int>& delayLengths, 
                const std::vector<FloatType>& gains); 
    ~Diffuser();
    FloatType getNextSample(FloatType x) override;
    void clear();
    void setDelayLengths(const std::vector<unsigned int>& delayLengths);
    void setGains(const std::vector<FloatType>& gains);
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
        throw std::invalid_argument("delayLengths.size() must match numStages");

    if (gains.size() != numStages)
        throw std::invalid_argument("gains.size() must match numStages");

    for (unsigned int i = 0; i < numStages; i++)
        m_allPassSections.push_back( Schroeder<FloatType>(delayLengths[i], 
                                                          gains[i] ));
}



// Destructor
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
    for (Schroeder<FloatType>& allPass : m_allPassSections)
        y = allPass.getNextSample(y);

    return y;
}


/**
 * Clear each all-pass filter's delay buffer.
 */
template<std::floating_point FloatType>
void Diffuser<FloatType>::clear()
{
    for (Schroeder<FloatType>& allPass : m_allPassSections)
        allPass.clear();
}


/**
 * Set the delay length in samples for each all-pass filter.
 *
 * @param delayLengths  A vector containing the delay lengths in samples for 
 *                      each all-pass section.
 */
template<std::floating_point FloatType>
void Diffuser<FloatType>::setDelayLengths(const std::vector<unsigned int>& delayLengths)
{
    if (delayLengths.size() != m_allPassSections.size()) 
        throw std::invalid_argument("delayLengths.size() must match numStages");

    for (size_t i = 0; i < m_allPassSections.size(); i++)
        m_allPassSections[i].setDelayLength(delayLengths[i]);
}


/**
 * Set the gain coefficient for each all-pass filter.
 *
 * @param gains   A vector containing the gain coefficients for each 
 *                all-pass section.
 */
template<std::floating_point FloatType>
void Diffuser<FloatType>::setGains(const std::vector<FloatType>& gains)
{
    if (gains.size() != m_allPassSections.size()) 
        throw std::invalid_argument("gains.size() must match numStages");

    for (size_t i = 0; i < m_allPassSections.size(); i++)
        m_allPassSections[i].setGain(gains[i]);
}


#endif // DIFFUSER_H
