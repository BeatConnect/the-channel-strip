#pragma once
#include <juce_dsp/juce_dsp.h>

class Equalizer
{
public:
    Equalizer() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        lowBand.prepare(spec);
        lowMidBand.prepare(spec);
        highMidBand.prepare(spec);
        highBand.prepare(spec);

        lowBand.reset();
        lowMidBand.reset();
        highMidBand.reset();
        highBand.reset();

        updateAllCoefficients();
    }

    void reset()
    {
        lowBand.reset();
        lowMidBand.reset();
        highMidBand.reset();
        highBand.reset();
    }

    void setLowBand(float gain, float freq, bool isShelf)
    {
        lowGain = gain;
        lowFreq = freq;
        lowShelf = isShelf;
        updateLowCoefficients();
    }

    void setLowMidBand(float gain, float freq, float q)
    {
        lowMidGain = gain;
        lowMidFreq = freq;
        lowMidQ = q;
        updateLowMidCoefficients();
    }

    void setHighMidBand(float gain, float freq, float q)
    {
        highMidGain = gain;
        highMidFreq = freq;
        highMidQ = q;
        updateHighMidCoefficients();
    }

    void setHighBand(float gain, float freq, bool isShelf)
    {
        highGain = gain;
        highFreq = freq;
        highShelf = isShelf;
        updateHighCoefficients();
    }

    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        lowBand.process(context);
        lowMidBand.process(context);
        highMidBand.process(context);
        highBand.process(context);
    }

private:
    void updateAllCoefficients()
    {
        updateLowCoefficients();
        updateLowMidCoefficients();
        updateHighMidCoefficients();
        updateHighCoefficients();
    }

    void updateLowCoefficients()
    {
        if (lowShelf)
        {
            *lowBand.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                sampleRate, lowFreq, 0.707f, juce::Decibels::decibelsToGain(lowGain));
        }
        else
        {
            *lowBand.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, lowFreq, 0.707f, juce::Decibels::decibelsToGain(lowGain));
        }
    }

    void updateLowMidCoefficients()
    {
        *lowMidBand.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, lowMidFreq, lowMidQ, juce::Decibels::decibelsToGain(lowMidGain));
    }

    void updateHighMidCoefficients()
    {
        *highMidBand.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, highMidFreq, highMidQ, juce::Decibels::decibelsToGain(highMidGain));
    }

    void updateHighCoefficients()
    {
        if (highShelf)
        {
            *highBand.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                sampleRate, highFreq, 0.707f, juce::Decibels::decibelsToGain(highGain));
        }
        else
        {
            *highBand.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, highFreq, 0.707f, juce::Decibels::decibelsToGain(highGain));
        }
    }

    double sampleRate = 44100.0;

    // Low band
    float lowGain = 0.0f;
    float lowFreq = 80.0f;
    bool lowShelf = true;

    // Low-mid band
    float lowMidGain = 0.0f;
    float lowMidFreq = 400.0f;
    float lowMidQ = 1.0f;

    // High-mid band
    float highMidGain = 0.0f;
    float highMidFreq = 2500.0f;
    float highMidQ = 1.0f;

    // High band
    float highGain = 0.0f;
    float highFreq = 12000.0f;
    bool highShelf = true;

    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                   juce::dsp::IIR::Coefficients<float>>;
    Filter lowBand, lowMidBand, highMidBand, highBand;
};
