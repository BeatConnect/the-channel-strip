#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"
#include "DSP/InputStage.h"
#include "DSP/HighPassFilter.h"
#include "DSP/Equalizer.h"
#include "DSP/Gate.h"
#include "DSP/Compressor.h"
#include "DSP/Limiter.h"
#include "DSP/OutputStage.h"

TheChannelStripProcessor::TheChannelStripProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Create DSP modules
    inputStage = std::make_unique<InputStage>();
    highPassFilter = std::make_unique<HighPassFilter>();
    equalizer = std::make_unique<Equalizer>();
    gate = std::make_unique<Gate>();
    compressor = std::make_unique<Compressor>();
    limiter = std::make_unique<Limiter>();
    outputStage = std::make_unique<OutputStage>();
}

TheChannelStripProcessor::~TheChannelStripProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout TheChannelStripProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ==============================================================================
    // Input Stage
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::inputGain, 1 },
        "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::inputPhase, 1 },
        "Phase Invert",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::inputPad, 1 },
        "Pad (-20dB)",
        false));

    // ==============================================================================
    // High-Pass Filter
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::hpfEnabled, 1 },
        "HPF Enable",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::hpfFreq, 1 },
        "HPF Frequency",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.4f),
        80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { ParamIDs::hpfSlope, 1 },
        "HPF Slope",
        juce::StringArray { "12 dB/oct", "18 dB/oct", "24 dB/oct" },
        1));

    // ==============================================================================
    // EQ Section
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::eqEnabled, 1 },
        "EQ Enable",
        true));

    // Low Band
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqLowGain, 1 },
        "Low Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqLowFreq, 1 },
        "Low Freq",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.4f),
        80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::eqLowShelf, 1 },
        "Low Shelf",
        true));

    // Low-Mid Band
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqLowMidGain, 1 },
        "Low-Mid Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqLowMidFreq, 1 },
        "Low-Mid Freq",
        juce::NormalisableRange<float>(100.0f, 2000.0f, 1.0f, 0.4f),
        400.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqLowMidQ, 1 },
        "Low-Mid Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f),
        1.0f));

    // High-Mid Band
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqHighMidGain, 1 },
        "High-Mid Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqHighMidFreq, 1 },
        "High-Mid Freq",
        juce::NormalisableRange<float>(500.0f, 8000.0f, 1.0f, 0.4f),
        2500.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqHighMidQ, 1 },
        "High-Mid Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f),
        1.0f));

    // High Band
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqHighGain, 1 },
        "High Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::eqHighFreq, 1 },
        "High Freq",
        juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f, 0.4f),
        12000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::eqHighShelf, 1 },
        "High Shelf",
        true));

    // ==============================================================================
    // Gate
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::gateEnabled, 1 },
        "Gate Enable",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::gateThreshold, 1 },
        "Gate Threshold",
        juce::NormalisableRange<float>(-80.0f, 0.0f, 0.1f),
        -40.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::gateAttack, 1 },
        "Gate Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f),
        1.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::gateRelease, 1 },
        "Gate Release",
        juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.4f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::gateRange, 1 },
        "Gate Range",
        juce::NormalisableRange<float>(-80.0f, 0.0f, 0.1f),
        -80.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // ==============================================================================
    // Compressor
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::compEnabled, 1 },
        "Compressor Enable",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compThreshold, 1 },
        "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compRatio, 1 },
        "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f),
        4.0f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compAttack, 1 },
        "Comp Attack",
        juce::NormalisableRange<float>(0.1f, 300.0f, 0.1f, 0.4f),
        10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compRelease, 1 },
        "Comp Release",
        juce::NormalisableRange<float>(10.0f, 3000.0f, 1.0f, 0.4f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compMakeup, 1 },
        "Comp Makeup",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::compKnee, 1 },
        "Comp Knee",
        juce::NormalisableRange<float>(0.0f, 12.0f, 0.1f),
        6.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // ==============================================================================
    // Limiter
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::limiterEnabled, 1 },
        "Limiter Enable",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::limiterCeiling, 1 },
        "Limiter Ceiling",
        juce::NormalisableRange<float>(-12.0f, 0.0f, 0.1f),
        -0.3f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::limiterRelease, 1 },
        "Limiter Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // ==============================================================================
    // Output Stage
    // ==============================================================================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::outputGain, 1 },
        "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { ParamIDs::outputWidth, 1 },
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { ParamIDs::masterBypass, 1 },
        "Master Bypass",
        false));

    return { params.begin(), params.end() };
}

void TheChannelStripProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock * 2); // Headroom for variable buffer sizes
    spec.numChannels = 2;

    inputStage->prepare(spec);
    highPassFilter->prepare(spec);
    equalizer->prepare(spec);
    gate->prepare(spec);
    compressor->prepare(spec);
    limiter->prepare(spec);
    outputStage->prepare(spec);
}

void TheChannelStripProcessor::releaseResources()
{
    inputStage->reset();
    highPassFilter->reset();
    equalizer->reset();
    gate->reset();
    compressor->reset();
    limiter->reset();
    outputStage->reset();
}

void TheChannelStripProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Measure input levels
    if (totalNumInputChannels > 0)
        inputLevelL.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    if (totalNumInputChannels > 1)
        inputLevelR.store(buffer.getMagnitude(1, 0, buffer.getNumSamples()));

    // Check master bypass
    bool bypassed = *apvts.getRawParameterValue(ParamIDs::masterBypass) > 0.5f;

    if (bypassed)
    {
        // Reset all stages to prevent clicks on re-enable
        inputStage->reset();
        highPassFilter->reset();
        equalizer->reset();
        gate->reset();
        compressor->reset();
        limiter->reset();
        outputStage->reset();

        // Copy input to output for metering
        outputLevelL.store(inputLevelL.load());
        outputLevelR.store(inputLevelR.load());
        gateGR.store(0.0f);
        compGR.store(0.0f);
        limiterGR.store(0.0f);
        return;
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // ==============================================================================
    // Signal Flow: Input -> HPF -> EQ -> Gate -> Comp -> Limiter -> Output
    // ==============================================================================

    // Input Stage
    inputStage->setGain(*apvts.getRawParameterValue(ParamIDs::inputGain));
    inputStage->setPhaseInvert(*apvts.getRawParameterValue(ParamIDs::inputPhase) > 0.5f);
    inputStage->setPad(*apvts.getRawParameterValue(ParamIDs::inputPad) > 0.5f);
    inputStage->process(context);

    // High-Pass Filter
    bool hpfEnabled = *apvts.getRawParameterValue(ParamIDs::hpfEnabled) > 0.5f;
    if (hpfEnabled)
    {
        highPassFilter->setFrequency(*apvts.getRawParameterValue(ParamIDs::hpfFreq));
        int slopeIndex = static_cast<int>(*apvts.getRawParameterValue(ParamIDs::hpfSlope));
        highPassFilter->setSlope(slopeIndex);
        highPassFilter->process(context);
    }

    // EQ
    bool eqEnabled = *apvts.getRawParameterValue(ParamIDs::eqEnabled) > 0.5f;
    if (eqEnabled)
    {
        equalizer->setLowBand(
            *apvts.getRawParameterValue(ParamIDs::eqLowGain),
            *apvts.getRawParameterValue(ParamIDs::eqLowFreq),
            *apvts.getRawParameterValue(ParamIDs::eqLowShelf) > 0.5f
        );
        equalizer->setLowMidBand(
            *apvts.getRawParameterValue(ParamIDs::eqLowMidGain),
            *apvts.getRawParameterValue(ParamIDs::eqLowMidFreq),
            *apvts.getRawParameterValue(ParamIDs::eqLowMidQ)
        );
        equalizer->setHighMidBand(
            *apvts.getRawParameterValue(ParamIDs::eqHighMidGain),
            *apvts.getRawParameterValue(ParamIDs::eqHighMidFreq),
            *apvts.getRawParameterValue(ParamIDs::eqHighMidQ)
        );
        equalizer->setHighBand(
            *apvts.getRawParameterValue(ParamIDs::eqHighGain),
            *apvts.getRawParameterValue(ParamIDs::eqHighFreq),
            *apvts.getRawParameterValue(ParamIDs::eqHighShelf) > 0.5f
        );
        equalizer->process(context);
    }

    // Gate
    bool gateEnabled = *apvts.getRawParameterValue(ParamIDs::gateEnabled) > 0.5f;
    if (gateEnabled)
    {
        gate->setThreshold(*apvts.getRawParameterValue(ParamIDs::gateThreshold));
        gate->setAttack(*apvts.getRawParameterValue(ParamIDs::gateAttack));
        gate->setRelease(*apvts.getRawParameterValue(ParamIDs::gateRelease));
        gate->setRange(*apvts.getRawParameterValue(ParamIDs::gateRange));
        gate->process(context);
        gateGR.store(gate->getGainReduction());
    }
    else
    {
        gateGR.store(0.0f);
    }

    // Compressor
    bool compEnabled = *apvts.getRawParameterValue(ParamIDs::compEnabled) > 0.5f;
    if (compEnabled)
    {
        compressor->setThreshold(*apvts.getRawParameterValue(ParamIDs::compThreshold));
        compressor->setRatio(*apvts.getRawParameterValue(ParamIDs::compRatio));
        compressor->setAttack(*apvts.getRawParameterValue(ParamIDs::compAttack));
        compressor->setRelease(*apvts.getRawParameterValue(ParamIDs::compRelease));
        compressor->setMakeup(*apvts.getRawParameterValue(ParamIDs::compMakeup));
        compressor->setKnee(*apvts.getRawParameterValue(ParamIDs::compKnee));
        compressor->process(context);
        compGR.store(compressor->getGainReduction());
    }
    else
    {
        compGR.store(0.0f);
    }

    // Limiter
    bool limiterEnabled = *apvts.getRawParameterValue(ParamIDs::limiterEnabled) > 0.5f;
    if (limiterEnabled)
    {
        limiter->setCeiling(*apvts.getRawParameterValue(ParamIDs::limiterCeiling));
        limiter->setRelease(*apvts.getRawParameterValue(ParamIDs::limiterRelease));
        limiter->process(context);
        limiterGR.store(limiter->getGainReduction());
    }
    else
    {
        limiterGR.store(0.0f);
    }

    // Output Stage
    outputStage->setGain(*apvts.getRawParameterValue(ParamIDs::outputGain));
    outputStage->setWidth(*apvts.getRawParameterValue(ParamIDs::outputWidth));
    outputStage->process(context);

    // Measure output levels
    if (totalNumOutputChannels > 0)
        outputLevelL.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    if (totalNumOutputChannels > 1)
        outputLevelR.store(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
}

juce::AudioProcessorEditor* TheChannelStripProcessor::createEditor()
{
    return new TheChannelStripEditor(*this);
}

void TheChannelStripProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->setAttribute("stateVersion", kStateVersion);
    copyXmlToBinary(*xml, destData);
}

void TheChannelStripProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
    {
        int loadedVersion = xmlState->getIntAttribute("stateVersion", 0);
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        if (loadedVersion != kStateVersion)
        {
            DBG("State version mismatch: loaded " << loadedVersion << ", expected " << kStateVersion);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheChannelStripProcessor();
}
