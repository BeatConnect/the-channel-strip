<script lang="ts">
  import { createEventDispatcher } from 'svelte';

  export let active: boolean = false;
  export let label: string = '';
  export let accent: string = 'cyan';
  export let size: 'sm' | 'md' = 'md';

  const dispatch = createEventDispatcher();

  function handleClick() {
    active = !active;
    dispatch('change', active);
  }

  $: accentColor = `var(--neon-${accent})`;
</script>

<button
  class="toggle-button"
  class:active
  class:sm={size === 'sm'}
  style="--accent: {accentColor}"
  on:click={handleClick}
>
  <span class="led" class:active></span>
  {#if label}
    <span class="label">{label}</span>
  {/if}
</button>

<style>
  .toggle-button {
    display: flex;
    align-items: center;
    gap: 6px;
    padding: 6px 10px;
    background: var(--bg-light);
    border: 1px solid var(--bg-lighter);
    border-radius: var(--border-radius-sm);
    color: var(--text-muted);
    cursor: pointer;
    transition: all 0.15s ease;
  }

  .toggle-button.sm {
    padding: 4px 8px;
    gap: 4px;
  }

  .toggle-button:hover {
    background: var(--bg-lighter);
    color: var(--text-secondary);
  }

  .toggle-button.active {
    background: var(--bg-lighter);
    border-color: var(--accent);
    color: var(--accent);
  }

  .led {
    width: 6px;
    height: 6px;
    border-radius: 50%;
    background: var(--bg-medium);
    border: 1px solid var(--text-dim);
    transition: all 0.15s ease;
  }

  .led.active {
    background: var(--accent);
    border-color: var(--accent);
    box-shadow: 0 0 6px var(--accent), 0 0 12px var(--accent);
  }

  .label {
    font-size: 9px;
    font-weight: 500;
    letter-spacing: 0.08em;
    text-transform: uppercase;
  }
</style>
