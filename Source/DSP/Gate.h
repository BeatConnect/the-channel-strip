#pragma once
#include <juce_dsp/juce_dsp.h>

class Gate
{
public:
    Gate() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope = 0.0f;
        currentGain = 1.0f;
        gainReduction = 0.0f;
    }

    void reset()
    {
        envelope = 0.0f;
        currentGain = 1.0f;
        gainReduction = 0.0f;
    }

    void setThreshold(float dB) { thresholdDb = dB; }
    void setAttack(float ms) { attackMs = ms; }
    void setRelease(float ms) { releaseMs = ms; }
    void setRange(float dB) { rangeDb = dB; }

    float getGainReduction() const { return gainReduction; }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        float threshold = juce::Decibels::decibelsToGain(thresholdDb);
        float rangeGain = juce::Decibels::decibelsToGain(rangeDb);
        float attackCoef = std::exp(-1.0f / (static_cast<float>(sampleRate) * attackMs * 0.001f));
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

            // Calculate gate gain
            float targetGain = 1.0f;
            if (envelope < threshold)
            {
                targetGain = rangeGain;
            }

            // Smooth gain changes
            if (targetGain < currentGain)
                currentGain = attackCoef * currentGain + (1.0f - attackCoef) * targetGain;
            else
                currentGain = releaseCoef * currentGain + (1.0f - releaseCoef) * targetGain;

            // Apply gain
            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                block.getChannelPointer(ch)[sample] *= currentGain;
            }

            // Track max gain reduction
            float gr = 1.0f - currentGain;
            if (gr > maxGR) maxGR = gr;
        }

        gainReduction = juce::Decibels::gainToDecibels(1.0f - maxGR + 0.0001f);
    }

private:
    double sampleRate = 44100.0;
    float thresholdDb = -40.0f;
    float attackMs = 1.0f;
    float releaseMs = 100.0f;
    float rangeDb = -80.0f;

    float envelope = 0.0f;
    float currentGain = 1.0f;
    float gainReduction = 0.0f;
};
