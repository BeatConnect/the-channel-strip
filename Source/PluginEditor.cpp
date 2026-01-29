#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.h"

TheChannelStripEditor::TheChannelStripEditor(TheChannelStripProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // CRITICAL ORDER:
    // 1. setupWebView() - creates relays AND WebView
    // 2. setupAttachments() - connects relays to APVTS
    // 3. setSize() - AFTER WebView exists so resized() can set bounds

    setupWebView();
    setupAttachments();

    setSize(1200, 700);
    setResizable(false, false);

    startTimerHz(30);
}

TheChannelStripEditor::~TheChannelStripEditor()
{
    stopTimer();

    // Clean up attachments BEFORE webView

    // Input Stage
    inputGainAttachment.reset();
    inputPhaseAttachment.reset();
    inputPadAttachment.reset();

    // HPF
    hpfEnabledAttachment.reset();
    hpfFreqAttachment.reset();
    hpfSlopeAttachment.reset();

    // EQ
    eqEnabledAttachment.reset();
    eqLowGainAttachment.reset();
    eqLowFreqAttachment.reset();
    eqLowShelfAttachment.reset();
    eqLowMidGainAttachment.reset();
    eqLowMidFreqAttachment.reset();
    eqLowMidQAttachment.reset();
    eqHighMidGainAttachment.reset();
    eqHighMidFreqAttachment.reset();
    eqHighMidQAttachment.reset();
    eqHighGainAttachment.reset();
    eqHighFreqAttachment.reset();
    eqHighShelfAttachment.reset();

    // Gate
    gateEnabledAttachment.reset();
    gateThresholdAttachment.reset();
    gateAttackAttachment.reset();
    gateReleaseAttachment.reset();
    gateRangeAttachment.reset();

    // Compressor
    compEnabledAttachment.reset();
    compThresholdAttachment.reset();
    compRatioAttachment.reset();
    compAttackAttachment.reset();
    compReleaseAttachment.reset();
    compMakeupAttachment.reset();
    compKneeAttachment.reset();

    // Limiter
    limiterEnabledAttachment.reset();
    limiterCeilingAttachment.reset();
    limiterReleaseAttachment.reset();

    // Output
    outputGainAttachment.reset();
    outputWidthAttachment.reset();
    masterBypassAttachment.reset();

    webView.reset();
}

