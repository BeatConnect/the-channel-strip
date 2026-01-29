<script lang="ts">
  import { onMount } from 'svelte';
  import {
    createSliderStore,
    createToggleStore,
    createComboStore,
    visualizerData
  } from './stores/params';

  import Section from './components/Section.svelte';
  import Knob from './components/Knob.svelte';
  import Meter from './components/Meter.svelte';
  import GainReductionMeter from './components/GainReductionMeter.svelte';
  import ToggleButton from './components/ToggleButton.svelte';
  import BloomOverlay from './components/BloomOverlay.svelte';

  // ==============================================================================
  // Input Stage
  // ==============================================================================
  const inputGain = createSliderStore('inputGain', 0.5);
  const inputPhase = createToggleStore('inputPhase', false);
  const inputPad = createToggleStore('inputPad', false);

  // ==============================================================================
  // High-Pass Filter
  // ==============================================================================
  const hpfEnabled = createToggleStore('hpfEnabled', false);
  const hpfFreq = createSliderStore('hpfFreq', 0.125); // 80Hz default
  const hpfSlope = createComboStore('hpfSlope', 1);

  // ==============================================================================
  // EQ Section
  // ==============================================================================
  const eqEnabled = createToggleStore('eqEnabled', true);
  const eqLowGain = createSliderStore('eqLowGain', 0.5);
  const eqLowFreq = createSliderStore('eqLowFreq', 0.125);
  const eqLowShelf = createToggleStore('eqLowShelf', true);
  const eqLowMidGain = createSliderStore('eqLowMidGain', 0.5);
  const eqLowMidFreq = createSliderStore('eqLowMidFreq', 0.16);
  const eqLowMidQ = createSliderStore('eqLowMidQ', 0.1);
  const eqHighMidGain = createSliderStore('eqHighMidGain', 0.5);
  const eqHighMidFreq = createSliderStore('eqHighMidFreq', 0.27);
  const eqHighMidQ = createSliderStore('eqHighMidQ', 0.1);
  const eqHighGain = createSliderStore('eqHighGain', 0.5);
  const eqHighFreq = createSliderStore('eqHighFreq', 0.56);
  const eqHighShelf = createToggleStore('eqHighShelf', true);

  // ==============================================================================
  // Gate
  // ==============================================================================
  const gateEnabled = createToggleStore('gateEnabled', false);
  const gateThreshold = createSliderStore('gateThreshold', 0.5);
  const gateAttack = createSliderStore('gateAttack', 0.01);
  const gateRelease = createSliderStore('gateRelease', 0.045);
  const gateRange = createSliderStore('gateRange', 0);

  // ==============================================================================
  // Compressor
  // ==============================================================================
  const compEnabled = createToggleStore('compEnabled', false);
  const compThreshold = createSliderStore('compThreshold', 0.67);
  const compRatio = createSliderStore('compRatio', 0.16);
  const compAttack = createSliderStore('compAttack', 0.033);
  const compRelease = createSliderStore('compRelease', 0.03);
  const compMakeup = createSliderStore('compMakeup', 0.33);
  const compKnee = createSliderStore('compKnee', 0.5);

  // ==============================================================================
  // Limiter
  // ==============================================================================
  const limiterEnabled = createToggleStore('limiterEnabled', false);
  const limiterCeiling = createSliderStore('limiterCeiling', 0.975);
  const limiterRelease = createSliderStore('limiterRelease', 0.09);

  // ==============================================================================
  // Output
  // ==============================================================================
  const outputGain = createSliderStore('outputGain', 0.5);
  const outputWidth = createSliderStore('outputWidth', 0.5);
  const masterBypass = createToggleStore('masterBypass', false);

  // HPF Slope choices
  const hpfSlopeChoices = ['12 dB/oct', '18 dB/oct', '24 dB/oct'];

  // Format values for display
  function formatDb(normalized: number, min: number, max: number): string {
    const value = min + normalized * (max - min);
    return value >= 0 ? `+${value.toFixed(1)}` : value.toFixed(1);
  }

  function formatHz(normalized: number, min: number, max: number, skew: number = 0.4): string {
    // Apply inverse skew for display
    const skewed = Math.pow(normalized, 1 / skew);
    const value = min + skewed * (max - min);
    return value >= 1000 ? `${(value / 1000).toFixed(1)}k` : Math.round(value).toString();
  }

  function formatMs(normalized: number, min: number, max: number, skew: number = 0.4): string {
    const skewed = Math.pow(normalized, 1 / skew);
    const value = min + skewed * (max - min);
    return value >= 1000 ? `${(value / 1000).toFixed(2)}s` : `${Math.round(value)}`;
  }

  function formatRatio(normalized: number): string {
    const ratio = 1 + normalized * 19;
    return ratio >= 20 ? 'inf' : ratio.toFixed(1);
  }

  function formatQ(normalized: number): string {
    const q = 0.1 + normalized * 9.9;
    return q.toFixed(2);
  }

  function formatPercent(normalized: number): string {
    return `${Math.round(normalized * 200)}`;
  }
