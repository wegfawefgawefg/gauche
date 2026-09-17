# Teeming browser build

The website and game files are hosted on **Cloudflare Pages** at
<https://teeming.pages.dev>. The existing VPS at `https://45.77.123.14` only
runs multiplayer services: room directory, authenticated UDP rendezvous/relay,
and the browser WebSocket bridge. No game website is served by the bridge.

## Build and test

Requires Emscripten 6.0.3, CMake, Ninja, Python 3, Node and Chrome. The build
script reuses the installed SDK in the sibling native-gb-web checkout; set
`EMSDK` to another installation when needed. Dependency revisions remain pinned
in the main CMake project; SDL_ttf's vendored HarfBuzz supplies GView's font backend.

```sh
npm ci
./scripts/build-web.sh
python3 -m http.server 8787 --bind 127.0.0.1 --directory build-web/site
# In another terminal:
npm run test:web
npm run test:web:gamepad
npm run test:web:crossplay  # also requires build-release/gauche
```

`TEEMING_TEST_URL` selects a deployed URL; `CHROME` selects the Chrome binary
for the main smoke. Browser tests are headless and close themselves. The
standard-gamepad test simulates the browser Gamepad API; real controller models
still need manual testing. The crossplay test runs bounded native processes
with isolated profiles and dummy video/audio.

The launcher loads content-addressed assets, initializes IndexedDB-backed
settings at `/persistent`, and starts the normal game. The native data directory
and executable remain compatible with existing installations; the displayed
name and browser module are Teeming. Music placeholders remain disabled.

The initial output is roughly 26 MiB before HTTP compression: a 10 MiB Wasm
module and two asset packs smaller than 8 MiB each. Each file fits the Pages
25 MiB limit. Music draft directories are excluded. Assets stay code-generated.

## Browser runtime and multiplayer

- The shared fixed-step simulation runs inside the normal main loop, yielding
  through Asyncify to requestAnimationFrame. Hidden tabs use a timer fallback;
  browsers can still suspend/throttle them. Keep the hosting tab visible during
  playtests. A short suspended-client catch-up is covered, not indefinite mobile
  background hosting. Asyncify overhead is a future optimization opportunity.
- Room operations retain completed HTTP responses while awaiting the next
  fetch. Replaying an incomplete operation never repeats a successful POST.
  Native builds keep their background HTTP worker. HTTP Date anchors the same
  monotonic service clock on both platforms.
- Browser transport forces relay. Binary WebSocket messages contain a two-byte
  service port followed by the existing signed protocol packet. The bridge
  permits only the fixed VPS's ports 8789/8790, with bounded queues, message size,
  origins and connections. Existing roomd authentication and allocation limits
  still apply. Browser packets cannot select arbitrary UDP destinations.
- No game wire-format or gameplay-version change is required. Browser/native
  crossplay uses the same simulation, snapshots and room codes. The transport
  is a reliable WebSocket stream; poor-network latency needs wider playtesting.
- Late snapshot catch-up does not send synthetic historical input back to the
  host. Hosts batch late packets before replaying, and client prediction stays
  near the last known host tick. Regression tests cover the catch-up bound and
  coalescing a packet burst into one replay.

Use **Save debug log** below the canvas for browser errors, connection state,
RTT/recovery counters and recent native-format network events. Logs omit room
credentials. `window.teeming.gameState` is a read-only published diagnostic
snapshot; `window.teeming.command` queues normal menu actions for smoke tests.
Optional test links accept `?host=NAME&autostart=1`, `?room=CODE`, `?bot=1`,
`?name=NAME`; ordinary visits show the title menu.

## Deploy and operate

Commit, rebuild/package that commit, run the browser tests, then execute
`./scripts/deploy-web.sh` with Wrangler authentication or Cloudflare credentials
in the shell. The separate Pages project is `teeming`, production branch `main`.
No custom domain has been purchased or configured.

`services/browser-relay/install.py` installs the bridge on the existing dedicated
VPS, adds CORS and `/browser-relay` to Nginx, validates and reloads Nginx, and
starts `teeming-browser-relay.service`. It does not restart roomd. The bridge
runs under the existing unprivileged room service account, on loopback TCP 8791.
The Nginx pre-browser backup is retained. Reloading bridge code requires only
restarting `teeming-browser-relay`; existing browser sockets reconnect by
rejoining, while native UDP sessions remain separate.

## Remaining browser coverage

- Longer parties, mobile Safari/Firefox, real controllers and constrained laptops.
- Long tab suspension, browser-host lifecycle, reconnects and level/reward/shop
  transitions across mixed platforms; the native lifecycle suite remains useful
  but does not establish every browser case.
- Datagram transport alternatives if reliable-stream latency becomes noticeable.
- Browser-specific display settings polish; browser presentation follows its own
  animation scheduler rather than the native V-sync/frame limiter.

## First public deployment — 2026-09-17

Build `a4b1b15`, Pages deployment `bd6ae846`, production URL
<https://teeming.pages.dev>. The same published package passed the browser
smoke against that public URL: keyboard play, audio activation, saved settings,
room browsing, three clients including a late join, short client suspension and
leave. Both guests had zero recovery events before the suspension check.

Local checks also passed standard Gamepad API input and both directions of
native/browser play. Seven native menu/UI/network CTest checks passed, including
lossy networking and regressions for batched late inputs and bounded catch-up.
The old menu smoke assumed Play was the first title button; it now opens Rules
explicitly and still exercises navigation/selection of the death policy.
The deployed bridge rejected unapproved origins, arbitrary ports and oversized
WebSocket messages. Real-world latency and longer lifecycle testing remain open.
