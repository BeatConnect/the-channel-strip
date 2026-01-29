<script lang="ts">
  import { createEventDispatcher } from 'svelte';

  export let value: number = 0.5;
  export let min: number = 0;
  export let max: number = 1;
  export let label: string = '';
  export let unit: string = '';
  export let decimals: number = 1;
  export let bipolar: boolean = false;
  export let size: 'sm' | 'md' | 'lg' = 'md';
  export let accent: string = 'cyan';
  export let disabled: boolean = false;

  const dispatch = createEventDispatcher();

  let isDragging = false;
  let startY = 0;
  let startValue = 0;

  // Size configurations
  const sizes = {
    sm: { outer: 32, inner: 24, stroke: 2 },
    md: { outer: 44, inner: 34, stroke: 2.5 },
    lg: { outer: 56, inner: 44, stroke: 3 },
  };

  $: config = sizes[size];
  $: normalizedValue = (value - min) / (max - min);
  $: displayValue = value.toFixed(decimals);

  // Arc calculations
  const startAngle = -135;
  const endAngle = 135;
  const totalAngle = endAngle - startAngle;

  function getArcPath(normalized: number, radius: number): string {
    const angle = startAngle + (normalized * totalAngle);
    const startRad = (startAngle * Math.PI) / 180;
    const endRad = (angle * Math.PI) / 180;

    const x1 = radius + radius * Math.sin(startRad);
    const y1 = radius - radius * Math.cos(startRad);
    const x2 = radius + radius * Math.sin(endRad);
    const y2 = radius - radius * Math.cos(endRad);

    const largeArc = normalized > 0.5 ? 1 : 0;

    return `M ${x1} ${y1} A ${radius} ${radius} 0 ${largeArc} 1 ${x2} ${y2}`;
  }

  function handleMouseDown(e: MouseEvent) {
    if (disabled) return;
    isDragging = true;
    startY = e.clientY;
    startValue = normalizedValue;
    dispatch('dragstart');
    window.addEventListener('mousemove', handleMouseMove);
    window.addEventListener('mouseup', handleMouseUp);
  }

  function handleMouseMove(e: MouseEvent) {
    if (!isDragging) return;
    const delta = (startY - e.clientY) / 150;
    const newNormalized = Math.max(0, Math.min(1, startValue + delta));
    const newValue = min + newNormalized * (max - min);
    value = newValue;
    dispatch('change', value);
  }

  function handleMouseUp() {
    isDragging = false;
    dispatch('dragend');
    window.removeEventListener('mousemove', handleMouseMove);
    window.removeEventListener('mouseup', handleMouseUp);
  }

  function handleDoubleClick() {
    if (disabled) return;
    // Reset to center for bipolar, minimum for unipolar
    const defaultValue = bipolar ? (min + max) / 2 : min;
    value = defaultValue;
    dispatch('change', value);
  }

  // Accent color CSS variable
  $: accentColor = `var(--neon-${accent})`;
  $: accentDimColor = `var(--neon-${accent}-dim)`;
</script>

<div class="knob-container" class:disabled style="--accent: {accentColor}; --accent-dim: {accentDimColor};">
  <svg
    class="knob"
    width={config.outer}
    height={config.outer}
    viewBox="0 0 {config.outer} {config.outer}"
    on:mousedown={handleMouseDown}
    on:dblclick={handleDoubleClick}
  >
    <!-- Background track -->
    <circle
      cx={config.outer / 2}
      cy={config.outer / 2}
      r={(config.outer - config.stroke) / 2}
      fill="var(--knob-bg)"
      stroke="var(--knob-ring)"
      stroke-width={config.stroke}
    />

    <!-- Value arc -->
    {#if normalizedValue > 0.001}
      <path
        d={getArcPath(normalizedValue, (config.outer - config.stroke) / 2)}
        fill="none"
        stroke="var(--accent)"
        stroke-width={config.stroke}
        stroke-linecap="round"
        class="value-arc"
        transform="translate({config.stroke / 2}, {config.stroke / 2})"
      />
    {/if}

    <!-- Center indicator -->
    <circle
      cx={config.outer / 2}
      cy={config.outer / 2}
      r={config.inner / 2}
      fill="var(--bg-medium)"
    />

    <!-- Indicator line -->
    <line
      x1={config.outer / 2}
      y1={config.outer / 2}
      x2={config.outer / 2}
      y2={config.outer / 2 - config.inner / 2 + 4}
      stroke="var(--accent)"
      stroke-width={2}
      stroke-linecap="round"
      transform="rotate({startAngle + normalizedValue * totalAngle}, {config.outer / 2}, {config.outer / 2})"
      class="indicator"
    />
  </svg>

  {#if label}
    <div class="label">{label}</div>
  {/if}

  <div class="value">{displayValue}{unit}</div>
</div>

<style>
  .knob-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 4px;
    user-select: none;
  }

  .knob-container.disabled {
    opacity: 0.4;
    pointer-events: none;
  }

  .knob {
    cursor: ns-resize;
    transition: filter 0.15s ease;
  }

  .knob:hover {
    filter: brightness(1.1);
  }

  .value-arc {
    filter: drop-shadow(0 0 4px var(--accent));
  }

  .indicator {
    filter: drop-shadow(0 0 3px var(--accent));
  }

  .label {
    font-size: 8px;
    font-weight: 500;
    letter-spacing: 0.1em;
    text-transform: uppercase;
    color: var(--text-muted);
  }

  .value {
    font-family: var(--font-mono);
    font-size: 9px;
    font-weight: 500;
    color: var(--text-secondary);
    min-width: 40px;
    text-align: center;
  }
</style>
