#pragma once
#include <juce_dsp/juce_dsp.h>

class OutputStage
{
public:
    OutputStage() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        smoothedGain.reset(spec.sampleRate, 0.02);
        smoothedGain.setCurrentAndTargetValue(1.0f);
        smoothedWidth.reset(spec.sampleRate, 0.02);
        smoothedWidth.setCurrentAndTargetValue(1.0f);
    }

    void reset()
    {
        smoothedGain.setCurrentAndTargetValue(smoothedGain.getTargetValue());
        smoothedWidth.setCurrentAndTargetValue(smoothedWidth.getTargetValue());
    }

    void setGain(float dB)
    {
        smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(dB));
    }

    void setWidth(float percent)
    {
        // 0% = mono, 100% = normal, 200% = extra wide
        smoothedWidth.setTargetValue(percent / 100.0f);
    }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        if (numChannels < 2)
        {
            // Mono - just apply gain
            for (size_t sample = 0; sample < numSamples; ++sample)
            {
                float gain = smoothedGain.getNextValue();
                smoothedWidth.getNextValue(); // Keep width smoother in sync

                for (size_t ch = 0; ch < numChannels; ++ch)
                {
                    block.getChannelPointer(ch)[sample] *= gain;
                }
            }
            return;
        }

        // Stereo processing with width control
        float* leftChannel = block.getChannelPointer(0);
        float* rightChannel = block.getChannelPointer(1);

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            float gain = smoothedGain.getNextValue();
            float width = smoothedWidth.getNextValue();

            float left = leftChannel[sample];
            float right = rightChannel[sample];

            // Mid-Side processing for width
            float mid = (left + right) * 0.5f;
            float side = (left - right) * 0.5f;

            // Apply width to side signal
            side *= width;

            // Convert back to L/R
            leftChannel[sample] = (mid + side) * gain;
            rightChannel[sample] = (mid - side) * gain;
        }
    }

private:
    double sampleRate = 44100.0;
    juce::SmoothedValue<float> smoothedGain;
    juce::SmoothedValue<float> smoothedWidth;
};
