#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// Forward declarations
class InputStage;
class HighPassFilter;
class Equalizer;
class Gate;
class Compressor;
class Limiter;
class OutputStage;

class TheChannelStripProcessor : public juce::AudioProcessor
{
public:
    TheChannelStripProcessor();
    ~TheChannelStripProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // ==============================================================================
    // Metering data for UI visualization
    // ==============================================================================
    float getInputLevelL() const { return inputLevelL.load(); }
    float getInputLevelR() const { return inputLevelR.load(); }
    float getOutputLevelL() const { return outputLevelL.load(); }
    float getOutputLevelR() const { return outputLevelR.load(); }
    float getGateGainReduction() const { return gateGR.load(); }
    float getCompGainReduction() const { return compGR.load(); }
    float getLimiterGainReduction() const { return limiterGR.load(); }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

    // ==============================================================================
    // DSP Processing Stages
    // ==============================================================================
    std::unique_ptr<InputStage> inputStage;
    std::unique_ptr<HighPassFilter> highPassFilter;
    std::unique_ptr<Equalizer> equalizer;
    std::unique_ptr<Gate> gate;
    std::unique_ptr<Compressor> compressor;
    std::unique_ptr<Limiter> limiter;
    std::unique_ptr<OutputStage> outputStage;

    // ==============================================================================
    // Metering
    // ==============================================================================
    std::atomic<float> inputLevelL { 0.0f };
    std::atomic<float> inputLevelR { 0.0f };
    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };
    std::atomic<float> gateGR { 0.0f };
    std::atomic<float> compGR { 0.0f };
    std::atomic<float> limiterGR { 0.0f };

    // State versioning
    static constexpr int kStateVersion = 1;

    // Sample rate for parameter smoothing
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheChannelStripProcessor)
};
