/**
 * Svelte Stores for JUCE Parameter Binding
 */

import { writable, derived, type Writable, type Readable } from 'svelte/store';
import { onMount, onDestroy } from 'svelte';
import {
  getSliderState,
  getToggleState,
  getComboBoxState,
  addCustomEventListener,
  isInJuceWebView,
  type SliderState,
  type ToggleState,
  type ComboBoxState,
} from '../lib/juce-bridge';

// ==============================================================================
// Slider Parameter Store
// ==============================================================================

export interface SliderStore {
  subscribe: Writable<number>['subscribe'];
  set: (value: number) => void;
  dragStart: () => void;
  dragEnd: () => void;
  getScaled: () => number;
}

export function createSliderStore(paramId: string, defaultValue = 0.5): SliderStore {
  const { subscribe, set, update } = writable(defaultValue);
  let state: SliderState;

  // Initialize state
  if (typeof window !== 'undefined') {
    state = getSliderState(paramId);

    if (isInJuceWebView()) {
      set(state.getNormalisedValue());
    }

    state.valueChangedEvent.addListener(() => {
      set(state.getNormalisedValue());
    });
  }

  return {
    subscribe,
    set: (value: number) => {
      set(value);
      state?.setNormalisedValue(value);
    },
    dragStart: () => state?.sliderDragStarted(),
    dragEnd: () => state?.sliderDragEnded(),
    getScaled: () => state?.getScaledValue() ?? defaultValue,
  };
}

// ==============================================================================
// Toggle Parameter Store
// ==============================================================================

export interface ToggleStore {
  subscribe: Writable<boolean>['subscribe'];
  set: (value: boolean) => void;
  toggle: () => void;
}

export function createToggleStore(paramId: string, defaultValue = false): ToggleStore {
  const { subscribe, set, update } = writable(defaultValue);
  let state: ToggleState;
  let currentValue = defaultValue;

  // Track value for toggle function
  subscribe(v => currentValue = v);

  if (typeof window !== 'undefined') {
    state = getToggleState(paramId);

    if (isInJuceWebView()) {
      set(state.getValue());
    }

    state.valueChangedEvent.addListener(() => {
      set(state.getValue());
    });
  }

  return {
    subscribe,
    set: (value: boolean) => {
      set(value);
      state?.setValue(value);
    },
    toggle: () => {
      const newValue = !currentValue;
      set(newValue);
      state?.setValue(newValue);
    },
  };
}

// ==============================================================================
// ComboBox Parameter Store
// ==============================================================================

export interface ComboStore {
  subscribe: Writable<number>['subscribe'];
  set: (index: number) => void;
  choices: Readable<string[]>;
}

export function createComboStore(paramId: string, defaultIndex = 0): ComboStore {
  const { subscribe, set } = writable(defaultIndex);
  const choicesStore = writable<string[]>([]);
  let state: ComboBoxState;

  if (typeof window !== 'undefined') {
    state = getComboBoxState(paramId);

    if (isInJuceWebView()) {
      set(state.getChoiceIndex());
      choicesStore.set(state.getChoices());
    }

    state.valueChangedEvent.addListener(() => {
      set(state.getChoiceIndex());
    });

    state.propertiesChangedEvent.addListener(() => {
      choicesStore.set(state.getChoices());
    });
  }

  return {
    subscribe,
    set: (index: number) => {
      set(index);
      state?.setChoiceIndex(index);
    },
    choices: { subscribe: choicesStore.subscribe },
  };
}

// ==============================================================================
// Visualizer Data Store
// ==============================================================================

export interface VisualizerData {
  inputLevelL: number;
  inputLevelR: number;
  outputLevelL: number;
  outputLevelR: number;
  gateGR: number;
  compGR: number;
  limiterGR: number;
}

const defaultVisualizerData: VisualizerData = {
  inputLevelL: 0,
  inputLevelR: 0,
  outputLevelL: 0,
  outputLevelR: 0,
  gateGR: 0,
  compGR: 0,
  limiterGR: 0,
};

export const visualizerData = writable<VisualizerData>(defaultVisualizerData);

// Set up visualizer listener
if (typeof window !== 'undefined') {
  addCustomEventListener('visualizerData', (data) => {
    visualizerData.set(data as VisualizerData);
  });
}
