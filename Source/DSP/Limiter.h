#pragma once
#include <juce_dsp/juce_dsp.h>

class Limiter
{
public:
    Limiter() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope = 0.0f;
        gainReduction = 0.0f;
    }

    void reset()
    {
        envelope = 0.0f;
        gainReduction = 0.0f;
    }

    void setCeiling(float dB) { ceilingDb = dB; }
    void setRelease(float ms) { releaseMs = ms; }

    float getGainReduction() const { return gainReduction; }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        float ceiling = juce::Decibels::decibelsToGain(ceilingDb);
        // Very fast attack for brick-wall limiting
        float attackCoef = std::exp(-1.0f / (static_cast<float>(sampleRate) * 0.001f)); // 1ms attack
        float releaseCoef = std::exp(-1.0f / (static_cast<float>(sampleRate) * releaseMs * 0.001f));

        float maxGR = 0.0f;

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            // Get max input level across channels
            float inputLevel = 0.0f;
            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                inputLevel = std::max(inputLevel, std::abs(block.getChannelPointer(ch)[sample]));
            }

            // Envelope follower
            if (inputLevel > envelope)
                envelope = attackCoef * envelope + (1.0f - attackCoef) * inputLevel;
            else
                envelope = releaseCoef * envelope + (1.0f - releaseCoef) * inputLevel;

            // Calculate limiter gain
            float gain = 1.0f;
            if (envelope > ceiling)
            {
                gain = ceiling / envelope;
            }

            // Apply gain
            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                float& sample_val = block.getChannelPointer(ch)[sample];
                sample_val *= gain;

                // Hard clip as safety net
                sample_val = juce::jlimit(-ceiling, ceiling, sample_val);
            }

            // Track max gain reduction
            float gr = 1.0f - gain;
            if (gr > maxGR) maxGR = gr;
        }

        gainReduction = juce::Decibels::gainToDecibels(1.0f - maxGR + 0.0001f);
    }

private:
    double sampleRate = 44100.0;
    float ceilingDb = -0.3f;
    float releaseMs = 100.0f;

    float envelope = 0.0f;
    float gainReduction = 0.0f;
};
