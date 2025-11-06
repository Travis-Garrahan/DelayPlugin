#ifndef LOOP_FILTER_H
#define LOOP_FILTER_H

#include "LowPass1P.h"

/* Filter that can switch between lowpass, highpass, and none. */

enum class FilterType
{
    lowPass,
    highPass,
    none
};


class LoopFilter
{
private:
    FilterType m_filterType;
    LowPass1P<float> m_lowPass;
    
public:
    LoopFilter();
    ~LoopFilter();
    float getNextSample(float x);
    void setFilterType(FilterType filterType);
    void setSampleRate(float sampleRate);
    void setCutoff(float cutoff);
    void clear();
};
#endif // LOOP_FILTER_H
