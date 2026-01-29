<script lang="ts">
  import { onMount, onDestroy } from 'svelte';
  import * as THREE from 'three';
  import { EffectComposer } from 'three/examples/jsm/postprocessing/EffectComposer.js';
  import { RenderPass } from 'three/examples/jsm/postprocessing/RenderPass.js';
  import { UnrealBloomPass } from 'three/examples/jsm/postprocessing/UnrealBloomPass.js';

  let container: HTMLDivElement;
  let scene: THREE.Scene;
  let camera: THREE.OrthographicCamera;
  let renderer: THREE.WebGLRenderer;
  let composer: EffectComposer;
  let animationId: number;

  // Bloom parameters
  const bloomStrength = 0.8;
  const bloomRadius = 0.4;
  const bloomThreshold = 0.2;

  onMount(() => {
    init();
    animate();

    window.addEventListener('resize', handleResize);
    return () => {
      window.removeEventListener('resize', handleResize);
      if (animationId) cancelAnimationFrame(animationId);
      renderer?.dispose();
    };
  });

  function init() {
    const width = container.clientWidth;
    const height = container.clientHeight;

    // Scene
    scene = new THREE.Scene();
    scene.background = null; // Transparent

    // Orthographic camera for 2D overlay
    camera = new THREE.OrthographicCamera(
      width / -2, width / 2,
      height / 2, height / -2,
      0.1, 1000
    );
    camera.position.z = 100;

    // Renderer with transparency
    renderer = new THREE.WebGLRenderer({
      alpha: true,
      antialias: true,
      powerPreference: 'high-performance'
    });
    renderer.setSize(width, height);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    renderer.setClearColor(0x000000, 0);
    container.appendChild(renderer.domElement);

    // Post-processing
    composer = new EffectComposer(renderer);

    const renderPass = new RenderPass(scene, camera);
    renderPass.clear = true;
    renderPass.clearAlpha = 0;
    composer.addPass(renderPass);

    const bloomPass = new UnrealBloomPass(
      new THREE.Vector2(width, height),
      bloomStrength,
      bloomRadius,
      bloomThreshold
    );
    composer.addPass(bloomPass);

    // Create glow points for LEDs (these will be synced with actual LED positions)
    createGlowGeometry();
  }

  function createGlowGeometry() {
    // Create a few ambient glow points that represent UI glow
    // In a full implementation, you'd sync these with actual LED elements
    const geometry = new THREE.CircleGeometry(5, 32);

    // Cyan glow
    const cyanMaterial = new THREE.MeshBasicMaterial({
      color: 0x00f0ff,
      transparent: true,
      opacity: 0.6
    });

    // Create scattered glow points
    const glowPoints = [
      { x: -300, y: 200, color: 0x00f0ff },
      { x: -100, y: 200, color: 0xff00ff },
      { x: 100, y: 200, color: 0x00ff88 },
      { x: 250, y: 200, color: 0xff6600 },
      { x: 400, y: 200, color: 0xffee00 },
    ];

    glowPoints.forEach(point => {
      const material = new THREE.MeshBasicMaterial({
        color: point.color,
        transparent: true,
        opacity: 0.3
      });
      const mesh = new THREE.Mesh(geometry.clone(), material);
      mesh.position.set(point.x, point.y, 0);
      mesh.scale.set(2, 2, 1);
      scene.add(mesh);
    });
  }

  function handleResize() {
    const width = container.clientWidth;
    const height = container.clientHeight;

    camera.left = width / -2;
    camera.right = width / 2;
    camera.top = height / 2;
    camera.bottom = height / -2;
    camera.updateProjectionMatrix();

    renderer.setSize(width, height);
    composer.setSize(width, height);
  }

  function animate() {
    animationId = requestAnimationFrame(animate);

    // Subtle animation
    scene.children.forEach((child, i) => {
      if (child instanceof THREE.Mesh) {
        child.material.opacity = 0.2 + Math.sin(Date.now() * 0.002 + i) * 0.1;
      }
    });

    composer.render();
  }
</script>

<div class="bloom-overlay" bind:this={container}></div>

<style>
  .bloom-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    pointer-events: none;
    z-index: 1000;
    mix-blend-mode: screen;
    opacity: 0.5;
  }
</style>
