#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

class TheChannelStripEditor : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
    explicit TheChannelStripEditor(TheChannelStripProcessor&);
    ~TheChannelStripEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setupWebView();
    void setupAttachments();
    void timerCallback() override;
    void sendVisualizerData();
    void sendActivationState();

    TheChannelStripProcessor& processorRef;

    // ==============================================================================
    // Parameter Relays - created BEFORE WebBrowserComponent
    // ==============================================================================

    // Input Stage
    std::unique_ptr<juce::WebSliderRelay> inputGainRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> inputPhaseRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> inputPadRelay;

    // HPF
    std::unique_ptr<juce::WebToggleButtonRelay> hpfEnabledRelay;
    std::unique_ptr<juce::WebSliderRelay> hpfFreqRelay;
    std::unique_ptr<juce::WebComboBoxRelay> hpfSlopeRelay;

    // EQ
    std::unique_ptr<juce::WebToggleButtonRelay> eqEnabledRelay;
    std::unique_ptr<juce::WebSliderRelay> eqLowGainRelay;
    std::unique_ptr<juce::WebSliderRelay> eqLowFreqRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> eqLowShelfRelay;
    std::unique_ptr<juce::WebSliderRelay> eqLowMidGainRelay;
    std::unique_ptr<juce::WebSliderRelay> eqLowMidFreqRelay;
    std::unique_ptr<juce::WebSliderRelay> eqLowMidQRelay;
    std::unique_ptr<juce::WebSliderRelay> eqHighMidGainRelay;
    std::unique_ptr<juce::WebSliderRelay> eqHighMidFreqRelay;
    std::unique_ptr<juce::WebSliderRelay> eqHighMidQRelay;
    std::unique_ptr<juce::WebSliderRelay> eqHighGainRelay;
    std::unique_ptr<juce::WebSliderRelay> eqHighFreqRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> eqHighShelfRelay;

    // Gate
    std::unique_ptr<juce::WebToggleButtonRelay> gateEnabledRelay;
    std::unique_ptr<juce::WebSliderRelay> gateThresholdRelay;
    std::unique_ptr<juce::WebSliderRelay> gateAttackRelay;
    std::unique_ptr<juce::WebSliderRelay> gateReleaseRelay;
    std::unique_ptr<juce::WebSliderRelay> gateRangeRelay;

    // Compressor
    std::unique_ptr<juce::WebToggleButtonRelay> compEnabledRelay;
    std::unique_ptr<juce::WebSliderRelay> compThresholdRelay;
    std::unique_ptr<juce::WebSliderRelay> compRatioRelay;
    std::unique_ptr<juce::WebSliderRelay> compAttackRelay;
    std::unique_ptr<juce::WebSliderRelay> compReleaseRelay;
    std::unique_ptr<juce::WebSliderRelay> compMakeupRelay;
    std::unique_ptr<juce::WebSliderRelay> compKneeRelay;

    // Limiter
    std::unique_ptr<juce::WebToggleButtonRelay> limiterEnabledRelay;
    std::unique_ptr<juce::WebSliderRelay> limiterCeilingRelay;
    std::unique_ptr<juce::WebSliderRelay> limiterReleaseRelay;

    // Output
    std::unique_ptr<juce::WebSliderRelay> outputGainRelay;
    std::unique_ptr<juce::WebSliderRelay> outputWidthRelay;
    std::unique_ptr<juce::WebToggleButtonRelay> masterBypassRelay;

    // ==============================================================================
    // Parameter Attachments - created AFTER WebBrowserComponent
    // ==============================================================================

    // Input Stage
    std::unique_ptr<juce::WebSliderParameterAttachment> inputGainAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> inputPhaseAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> inputPadAttachment;

    // HPF
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> hpfEnabledAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> hpfFreqAttachment;
    std::unique_ptr<juce::WebComboBoxParameterAttachment> hpfSlopeAttachment;

    // EQ
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> eqEnabledAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqLowGainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqLowFreqAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> eqLowShelfAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqLowMidGainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqLowMidFreqAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqLowMidQAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqHighMidGainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqHighMidFreqAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqHighMidQAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqHighGainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> eqHighFreqAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> eqHighShelfAttachment;

    // Gate
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> gateEnabledAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> gateThresholdAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> gateAttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> gateReleaseAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> gateRangeAttachment;

    // Compressor
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> compEnabledAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compThresholdAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compRatioAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compAttackAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compReleaseAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compMakeupAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> compKneeAttachment;

    // Limiter
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> limiterEnabledAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> limiterCeilingAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> limiterReleaseAttachment;

    // Output
    std::unique_ptr<juce::WebSliderParameterAttachment> outputGainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> outputWidthAttachment;
    std::unique_ptr<juce::WebToggleButtonParameterAttachment> masterBypassAttachment;

    std::unique_ptr<juce::WebBrowserComponent> webView;
    juce::File resourcesDir;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheChannelStripEditor)
};
