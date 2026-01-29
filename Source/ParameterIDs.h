#pragma once

/**
 * Parameter IDs - The Channel Strip
 * Must match identifiers used in:
 * 1. WebSliderRelay/WebToggleButtonRelay constructors
 * 2. TypeScript getSliderState()/getToggleState() calls
 */
namespace ParamIDs
{
    // ==============================================================================
    // Input Stage
    // ==============================================================================
    inline constexpr const char* inputGain = "inputGain";
    inline constexpr const char* inputPhase = "inputPhase";
    inline constexpr const char* inputPad = "inputPad";

    // ==============================================================================
    // High-Pass Filter
    // ==============================================================================
    inline constexpr const char* hpfEnabled = "hpfEnabled";
    inline constexpr const char* hpfFreq = "hpfFreq";
    inline constexpr const char* hpfSlope = "hpfSlope";

    // ==============================================================================
    // EQ Section - Low Band
    // ==============================================================================
    inline constexpr const char* eqEnabled = "eqEnabled";
    inline constexpr const char* eqLowGain = "eqLowGain";
    inline constexpr const char* eqLowFreq = "eqLowFreq";
    inline constexpr const char* eqLowShelf = "eqLowShelf";

    // ==============================================================================
    // EQ Section - Low-Mid Band
    // ==============================================================================
    inline constexpr const char* eqLowMidGain = "eqLowMidGain";
    inline constexpr const char* eqLowMidFreq = "eqLowMidFreq";
    inline constexpr const char* eqLowMidQ = "eqLowMidQ";

    // ==============================================================================
    // EQ Section - High-Mid Band
    // ==============================================================================
    inline constexpr const char* eqHighMidGain = "eqHighMidGain";
    inline constexpr const char* eqHighMidFreq = "eqHighMidFreq";
    inline constexpr const char* eqHighMidQ = "eqHighMidQ";

    // ==============================================================================
    // EQ Section - High Band
    // ==============================================================================
    inline constexpr const char* eqHighGain = "eqHighGain";
    inline constexpr const char* eqHighFreq = "eqHighFreq";
    inline constexpr const char* eqHighShelf = "eqHighShelf";

    // ==============================================================================
    // Gate
    // ==============================================================================
    inline constexpr const char* gateEnabled = "gateEnabled";
    inline constexpr const char* gateThreshold = "gateThreshold";
    inline constexpr const char* gateAttack = "gateAttack";
    inline constexpr const char* gateRelease = "gateRelease";
    inline constexpr const char* gateRange = "gateRange";

    // ==============================================================================
    // Compressor
    // ==============================================================================
    inline constexpr const char* compEnabled = "compEnabled";
    inline constexpr const char* compThreshold = "compThreshold";
    inline constexpr const char* compRatio = "compRatio";
    inline constexpr const char* compAttack = "compAttack";
    inline constexpr const char* compRelease = "compRelease";
    inline constexpr const char* compMakeup = "compMakeup";
    inline constexpr const char* compKnee = "compKnee";

    // ==============================================================================
    // Limiter
    // ==============================================================================
    inline constexpr const char* limiterEnabled = "limiterEnabled";
    inline constexpr const char* limiterCeiling = "limiterCeiling";
    inline constexpr const char* limiterRelease = "limiterRelease";

    // ==============================================================================
    // Output Stage
    // ==============================================================================
    inline constexpr const char* outputGain = "outputGain";
    inline constexpr const char* outputWidth = "outputWidth";
    inline constexpr const char* masterBypass = "masterBypass";
}