void TheChannelStripEditor::setupWebView()
{
    // ===========================================================================
    // STEP 1: Create relays BEFORE WebBrowserComponent
    // ===========================================================================

    // Input Stage
    inputGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::inputGain);
    inputPhaseRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::inputPhase);
    inputPadRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::inputPad);

    // HPF
    hpfEnabledRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::hpfEnabled);
    hpfFreqRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::hpfFreq);
    hpfSlopeRelay = std::make_unique<juce::WebComboBoxRelay>(ParamIDs::hpfSlope);

    // EQ
    eqEnabledRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::eqEnabled);
    eqLowGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqLowGain);
    eqLowFreqRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqLowFreq);
    eqLowShelfRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::eqLowShelf);
    eqLowMidGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqLowMidGain);
    eqLowMidFreqRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqLowMidFreq);
    eqLowMidQRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqLowMidQ);
    eqHighMidGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqHighMidGain);
    eqHighMidFreqRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqHighMidFreq);
    eqHighMidQRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqHighMidQ);
    eqHighGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqHighGain);
    eqHighFreqRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::eqHighFreq);
    eqHighShelfRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::eqHighShelf);

    // Gate
    gateEnabledRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::gateEnabled);
    gateThresholdRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::gateThreshold);
    gateAttackRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::gateAttack);
    gateReleaseRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::gateRelease);
    gateRangeRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::gateRange);

    // Compressor
    compEnabledRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::compEnabled);
    compThresholdRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compThreshold);
    compRatioRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compRatio);
    compAttackRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compAttack);
    compReleaseRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compRelease);
    compMakeupRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compMakeup);
    compKneeRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::compKnee);

    // Limiter
    limiterEnabledRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::limiterEnabled);
    limiterCeilingRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::limiterCeiling);
    limiterReleaseRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::limiterRelease);

    // Output
    outputGainRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::outputGain);
    outputWidthRelay = std::make_unique<juce::WebSliderRelay>(ParamIDs::outputWidth);
    masterBypassRelay = std::make_unique<juce::WebToggleButtonRelay>(ParamIDs::masterBypass);

    // ===========================================================================
    // STEP 2: Find resources directory
    // ===========================================================================
    auto executableFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    auto executableDir = executableFile.getParentDirectory();

    resourcesDir = executableDir.getChildFile("Resources").getChildFile("WebUI");
    if (!resourcesDir.isDirectory())
        resourcesDir = executableDir.getChildFile("WebUI");
    if (!resourcesDir.isDirectory())
        resourcesDir = executableDir.getParentDirectory().getChildFile("Resources").getChildFile("WebUI");

    DBG("Resources dir: " + resourcesDir.getFullPathName());

    // ===========================================================================
    // STEP 3: Build WebBrowserComponent options
    // ===========================================================================
    auto options = juce::WebBrowserComponent::Options()
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withNativeIntegrationEnabled()
        .withResourceProvider(
            [this](const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>
            {
                auto path = url;
                if (path.startsWith("/")) path = path.substring(1);
                if (path.isEmpty()) path = "index.html";

                auto file = resourcesDir.getChildFile(path);
                if (!file.existsAsFile()) return std::nullopt;

                juce::String mimeType = "application/octet-stream";
                if (path.endsWith(".html")) mimeType = "text/html";
                else if (path.endsWith(".css")) mimeType = "text/css";
                else if (path.endsWith(".js")) mimeType = "application/javascript";
                else if (path.endsWith(".json")) mimeType = "application/json";
                else if (path.endsWith(".png")) mimeType = "image/png";
                else if (path.endsWith(".svg")) mimeType = "image/svg+xml";
                else if (path.endsWith(".woff")) mimeType = "font/woff";
                else if (path.endsWith(".woff2")) mimeType = "font/woff2";
                else if (path.endsWith(".ttf")) mimeType = "font/ttf";

                juce::MemoryBlock data;
                file.loadFileAsData(data);

                return juce::WebBrowserComponent::Resource{
                    std::vector<std::byte>(
                        reinterpret_cast<const std::byte*>(data.getData()),
                        reinterpret_cast<const std::byte*>(data.getData()) + data.getSize()),
                    mimeType.toStdString()
                };
            })
        // Register all relays
        .withOptionsFrom(*inputGainRelay)
        .withOptionsFrom(*inputPhaseRelay)
        .withOptionsFrom(*inputPadRelay)
        .withOptionsFrom(*hpfEnabledRelay)
        .withOptionsFrom(*hpfFreqRelay)
        .withOptionsFrom(*hpfSlopeRelay)
        .withOptionsFrom(*eqEnabledRelay)
        .withOptionsFrom(*eqLowGainRelay)
        .withOptionsFrom(*eqLowFreqRelay)
        .withOptionsFrom(*eqLowShelfRelay)
        .withOptionsFrom(*eqLowMidGainRelay)
        .withOptionsFrom(*eqLowMidFreqRelay)
        .withOptionsFrom(*eqLowMidQRelay)
        .withOptionsFrom(*eqHighMidGainRelay)
        .withOptionsFrom(*eqHighMidFreqRelay)
        .withOptionsFrom(*eqHighMidQRelay)
        .withOptionsFrom(*eqHighGainRelay)
        .withOptionsFrom(*eqHighFreqRelay)
        .withOptionsFrom(*eqHighShelfRelay)
        .withOptionsFrom(*gateEnabledRelay)
        .withOptionsFrom(*gateThresholdRelay)
        .withOptionsFrom(*gateAttackRelay)
        .withOptionsFrom(*gateReleaseRelay)
        .withOptionsFrom(*gateRangeRelay)
        .withOptionsFrom(*compEnabledRelay)
        .withOptionsFrom(*compThresholdRelay)
        .withOptionsFrom(*compRatioRelay)
        .withOptionsFrom(*compAttackRelay)
        .withOptionsFrom(*compReleaseRelay)
        .withOptionsFrom(*compMakeupRelay)
        .withOptionsFrom(*compKneeRelay)
        .withOptionsFrom(*limiterEnabledRelay)
        .withOptionsFrom(*limiterCeilingRelay)
        .withOptionsFrom(*limiterReleaseRelay)
        .withOptionsFrom(*outputGainRelay)
        .withOptionsFrom(*outputWidthRelay)
        .withOptionsFrom(*masterBypassRelay)
        // Activation status event
        .withEventListener("getActivationStatus", [this](const juce::var&) {
            juce::DynamicObject::Ptr data = new juce::DynamicObject();
#if BEATCONNECT_ACTIVATION_ENABLED
            data->setProperty("isConfigured", true);
            data->setProperty("isActivated", false);
#else
            data->setProperty("isConfigured", false);
            data->setProperty("isActivated", false);
#endif
            webView->emitEventIfBrowserIsVisible("activationState", juce::var(data.get()));
        })
        // Windows-specific options
        .withWinWebView2Options(
            juce::WebBrowserComponent::Options::WinWebView2()
                .withBackgroundColour(juce::Colour(0xFF0d0d0d))
                .withStatusBarDisabled()
                .withUserDataFolder(
                    juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("TheChannelStrip_WebView2")));

    // ===========================================================================
    // STEP 4: Create WebBrowserComponent and load URL
    // ===========================================================================
    webView = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(*webView);

#if THE_CHANNEL_STRIP_DEV_MODE
    webView->goToURL("http://localhost:5173");
    DBG("Loading dev server at localhost:5173");
#else
    webView->goToURL(webView->getResourceProviderRoot());
    DBG("Loading from resource provider");
#endif
}

