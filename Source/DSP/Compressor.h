#pragma once
#include <juce_dsp/juce_dsp.h>

class Compressor
{
public:
    Compressor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope = 0.0f;
        gainReduction = 0.0f;
        smoothedMakeup.reset(spec.sampleRate, 0.02);
        smoothedMakeup.setCurrentAndTargetValue(1.0f);
    }

    void reset()
    {
        envelope = 0.0f;
        gainReduction = 0.0f;
        smoothedMakeup.setCurrentAndTargetValue(smoothedMakeup.getTargetValue());
    }

    void setThreshold(float dB) { thresholdDb = dB; }
    void setRatio(float r) { ratio = r; }
    void setAttack(float ms) { attackMs = ms; }
    void setRelease(float ms) { releaseMs = ms; }
    void setMakeup(float dB) { smoothedMakeup.setTargetValue(juce::Decibels::decibelsToGain(dB)); }
    void setKnee(float dB) { kneeDb = dB; }

    float getGainReduction() const { return gainReduction; }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

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

            // Convert to dB
            float inputDb = juce::Decibels::gainToDecibels(inputLevel + 0.0001f);

            // Envelope follower (in dB domain)
            if (inputDb > envelope)
                envelope = attackCoef * envelope + (1.0f - attackCoef) * inputDb;
            else
                envelope = releaseCoef * envelope + (1.0f - releaseCoef) * inputDb;

            // Calculate gain reduction with soft knee
            float gainDb = computeGain(envelope);

            // Apply gain and makeup
            float totalGain = juce::Decibels::decibelsToGain(gainDb) * smoothedMakeup.getNextValue();

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                block.getChannelPointer(ch)[sample] *= totalGain;
            }

            // Track max gain reduction
            if (-gainDb > maxGR) maxGR = -gainDb;
        }

        gainReduction = -maxGR;
    }

private:
    float computeGain(float inputDb)
    {
        // Soft knee compression
        float halfKnee = kneeDb / 2.0f;

        if (inputDb < thresholdDb - halfKnee)
        {
            // Below knee - no compression
            return 0.0f;
        }
        else if (inputDb > thresholdDb + halfKnee)
        {
            // Above knee - full compression
            return (thresholdDb + (inputDb - thresholdDb) / ratio) - inputDb;
        }
        else
        {
            // In knee - interpolate
            float x = inputDb - thresholdDb + halfKnee;
            float compressionRatio = 1.0f + (1.0f / ratio - 1.0f) * (x / kneeDb);
            return (thresholdDb - halfKnee + x * compressionRatio) - inputDb;
        }
    }

    double sampleRate = 44100.0;
    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float kneeDb = 6.0f;

    float envelope = 0.0f;
    float gainReduction = 0.0f;
    juce::SmoothedValue<float> smoothedMakeup;
};
