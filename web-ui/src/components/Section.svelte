<script lang="ts">
  export let title: string;
  export let accent: string = 'cyan';
  export let enabled: boolean = true;
  export let collapsible: boolean = false;
  export let showEnable: boolean = false;

  import { createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();

  $: accentColor = `var(--neon-${accent})`;
</script>

<div class="section" class:disabled={!enabled} style="--accent: {accentColor}">
  <div class="section-header">
    <div class="section-title-row">
      {#if showEnable}
        <button class="enable-btn" class:active={enabled} on:click={() => dispatch('toggle')}>
          <span class="led" class:active={enabled}></span>
        </button>
      {/if}
      <h3 class="section-title">{title}</h3>
    </div>
    <div class="accent-line"></div>
  </div>

  <div class="section-content">
    <slot />
  </div>
</div>

<style>
  .section {
    background: var(--bg-panel);
    border-radius: var(--border-radius);
    padding: 12px;
    display: flex;
    flex-direction: column;
    gap: 12px;
    transition: opacity 0.2s ease;
  }

  .section.disabled {
    opacity: 0.5;
  }

  .section.disabled .section-content {
    pointer-events: none;
  }

  .section-header {
    display: flex;
    flex-direction: column;
    gap: 6px;
  }

  .section-title-row {
    display: flex;
    align-items: center;
    gap: 8px;
  }

  .section-title {
    font-size: 10px;
    font-weight: 600;
    letter-spacing: 0.12em;
    text-transform: uppercase;
    color: var(--accent);
    text-shadow: 0 0 8px var(--accent);
    margin: 0;
  }

  .accent-line {
    height: 1px;
    background: linear-gradient(90deg, var(--accent), transparent);
    opacity: 0.5;
  }

  .enable-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 18px;
    height: 18px;
    padding: 0;
    background: var(--bg-light);
    border: 1px solid var(--bg-lighter);
    border-radius: 3px;
    cursor: pointer;
    transition: all 0.15s ease;
  }

  .enable-btn:hover {
    background: var(--bg-lighter);
  }

  .enable-btn.active {
    border-color: var(--accent);
  }

  .led {
    width: 6px;
    height: 6px;
    border-radius: 50%;
    background: var(--bg-medium);
    transition: all 0.15s ease;
  }

  .led.active {
    background: var(--accent);
    box-shadow: 0 0 6px var(--accent), 0 0 10px var(--accent);
  }

  .section-content {
    display: flex;
    flex-direction: column;
    gap: 10px;
  }
</style>
