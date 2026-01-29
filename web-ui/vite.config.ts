import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

export default defineConfig({
  plugins: [svelte()],

  // Relative paths for local file loading in WebView
  base: './',

  build: {
    // Output to Resources/WebUI for JUCE to bundle
    outDir: '../Resources/WebUI',

    // Clean output directory before build
    emptyOutDir: true,

    // Predictable asset naming for resource provider
    rollupOptions: {
      output: {
        entryFileNames: 'assets/index.js',
        chunkFileNames: 'assets/[name].js',
        assetFileNames: 'assets/[name].[ext]',
      },
    },
  },

  server: {
    port: 5173,
    strictPort: true,
    cors: true,
  },
});
