# Renderer

`src/renderer/device.hpp` is the game's drawing boundary. It owns textures,
render targets, ordered triangle batches, clipping, blend state, readback and
presentation. SDL still supplies windows, input, audio and image/font decoding.
Game drawing uses `tr::` functions. A compile-time adapter in `sdl_bridge.hpp`
connects the pinned Gubsy/GView/ImGui drawing calls to the same renderer without
modifying those dependencies or SDL's implementation.

## Backends

- Linux: Vulkan, with an intermediate RGBA target and a window swapchain.
  Requires Vulkan headers/loader when building and an installed Vulkan driver.
- Browser: requests a WebGPU adapter/device at startup. If unavailable or creation
  fails, uses WebGL 2. Append `?renderer=webgl2` to force the fallback for comparisons.
  Browser animation timing remains requestAnimationFrame-driven.
- macOS: deferred. There is no native Metal backend in this change.

This implements the game's 2D renderer. It does not yet provide a 3D scene API,
depth-buffered meshes, or a general game-facing material editor. Backend shader
sources can now be changed directly; additional effects no longer depend on
SDL_Renderer's fixed shading path.

## Drawing and lighting

Adjacent compatible draws merge without changing their order. Immutable sprite
files share padded atlas pages; render targets and transient font textures have
independent allocations. Vertex/index buffers grow as needed and are reused.
Vulkan keeps one submission in flight and waits before reusing its resources.
Texture updates and readbacks flush pending draws to preserve ordering.

Terrain keeps four vertices and two triangles per tile. A separate lightmap
contains the existing corner samples. The fragment shader interpolates the
lightmap across each square; sprite textures retain nearest filtering. Source
placement, propagation and gameplay are unchanged. Lightmaps upload only when
the lighting cache changes. F1 → Presentation → **Bilinear terrain lightmap**
switches back to vertex corner lighting for visual comparison. The same section
shows the backend, batches, triangles and atlas page count. These counters are
not GPU execution timings.

For a browser zoom stress test, enable F1 → Presentation → **Allow zoom below 2x**,
close F1 and use `-` / `+` down to 0.5x. `?debugzoom=1` enables this on startup.
Disabling the option restores the 2x minimum. Wheel zoom remains disabled.

Vulkan shaders are in `shaders/renderer/`. Regenerate the checked-in SPIR-V header
with `python3 tools/compile_renderer_shaders.py` (requires `glslangValidator`).
WebGL shaders live in `src/renderer/webgl.cpp`; WGSL and browser device operations
live in `src/renderer/webgpu_runtime.js`.

## Checks

```sh
cmake --build build-release --target teeming teeming_renderer_tests -j 8
ctest --test-dir build-release -R '^renderer$' --output-on-failure
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy TEEMING_VK_VALIDATION=1 \
  build-release/teeming --smoke-run --frames 4 --capture /tmp/teeming-vulkan.bmp
./scripts/build-web.sh
# Serve build-web/site locally on port 8787, then:
xvfb-run -a npm run test:web:renderer
npm run test:web
npm run test:web:settings
```

The pixel test checks clipping, scaling, blending, lightmap interpolation,
render-target orientation, atlas batching and pixel format conversion. Browser
checks exercise WebGPU, forced WebGL 2, adapter/device failure fallback, resizing,
gameplay and ImGui. Vulkan validation needs the optional validation-layer package.
Dummy-driver native checks exercise offscreen Vulkan; window presentation and
native fullscreen still need a desktop playtest. On Linux, WebGPU capture needs Xvfb and Vulkan compositing (the test sets
the Chromium flags). Headless browser tests use software GPU drivers and are correctness checks, not performance benchmarks.
