<script lang="ts">
  export let value: number = 0; // Gain reduction in dB (negative or 0)
  export let label: string = 'GR';
  export let maxDb: number = -24;
  export let accent: string = 'yellow';

  // Convert GR to display percentage (0 = no reduction, maxDb = full)
  $: displayValue = value.toFixed(1);
  $: percent = Math.min(100, Math.max(0, (Math.abs(value) / Math.abs(maxDb)) * 100));

  $: accentColor = `var(--neon-${accent})`;
</script>

<div class="gr-meter" style="--accent: {accentColor}">
  <div class="gr-label">{label}</div>

  <div class="gr-bar-container">
    <div class="gr-track">
      <div
        class="gr-fill"
        style="height: {percent}%"
      ></div>
    </div>

    <!-- Scale -->
    <div class="gr-scale">
      {#each [0, -6, -12, -18, -24] as mark}
        <div class="gr-mark" style="top: {(Math.abs(mark) / Math.abs(maxDb)) * 100}%">
          <span class="gr-tick"></span>
          <span class="gr-value">{mark}</span>
        </div>
      {/each}
    </div>
  </div>

  <div class="gr-readout">{displayValue}</div>
</div>

<style>
  .gr-meter {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 6px;
    width: 32px;
  }

  .gr-label {
    font-size: 8px;
    font-weight: 600;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    color: var(--accent);
    text-shadow: 0 0 4px var(--accent);
  }

  .gr-bar-container {
    position: relative;
    width: 100%;
    height: 100%;
    min-height: 80px;
    display: flex;
    gap: 4px;
  }

  .gr-track {
    flex: 1;
    background: var(--meter-track);
    border-radius: 2px;
    position: relative;
    overflow: hidden;
  }

  .gr-fill {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    background: linear-gradient(to bottom, var(--accent), transparent);
    transition: height 0.05s linear;
    box-shadow: 0 0 8px var(--accent);
  }

  .gr-scale {
    position: relative;
    width: 18px;
  }

  .gr-mark {
    position: absolute;
    display: flex;
    align-items: center;
    gap: 2px;
    transform: translateY(-50%);
  }

  .gr-tick {
    width: 3px;
    height: 1px;
    background: var(--text-dim);
  }

  .gr-value {
    font-family: var(--font-mono);
    font-size: 7px;
    color: var(--text-dim);
  }

  .gr-readout {
    font-family: var(--font-mono);
    font-size: 9px;
    font-weight: 500;
    color: var(--accent);
    text-shadow: 0 0 4px var(--accent);
  }
</style>