</script>

<div class="channel-strip" class:bypassed={$masterBypass}>
  <!-- Header -->
  <header class="header">
    <div class="logo">
      <span class="logo-text">NWOM</span>
      <span class="logo-divider">|</span>
      <span class="product-name">THE CHANNEL STRIP</span>
    </div>
    <div class="header-controls">
      <ToggleButton
        active={$masterBypass}
        label="BYPASS"
        accent="red"
        on:change={() => masterBypass.toggle()}
      />
    </div>
  </header>

  <!-- Main Content -->
  <main class="main-content">
    <!-- Input Meter -->
    <div class="meter-section input-meters">
      <Meter
        levelL={$visualizerData.inputLevelL}
        levelR={$visualizerData.inputLevelR}
        label="IN"
        size="md"
      />
    </div>

    <!-- Input Stage -->
    <Section title="Input" accent="cyan">
      <div class="control-row">
        <Knob
          bind:value={$inputGain}
          min={-24}
          max={24}
          label="Gain"
          unit="dB"
          decimals={1}
          bipolar={true}
          accent="cyan"
          on:dragstart={() => inputGain.dragStart()}
          on:dragend={() => inputGain.dragEnd()}
          on:change={(e) => inputGain.set(($inputGain + 24) / 48)}
        />
      </div>
      <div class="button-row">
        <ToggleButton
          active={$inputPhase}
          label="PHASE"
          accent="cyan"
          size="sm"
          on:change={() => inputPhase.toggle()}
        />
        <ToggleButton
          active={$inputPad}
          label="PAD"
          accent="cyan"
          size="sm"
          on:change={() => inputPad.toggle()}
        />
      </div>
    </Section>

    <!-- HPF -->
    <Section
      title="HPF"
      accent="magenta"
      enabled={$hpfEnabled}
      showEnable={true}
      on:toggle={() => hpfEnabled.toggle()}
    >
      <div class="control-row">
        <Knob
          value={$hpfFreq}
          min={20}
          max={500}
          label="Freq"
          unit="Hz"
          decimals={0}
          accent="magenta"
          on:dragstart={() => hpfFreq.dragStart()}
          on:dragend={() => hpfFreq.dragEnd()}
          on:change={(e) => hpfFreq.set(e.detail)}
        />
      </div>
      <div class="slope-select">
        {#each hpfSlopeChoices as choice, i}
          <button
            class="slope-btn"
            class:active={$hpfSlope === i}
            on:click={() => hpfSlope.set(i)}
          >
            {choice}
          </button>
        {/each}
      </div>
    </Section>

    <!-- EQ Section -->
    <Section
      title="Equalizer"
      accent="green"
      enabled={$eqEnabled}
      showEnable={true}
      on:toggle={() => eqEnabled.toggle()}
    >
      <div class="eq-bands">
        <!-- Low Band -->
        <div class="eq-band">
          <div class="band-label low">LOW</div>
          <Knob
            value={$eqLowGain}
            min={-18}
            max={18}
            label="Gain"
            unit="dB"
            decimals={1}
            bipolar={true}
            size="sm"
            accent="green"
            on:dragstart={() => eqLowGain.dragStart()}
            on:dragend={() => eqLowGain.dragEnd()}
            on:change={(e) => eqLowGain.set(e.detail)}
          />
          <Knob
            value={$eqLowFreq}
            min={20}
            max={500}
            label="Freq"
            unit="Hz"
            decimals={0}
            size="sm"
            accent="green"
            on:dragstart={() => eqLowFreq.dragStart()}
            on:dragend={() => eqLowFreq.dragEnd()}
            on:change={(e) => eqLowFreq.set(e.detail)}
          />
          <ToggleButton
            active={$eqLowShelf}
            label="SHELF"
            accent="green"
            size="sm"
            on:change={() => eqLowShelf.toggle()}
          />
        </div>

        <!-- Low-Mid Band -->
        <div class="eq-band">
          <div class="band-label lmid">L-MID</div>
          <Knob
            value={$eqLowMidGain}
            min={-18}
            max={18}
            label="Gain"
            unit="dB"
            decimals={1}
            bipolar={true}
            size="sm"
            accent="green"
            on:dragstart={() => eqLowMidGain.dragStart()}
            on:dragend={() => eqLowMidGain.dragEnd()}
            on:change={(e) => eqLowMidGain.set(e.detail)}
          />
          <Knob
            value={$eqLowMidFreq}
            min={100}
            max={2000}
            label="Freq"
            unit="Hz"
            decimals={0}
            size="sm"
            accent="green"
            on:dragstart={() => eqLowMidFreq.dragStart()}
            on:dragend={() => eqLowMidFreq.dragEnd()}
            on:change={(e) => eqLowMidFreq.set(e.detail)}
          />
          <Knob
            value={$eqLowMidQ}
            min={0.1}
            max={10}
            label="Q"
            unit=""
            decimals={2}
            size="sm"
            accent="green"
            on:dragstart={() => eqLowMidQ.dragStart()}
            on:dragend={() => eqLowMidQ.dragEnd()}
            on:change={(e) => eqLowMidQ.set(e.detail)}
          />
        </div>

        <!-- High-Mid Band -->
        <div class="eq-band">
          <div class="band-label hmid">H-MID</div>
          <Knob
            value={$eqHighMidGain}
            min={-18}
            max={18}
            label="Gain"
            unit="dB"
            decimals={1}
            bipolar={true}
            size="sm"
            accent="green"
            on:dragstart={() => eqHighMidGain.dragStart()}
            on:dragend={() => eqHighMidGain.dragEnd()}
            on:change={(e) => eqHighMidGain.set(e.detail)}
          />
          <Knob
            value={$eqHighMidFreq}
            min={500}
            max={8000}
            label="Freq"
            unit="Hz"
            decimals={0}
            size="sm"
            accent="green"
            on:dragstart={() => eqHighMidFreq.dragStart()}
            on:dragend={() => eqHighMidFreq.dragEnd()}
            on:change={(e) => eqHighMidFreq.set(e.detail)}
          />
          <Knob
            value={$eqHighMidQ}
            min={0.1}
            max={10}
            label="Q"
            unit=""
            decimals={2}
            size="sm"
            accent="green"
            on:dragstart={() => eqHighMidQ.dragStart()}
            on:dragend={() => eqHighMidQ.dragEnd()}
            on:change={(e) => eqHighMidQ.set(e.detail)}
          />
        </div>

        <!-- High Band -->
        <div class="eq-band">
          <div class="band-label high">HIGH</div>
          <Knob
            value={$eqHighGain}
            min={-18}
            max={18}
            label="Gain"
            unit="dB"
            decimals={1}
            bipolar={true}
            size="sm"
            accent="green"
            on:dragstart={() => eqHighGain.dragStart()}
            on:dragend={() => eqHighGain.dragEnd()}
            on:change={(e) => eqHighGain.set(e.detail)}
          />
          <Knob
            value={$eqHighFreq}
            min={2000}
            max={20000}
            label="Freq"
            unit="Hz"
            decimals={0}
            size="sm"
            accent="green"
            on:dragstart={() => eqHighFreq.dragStart()}
            on:dragend={() => eqHighFreq.dragEnd()}
            on:change={(e) => eqHighFreq.set(e.detail)}
          />
          <ToggleButton
            active={$eqHighShelf}
            label="SHELF"
            accent="green"
            size="sm"
            on:change={() => eqHighShelf.toggle()}
          />
        </div>
      </div>
    </Section>

    <!-- Gate -->
    <Section
      title="Gate"
      accent="orange"
      enabled={$gateEnabled}
      showEnable={true}
      on:toggle={() => gateEnabled.toggle()}
    >
      <div class="dynamics-content">
        <div class="control-grid">
          <Knob
            value={$gateThreshold}
            min={-80}
            max={0}
            label="Thresh"
            unit="dB"
            decimals={1}
            accent="orange"
            on:dragstart={() => gateThreshold.dragStart()}
            on:dragend={() => gateThreshold.dragEnd()}
            on:change={(e) => gateThreshold.set(e.detail)}
          />
          <Knob
            value={$gateAttack}
            min={0.1}
            max={100}
            label="Attack"
            unit="ms"
            decimals={1}
            accent="orange"
            on:dragstart={() => gateAttack.dragStart()}
            on:dragend={() => gateAttack.dragEnd()}
            on:change={(e) => gateAttack.set(e.detail)}
          />
          <Knob
            value={$gateRelease}
            min={10}
            max={2000}
            label="Release"
            unit="ms"
            decimals={0}
            accent="orange"
            on:dragstart={() => gateRelease.dragStart()}
            on:dragend={() => gateRelease.dragEnd()}
            on:change={(e) => gateRelease.set(e.detail)}
          />
          <Knob
            value={$gateRange}
            min={-80}
            max={0}
            label="Range"
            unit="dB"
            decimals={0}
            accent="orange"
            on:dragstart={() => gateRange.dragStart()}
            on:dragend={() => gateRange.dragEnd()}
            on:change={(e) => gateRange.set(e.detail)}
          />
        </div>
        <GainReductionMeter
          value={$visualizerData.gateGR}
          label="GR"
          accent="orange"
        />
      </div>
    </Section>

    <!-- Compressor -->
    <Section
      title="Compressor"
      accent="yellow"
      enabled={$compEnabled}
      showEnable={true}
      on:toggle={() => compEnabled.toggle()}
    >
      <div class="dynamics-content">
        <div class="control-grid comp-grid">
          <Knob
            value={$compThreshold}
            min={-60}
            max={0}
            label="Thresh"
            unit="dB"
            decimals={1}
            accent="yellow"
            on:dragstart={() => compThreshold.dragStart()}
            on:dragend={() => compThreshold.dragEnd()}
            on:change={(e) => compThreshold.set(e.detail)}
          />
          <Knob
            value={$compRatio}
            min={1}
            max={20}
            label="Ratio"
            unit=":1"
            decimals={1}
            accent="yellow"
            on:dragstart={() => compRatio.dragStart()}
            on:dragend={() => compRatio.dragEnd()}
            on:change={(e) => compRatio.set(e.detail)}
          />
          <Knob
            value={$compAttack}
            min={0.1}
            max={300}
            label="Attack"
            unit="ms"
            decimals={1}
            accent="yellow"
            on:dragstart={() => compAttack.dragStart()}
            on:dragend={() => compAttack.dragEnd()}
            on:change={(e) => compAttack.set(e.detail)}
          />
          <Knob
            value={$compRelease}
            min={10}
            max={3000}
            label="Release"
            unit="ms"
            decimals={0}
            accent="yellow"
            on:dragstart={() => compRelease.dragStart()}
            on:dragend={() => compRelease.dragEnd()}
            on:change={(e) => compRelease.set(e.detail)}
          />
          <Knob
            value={$compMakeup}
            min={-12}
            max={24}
            label="Makeup"
            unit="dB"
            decimals={1}
            bipolar={true}
            accent="yellow"
            on:dragstart={() => compMakeup.dragStart()}
            on:dragend={() => compMakeup.dragEnd()}
            on:change={(e) => compMakeup.set(e.detail)}
          />
          <Knob
            value={$compKnee}
            min={0}
            max={12}
            label="Knee"
            unit="dB"
            decimals={1}
            accent="yellow"
            on:dragstart={() => compKnee.dragStart()}
            on:dragend={() => compKnee.dragEnd()}
            on:change={(e) => compKnee.set(e.detail)}
          />
        </div>
        <GainReductionMeter
          value={$visualizerData.compGR}
          label="GR"
          accent="yellow"
        />
      </div>
    </Section>

    <!-- Limiter -->
    <Section
      title="Limiter"
      accent="red"
      enabled={$limiterEnabled}
      showEnable={true}
      on:toggle={() => limiterEnabled.toggle()}
    >
      <div class="dynamics-content">
        <div class="control-grid limiter-grid">
          <Knob
            value={$limiterCeiling}
            min={-12}
            max={0}
            label="Ceiling"
            unit="dB"
            decimals={1}
            accent="red"
            on:dragstart={() => limiterCeiling.dragStart()}
            on:dragend={() => limiterCeiling.dragEnd()}
            on:change={(e) => limiterCeiling.set(e.detail)}
          />
          <Knob
            value={$limiterRelease}
            min={10}
            max={1000}
            label="Release"
            unit="ms"
            decimals={0}
            accent="red"
            on:dragstart={() => limiterRelease.dragStart()}
            on:dragend={() => limiterRelease.dragEnd()}
            on:change={(e) => limiterRelease.set(e.detail)}
          />
        </div>
        <GainReductionMeter
          value={$visualizerData.limiterGR}
          label="GR"
          accent="red"
        />
      </div>
    </Section>

    <!-- Output Stage -->
    <Section title="Output" accent="cyan">
      <div class="control-row">
        <Knob
          value={$outputGain}
          min={-24}
          max={24}
          label="Gain"
          unit="dB"
          decimals={1}
          bipolar={true}
          accent="cyan"
          on:dragstart={() => outputGain.dragStart()}
          on:dragend={() => outputGain.dragEnd()}
          on:change={(e) => outputGain.set(e.detail)}
        />
        <Knob
          value={$outputWidth}
          min={0}
          max={200}
          label="Width"
          unit="%"
          decimals={0}
          accent="cyan"
          on:dragstart={() => outputWidth.dragStart()}
          on:dragend={() => outputWidth.dragEnd()}
          on:change={(e) => outputWidth.set(e.detail)}
        />
      </div>
    </Section>

    <!-- Output Meter -->
    <div class="meter-section output-meters">
      <Meter
        levelL={$visualizerData.outputLevelL}
        levelR={$visualizerData.outputLevelR}
        label="OUT"
        size="lg"
      />
    </div>
  </main>

  <!-- Bloom post-processing overlay -->
  <BloomOverlay />
</div>

<style>
  .channel-strip {
    width: 100%;
    height: 100%;
    display: flex;
    flex-direction: column;
    background: var(--bg-darkest);
    overflow: hidden;
  }

  .channel-strip.bypassed {
    opacity: 0.6;
  }

  .channel-strip.bypassed .main-content {
    filter: grayscale(0.5);
  }

  /* Header */
  .header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 12px 20px;
    background: var(--bg-dark);
    border-bottom: 1px solid var(--bg-lighter);
  }

  .logo {
    display: flex;
    align-items: center;
    gap: 10px;
  }

  .logo-text {
    font-size: 14px;
    font-weight: 600;
    letter-spacing: 0.2em;
    color: var(--neon-cyan);
    text-shadow: 0 0 10px var(--neon-cyan), 0 0 20px var(--neon-cyan-dim);
  }

  .logo-divider {
    color: var(--text-dim);
  }

  .product-name {
    font-size: 11px;
    font-weight: 500;
    letter-spacing: 0.15em;
    color: var(--text-secondary);
  }

  .header-controls {
    display: flex;
    gap: 12px;
  }

  /* Main Content */
  .main-content {
    flex: 1;
    display: flex;
    gap: var(--section-gap);
    padding: 12px;
    overflow-x: auto;
  }

  /* Meter Sections */
  .meter-section {
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 12px;
    background: var(--bg-panel);
    border-radius: var(--border-radius);
    min-width: 60px;
  }

  .input-meters {
    height: 100%;
  }

  .output-meters {
    height: 100%;
    min-width: 80px;
  }

  /* Control Layouts */
  .control-row {
    display: flex;
    gap: 16px;
    justify-content: center;
    flex-wrap: wrap;
  }

  .button-row {
    display: flex;
    gap: 8px;
    justify-content: center;
  }

  /* EQ Bands */
  .eq-bands {
    display: flex;
    gap: 12px;
  }

  .eq-band {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 8px;
    padding: 8px;
    background: var(--bg-light);
    border-radius: var(--border-radius-sm);
  }

  .band-label {
    font-size: 8px;
    font-weight: 600;
    letter-spacing: 0.1em;
    color: var(--neon-green);
    text-shadow: 0 0 4px var(--neon-green);
  }

  /* HPF Slope Buttons */
  .slope-select {
    display: flex;
    gap: 4px;
  }

  .slope-btn {
    padding: 4px 8px;
    font-size: 8px;
    font-weight: 500;
    letter-spacing: 0.05em;
    background: var(--bg-light);
    border: 1px solid var(--bg-lighter);
    border-radius: var(--border-radius-sm);
    color: var(--text-muted);
    cursor: pointer;
    transition: all 0.15s ease;
  }

  .slope-btn:hover {
    background: var(--bg-lighter);
    color: var(--text-secondary);
  }

  .slope-btn.active {
    background: var(--bg-lighter);
    border-color: var(--neon-magenta);
    color: var(--neon-magenta);
    box-shadow: 0 0 4px var(--neon-magenta-dim);
  }

  /* Dynamics Sections */
  .dynamics-content {
    display: flex;
    gap: 16px;
    align-items: stretch;
  }

  .control-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 12px;
  }

  .comp-grid {
    grid-template-columns: repeat(3, 1fr);
  }

  .limiter-grid {
    grid-template-columns: repeat(2, 1fr);
  }

  /* Responsive adjustments */
  @media (max-width: 1100px) {
    .eq-bands {
      flex-wrap: wrap;
    }

    .comp-grid {
      grid-template-columns: repeat(2, 1fr);
    }
  }
</style>
