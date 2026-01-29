#pragma once
#include <juce_dsp/juce_dsp.h>

class InputStage
{
public:
    InputStage() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        smoothedGain.reset(spec.sampleRate, 0.02);
        smoothedGain.setCurrentAndTargetValue(1.0f);
    }

    void reset()
    {
        smoothedGain.setCurrentAndTargetValue(smoothedGain.getTargetValue());
    }

    void setGain(float dB)
    {
        smoothedGain.setTargetValue(juce::Decibels::decibelsToGain(dB));
    }

    void setPhaseInvert(bool invert) { phaseInvert = invert; }
    void setPad(bool enabled) { padEnabled = enabled; }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        // Pad applies -20dB
        float padGain = padEnabled ? juce::Decibels::decibelsToGain(-20.0f) : 1.0f;
        float phaseMultiplier = phaseInvert ? -1.0f : 1.0f;

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            float gain = smoothedGain.getNextValue() * padGain * phaseMultiplier;

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                block.getChannelPointer(ch)[sample] *= gain;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    juce::SmoothedValue<float> smoothedGain;
    bool phaseInvert = false;
    bool padEnabled = false;
};
