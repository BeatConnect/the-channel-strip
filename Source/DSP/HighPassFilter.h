#pragma once
#include <juce_dsp/juce_dsp.h>

class HighPassFilter
{
public:
    HighPassFilter() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        for (auto& filter : filters)
        {
            filter.prepare(spec);
            filter.reset();
        }

        updateCoefficients();
    }

    void reset()
    {
        for (auto& filter : filters)
            filter.reset();
    }

    void setFrequency(float freq)
    {
        if (frequency != freq)
        {
            frequency = freq;
            updateCoefficients();
        }
    }

    void setSlope(int slopeIndex)
    {
        // 0 = 12 dB/oct (2 poles), 1 = 18 dB/oct (3 poles), 2 = 24 dB/oct (4 poles)
        int newOrder = (slopeIndex + 1) * 2;
        if (filterOrder != newOrder)
        {
            filterOrder = newOrder;
            updateCoefficients();
        }
    }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        // Apply cascaded filters based on order
        int numFiltersToUse = filterOrder / 2;

        for (int i = 0; i < numFiltersToUse && i < maxFilters; ++i)
        {
            filters[i].process(context);
        }
    }

private:
    void updateCoefficients()
    {
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, frequency);

        for (auto& filter : filters)
        {
            *filter.state = *coeffs;
        }
    }

    double sampleRate = 44100.0;
    float frequency = 80.0f;
    int filterOrder = 4; // 24 dB/oct default

    static constexpr int maxFilters = 4;
    std::array<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
               juce::dsp::IIR::Coefficients<float>>, maxFilters> filters;
};
