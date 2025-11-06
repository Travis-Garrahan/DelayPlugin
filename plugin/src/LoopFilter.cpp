#include "DelayPlugin/LoopFilter.h"


LoopFilter::LoopFilter() : m_filterType{FilterType::none}
{
    m_lowPass.useApproxCutoff(true);
}


LoopFilter::~LoopFilter()
{
}


float LoopFilter::getNextSample(float x)
{
    float y{};

    using enum FilterType;
    
    switch (m_filterType)
    {
        case lowPass:
            y = m_lowPass.getNextSample(x);
            break;

        case highPass:
            // Get high pass by subtracting low pass output from original signal
            y = x - m_lowPass.getNextSample(x);
            break;

        case none:
            y = x;
            break;
            
        default:
            y = x;
            break;
    }
    return y;
}


void LoopFilter::setFilterType(FilterType filterType)
{
    m_filterType = filterType;
}


void LoopFilter::setCutoff(float cutoff)
{
    m_lowPass.setCutoff(cutoff);
}

void LoopFilter::clear()
{
    m_lowPass.clear();
}

void LoopFilter::setSampleRate(float sampleRate)
{
    m_lowPass.setSampleRate(sampleRate);
}