void TheChannelStripEditor::setupAttachments()
{
    auto& apvts = processorRef.getAPVTS();

    // Input Stage
    inputGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::inputGain), *inputGainRelay, nullptr);
    inputPhaseAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::inputPhase), *inputPhaseRelay, nullptr);
    inputPadAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::inputPad), *inputPadRelay, nullptr);

    // HPF
    hpfEnabledAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::hpfEnabled), *hpfEnabledRelay, nullptr);
    hpfFreqAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::hpfFreq), *hpfFreqRelay, nullptr);
    hpfSlopeAttachment = std::make_unique<juce::WebComboBoxParameterAttachment>(
        *apvts.getParameter(ParamIDs::hpfSlope), *hpfSlopeRelay, nullptr);

    // EQ
    eqEnabledAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqEnabled), *eqEnabledRelay, nullptr);
    eqLowGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowGain), *eqLowGainRelay, nullptr);
    eqLowFreqAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowFreq), *eqLowFreqRelay, nullptr);
    eqLowShelfAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowShelf), *eqLowShelfRelay, nullptr);
    eqLowMidGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowMidGain), *eqLowMidGainRelay, nullptr);
    eqLowMidFreqAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowMidFreq), *eqLowMidFreqRelay, nullptr);
    eqLowMidQAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqLowMidQ), *eqLowMidQRelay, nullptr);
    eqHighMidGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighMidGain), *eqHighMidGainRelay, nullptr);
    eqHighMidFreqAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighMidFreq), *eqHighMidFreqRelay, nullptr);
    eqHighMidQAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighMidQ), *eqHighMidQRelay, nullptr);
    eqHighGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighGain), *eqHighGainRelay, nullptr);
    eqHighFreqAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighFreq), *eqHighFreqRelay, nullptr);
    eqHighShelfAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::eqHighShelf), *eqHighShelfRelay, nullptr);

    // Gate
    gateEnabledAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::gateEnabled), *gateEnabledRelay, nullptr);
    gateThresholdAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::gateThreshold), *gateThresholdRelay, nullptr);
    gateAttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::gateAttack), *gateAttackRelay, nullptr);
    gateReleaseAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::gateRelease), *gateReleaseRelay, nullptr);
    gateRangeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::gateRange), *gateRangeRelay, nullptr);

    // Compressor
    compEnabledAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::compEnabled), *compEnabledRelay, nullptr);
    compThresholdAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compThreshold), *compThresholdRelay, nullptr);
    compRatioAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compRatio), *compRatioRelay, nullptr);
    compAttackAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compAttack), *compAttackRelay, nullptr);
    compReleaseAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compRelease), *compReleaseRelay, nullptr);
    compMakeupAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compMakeup), *compMakeupRelay, nullptr);
    compKneeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::compKnee), *compKneeRelay, nullptr);

    // Limiter
    limiterEnabledAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::limiterEnabled), *limiterEnabledRelay, nullptr);
    limiterCeilingAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::limiterCeiling), *limiterCeilingRelay, nullptr);
    limiterReleaseAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::limiterRelease), *limiterReleaseRelay, nullptr);

    // Output
    outputGainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::outputGain), *outputGainRelay, nullptr);
    outputWidthAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *apvts.getParameter(ParamIDs::outputWidth), *outputWidthRelay, nullptr);
    masterBypassAttachment = std::make_unique<juce::WebToggleButtonParameterAttachment>(
        *apvts.getParameter(ParamIDs::masterBypass), *masterBypassRelay, nullptr);
}

void TheChannelStripEditor::timerCallback()
{
    sendVisualizerData();
}

void TheChannelStripEditor::sendVisualizerData()
{
    if (!webView) return;

    juce::DynamicObject::Ptr data = new juce::DynamicObject();
    data->setProperty("inputLevelL", processorRef.getInputLevelL());
    data->setProperty("inputLevelR", processorRef.getInputLevelR());
    data->setProperty("outputLevelL", processorRef.getOutputLevelL());
    data->setProperty("outputLevelR", processorRef.getOutputLevelR());
    data->setProperty("gateGR", processorRef.getGateGainReduction());
    data->setProperty("compGR", processorRef.getCompGainReduction());
    data->setProperty("limiterGR", processorRef.getLimiterGainReduction());

    webView->emitEventIfBrowserIsVisible("visualizerData", juce::var(data.get()));
}

void TheChannelStripEditor::sendActivationState()
{
    if (!webView) return;

    juce::DynamicObject::Ptr data = new juce::DynamicObject();
#if BEATCONNECT_ACTIVATION_ENABLED
    data->setProperty("isConfigured", true);
    data->setProperty("isActivated", false);
#else
    data->setProperty("isConfigured", false);
    data->setProperty("isActivated", false);
#endif
    webView->emitEventIfBrowserIsVisible("activationState", juce::var(data.get()));
}

void TheChannelStripEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF0d0d0d));
}

void TheChannelStripEditor::resized()
{
    if (webView)
        webView->setBounds(getLocalBounds());
}
