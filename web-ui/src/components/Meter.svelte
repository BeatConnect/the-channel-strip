<script lang="ts">
  import { onMount } from 'svelte';

  export let levelL: number = 0;
  export let levelR: number = 0;
  export let showStereo: boolean = true;
  export let label: string = '';
  export let vertical: boolean = true;
  export let size: 'sm' | 'md' | 'lg' = 'md';
  export let showPeakHold: boolean = true;
  export let showScale: boolean = true;

  // Convert linear level to dB for display
  function linearToDb(level: number): number {
    if (level < 0.00001) return -80;
    return 20 * Math.log10(level);
  }

  // Map dB to meter percentage (-60dB to +6dB range)
  function dbToPercent(db: number): number {
    const minDb = -60;
    const maxDb = 6;
    return Math.max(0, Math.min(100, ((db - minDb) / (maxDb - minDb)) * 100));
  }

  // Get color based on level
  function getColor(db: number): string {
    if (db > 0) return 'var(--meter-red)';
    if (db > -6) return 'var(--meter-orange)';
    if (db > -12) return 'var(--meter-yellow)';
    return 'var(--meter-green)';
  }

  let dbL: number;
  let dbR: number;
  let percentL: number;
  let percentR: number;
  let peakL: number = 0;
  let peakR: number = 0;
  let peakHoldL: number = 0;
  let peakHoldR: number = 0;
  let peakDecayCounter = 0;

  $: {
    dbL = linearToDb(levelL);
    dbR = linearToDb(levelR);
    percentL = dbToPercent(dbL);
    percentR = dbToPercent(dbR);

    // Peak hold logic
    if (percentL > peakHoldL) {
      peakHoldL = percentL;
      peakDecayCounter = 0;
    }
    if (percentR > peakHoldR) {
      peakHoldR = percentR;
      peakDecayCounter = 0;
    }
  }

  // Peak decay
  onMount(() => {
    const interval = setInterval(() => {
      peakDecayCounter++;
      if (peakDecayCounter > 30) { // Hold for ~1 second at 30fps
        peakHoldL = Math.max(0, peakHoldL - 2);
        peakHoldR = Math.max(0, peakHoldR - 2);
      }
    }, 33);

    return () => clearInterval(interval);
  });

  // Size configurations
  const widths = { sm: 8, md: 12, lg: 16 };
  $: barWidth = widths[size];

  // Scale marks
  const scaleMarks = [0, -6, -12, -24, -36, -48, -60];
</script>

<div class="meter-container" class:vertical class:horizontal={!vertical}>
  {#if label}
    <div class="meter-label">{label}</div>
  {/if}

  <div class="meter-body">
    {#if showScale && vertical}
      <div class="scale">
        {#each scaleMarks as mark}
          <div class="scale-mark" style="bottom: {dbToPercent(mark)}%">
            <span class="scale-value">{mark}</span>
            <span class="scale-tick"></span>
          </div>
        {/each}
      </div>
    {/if}

    <div class="bars" style="--bar-width: {barWidth}px">
      <!-- Left channel -->
      <div class="bar-container">
        <div class="bar-track">
          <div
            class="bar-fill"
            style="height: {percentL}%; background: linear-gradient(to top, var(--meter-green) 0%, var(--meter-green) 60%, var(--meter-yellow) 75%, var(--meter-orange) 88%, var(--meter-red) 100%);"
          ></div>
          {#if showPeakHold && peakHoldL > 0}
            <div
              class="peak-hold"
              style="bottom: {peakHoldL}%; background: {getColor(dbL)}"
            ></div>
          {/if}
        </div>

        <!-- LED segments overlay for digital look -->
        <div class="led-overlay">
          {#each Array(24) as _, i}
            <div
              class="led-segment"
              class:active={percentL > (i / 24) * 100}
              style="--segment-color: {i >= 20 ? 'var(--meter-red)' : i >= 17 ? 'var(--meter-orange)' : i >= 14 ? 'var(--meter-yellow)' : 'var(--meter-green)'}"
            ></div>
          {/each}
        </div>
      </div>

      {#if showStereo}
        <!-- Right channel -->
        <div class="bar-container">
          <div class="bar-track">
            <div
              class="bar-fill"
              style="height: {percentR}%; background: linear-gradient(to top, var(--meter-green) 0%, var(--meter-green) 60%, var(--meter-yellow) 75%, var(--meter-orange) 88%, var(--meter-red) 100%);"
            ></div>
            {#if showPeakHold && peakHoldR > 0}
              <div
                class="peak-hold"
                style="bottom: {peakHoldR}%; background: {getColor(dbR)}"
              ></div>
            {/if}
          </div>

          <div class="led-overlay">
            {#each Array(24) as _, i}
              <div
                class="led-segment"
                class:active={percentR > (i / 24) * 100}
                style="--segment-color: {i >= 20 ? 'var(--meter-red)' : i >= 17 ? 'var(--meter-orange)' : i >= 14 ? 'var(--meter-yellow)' : 'var(--meter-green)'}"
              ></div>
            {/each}
          </div>
        </div>
      {/if}
    </div>

    <div class="db-readout">
      <span class="db-value">{dbL > -60 ? dbL.toFixed(1) : '-inf'}</span>
      <span class="db-unit">dB</span>
    </div>
  </div>
</div>

<style>
  .meter-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 6px;
  }

  .meter-label {
    font-size: 8px;
    font-weight: 500;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    color: var(--text-muted);
  }

  .meter-body {
    display: flex;
    gap: 4px;
    align-items: stretch;
  }

  .vertical .meter-body {
    flex-direction: row;
    height: 100%;
  }

  .scale {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    position: relative;
    height: 100%;
    padding: 2px 0;
  }

  .scale-mark {
    position: absolute;
    display: flex;
    align-items: center;
    gap: 2px;
    transform: translateY(50%);
  }

  .scale-value {
    font-family: var(--font-mono);
    font-size: 7px;
    color: var(--text-dim);
    width: 20px;
    text-align: right;
  }

  .scale-tick {
    width: 3px;
    height: 1px;
    background: var(--text-dim);
  }

  .bars {
    display: flex;
    gap: 2px;
  }

  .bar-container {
    position: relative;
    width: var(--bar-width);
    height: 100%;
    min-height: 120px;
  }

  .bar-track {
    position: absolute;
    inset: 0;
    background: var(--meter-track);
    border-radius: 2px;
    overflow: hidden;
  }

  .bar-fill {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    transition: height 0.05s linear;
    border-radius: 2px;
  }

  .peak-hold {
    position: absolute;
    left: 0;
    right: 0;
    height: 2px;
    transition: bottom 0.05s linear;
    box-shadow: 0 0 4px currentColor;
  }

  .led-overlay {
    position: absolute;
    inset: 0;
    display: flex;
    flex-direction: column-reverse;
    gap: 1px;
    padding: 1px;
    pointer-events: none;
  }

  .led-segment {
    flex: 1;
    background: transparent;
    border-radius: 1px;
    opacity: 0;
    transition: opacity 0.05s ease;
  }

  .led-segment.active {
    opacity: 1;
    background: var(--segment-color);
    box-shadow: 0 0 4px var(--segment-color), 0 0 8px var(--segment-color);
  }

  .db-readout {
    display: flex;
    flex-direction: column;
    align-items: center;
    margin-top: 4px;
  }

  .db-value {
    font-family: var(--font-mono);
    font-size: 10px;
    font-weight: 500;
    color: var(--text-primary);
    min-width: 36px;
    text-align: center;
  }

  .db-unit {
    font-size: 7px;
    color: var(--text-dim);
  }
</style>
