///
///    @file IAudioFilter.h
///    @brief Interface for audio processing objects.
///    @author Russell Brown
///    @date November 15, 2025 
///
///    Interface for audio processing objects. Derived classes must override 
///    the virtual method 'FloatType getNextSample(FloatType x)'.
///

#ifndef I_AUDIO_FILTER_H
#define I_AUDIO_FILTER_H

#include <concepts>

/**
 * @brief Interface for audio processing objects.
 * @author Russell Brown
 */
template <std::floating_point FloatType>
class IAudioFilter
{
public:
    virtual FloatType getNextSample(FloatType x) = 0;
    virtual ~IAudioFilter() = default;
};

#endif // I_AUDIO_FILTER_H
