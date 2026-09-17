# Browser multiplayer proposal

2026-09-17. Original feasibility review; implementation and operating instructions
are now in [BROWSER_BUILD.md](BROWSER_BUILD.md). Working title discussion: [Teeming](../design/NAME_AND_TRAILER.md).

## Direction

A downloadable native build plus a playable browser link would make multiplayer
playtesting easier. Recommended first experiment: Emscripten/SDL3 client served
as static assets from Cloudflare Pages, existing room directory on the VPS, and
a browser-compatible gameplay relay there. Preserve shared simulation and room
codes, aiming for browser/native crossplay.

SDL3 supports an Emscripten target, but a conventional blocking main loop needs
to yield through browser callbacks; controller, audio, asset and persistence
handling also need adaptation. [SDL platform guide](https://wiki.libsdl.org/SDL3/README-emscripten).

## Local work to reuse

These are sibling repositories, not new dependencies or copied implementations:

- `../native-gb/native-gb-web/runtime/tetris/CMakeLists.txt`: C++20 Emscripten
  target, WebGL2, memory growth, modular JS exports and IDBFS linkage.
- `../native-gb/native-gb-web/runtime/tetris/browser_main.cpp`: explicit start,
  frame, shutdown and audio-resume entry points.
- `../native-gb/native-gb-web/site/play/tetris/launcher.js`, `site/storage.js`,
  `site/_headers`: launcher/storage/deployment patterns to inspect when porting.
- `../native-gb/native-gb-web/tests/browser-smoke.js`: automated browser checks.
- `../native-gb/native-gb-web/scripts/deploy-pages.sh`: existing build/check/Pages
  deployment workflow; inspect, do not run against its existing project.
- `../native-gb/native-gb-pokemon-red-modern/docs/CONTROLS_AND_TIMING.md`:
  controller visibility after the first button gesture, periodic browser Gamepad
  sampling and SDL/Gubsy refresh without requiring a keyboard.

## Gauche-specific work

- `src/main.cpp` currently owns a blocking native loop. Extract a frame boundary
  with the same fixed-step simulation and browser-owned presentation scheduling.
- `src/net_socket.cpp` uses native UDP; `src/net/rooms/http.cpp` uses libcurl,
  with request work dispatched by `src/net/rooms/worker.cpp`. Provide browser
  asynchronous HTTP and gameplay transport implementations.
- Browser code cannot use direct UDP. WebSockets can provide the initial relay
  channel; WebRTC data channels or WebTransport are alternatives to evaluate if
  reliable-stream latency is unacceptable. This requires server-side support;
  a static deployment alone cannot replace the UDP relay. Avoid adopting the
  general POSIX socket proxy as the production architecture merely to compile.
  [Emscripten networking](https://emscripten.org/docs/porting/networking.html).
- Preserve authenticated room membership, scoped relay destinations and bounded
  messages. A WebSocket-to-existing-relay bridge must handle traversal identities,
  not expose an arbitrary UDP destination proxy.
- Verify native/Wasm deterministic hashes, snapshot compatibility, late joining,
  reconnects and shared reward/shop lifecycle. Browser tab suspension needs an
  explicit host/client recovery policy.
- Verify the pinned Gubsy/GView/SDL image/audio dependencies compile for web.
  The sibling project's Gubsy version is not proof that Gauche's pins already do.
- Load assets through a manifest; persist controls/settings in browser storage.
  Reuse click/button-to-start audio and gamepad activation patterns. Keep dev
  inspection tools available in test builds.

## Hosting

Pages can deploy prebuilt client assets. A generated `pages.dev` project address
is sufficient initially; choosing a commercial domain can wait.
[Pages deployment guide](https://developers.cloudflare.com/pages/get-started/direct-upload/).

Pages currently limits each individual asset to 25 MiB. Gauche's source `assets/`
directory is about 41 MiB; that is not a measured browser download or compiled
Wasm size. Measure the release outputs, split bundles or use separate asset
hosting where necessary. [Pages limits](https://developers.cloudflare.com/pages/platform/limits/).

## Proposed acceptance sequence

1. Local browser build: menu → generated Forest → play → next floor, keyboard
   and controller, audio activation, settings persistence, performance capture.
2. Browser room discovery/join plus authenticated relay prototype. Test browser
   host/native guest and native host/browser guest; include loss/latency and
   hidden-tab behavior before committing to the transport.
3. Two-browser smoke and native/Wasm replay/hash checks; then test larger parties
   without assuming the native 16-player result establishes browser performance.
4. Package and review a Pages preview with loading UI, matching build/version
   handling, download option and room-link flow. Deploy only as a separate release
   action once the prototype is concrete; this discussion does not choose a domain.
