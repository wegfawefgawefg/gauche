# Multiplayer playtesting priority — 2026-09-15

User needs friends to discover, join and remain in a party over the internet,
without forwarding a port on the hosting machine. This takes priority over the
next content slice. This document records the implementation scope and audit;
it does not claim that the missing integration is complete.

## Current Gauche state

- Direct IPv4 UDP hosting/joining; four players, one local player per machine.
- Host-canonical inputs, client prediction, bounded rollback, confirmed hashes,
  chunked snapshot recovery and reconnection using a retained local identity.
- Same-machine session, rollback, codec and lossy transport checks exist under
  `tests/`. Their existence is not proof of current internet playtest stability.
  No live multiplayer test was run for this audit.
- GView menus currently expose direct host address/port. No Gauche room browser,
  room-code integration, NAT punching or relayed game transport is wired in.
- No persistent desync input/snapshot capture comparable to Splonks' `.sdrp`
  dumps. Current recovery can overwrite useful evidence of the first divergence.
- Networking retry/timeout counters advance once per `pump_network` call, which
  runs per rendered frame. Replace those with monotonic elapsed time before
  relying on mixed-refresh-rate sessions; render FPS must not set timeout length.
- Friendly fire is currently enabled by behavior: melee and bullets do not
  exclude teammates. There is no lobby disable setting. Requested rule is ON
  by default, with a host-controlled pre-run option to disable it.

## Immediate route for friends

Reachable LAN or publicly reachable friend host can use the current direct path.
An overlay network such as Tailscale can supply reachability without router port
forwarding. Both players install it and authorize access to the host device; then
join the host's Tailscale IPv4 address. Gauche has not been live-tested over this
path yet. Host OS firewall/access policies must permit the game's UDP port.

Both players should use the same Gauche revision. Example explicit-port launch:

```sh
# Host
./scripts/run.sh --host 39000
# Friend: replace HOST_TAILSCALE_IP with the host's Tailscale IPv4 address
./scripts/run.sh --join HOST_TAILSCALE_IP:39000
```

The official [private game server guide](https://tailscale.com/docs/use-cases/personal-or-at-home-use/share-private-game-server)
explains sharing with friends. Tailscale can fall back to a relay when direct
connectivity fails. This is an interim playtesting route, not the intended
in-game room-discovery experience.

## Native room and party integration

Use Gubsy Realnet and the existing Splonks integrations as the source:

- `Splonks/gubsy/tools/room_server`: room directory, rendezvous and UDP relay.
- `Splonks/splonks-cpp/src/network`: mature transport, lifecycle, lockstep
  diagnostics and capture behavior.
- `Splonks/splonks-cpp-gview-menu/src/network/net_lobby_realnet.cpp` and that
  workspace's menu integration: adapt its working connection flow to Gauche UI.
- `Splonks/splonks-cpp/src/network/net_lobby_input_lockstep.cpp`: bounded desync
  replay capture; `.sdrp` artifacts and a LAN verdict exist in its logs directory.
  Recorded Splonks validation is useful reference evidence, not Gauche validation.

Implement a Gauche transport adapter using direct UDP, authenticated punch and
relay fallback without coupling the service to Gauche simulation. Use the same
socket for endpoint discovery and gameplay. Preserve distinct relay peer routes,
packet-size limits, cancellation, retry and reconnect state.

One publicly reachable service deployment can provide room discovery, punch
coordination and opaque packet relay. A small Linux VPS is a suitable starting
deployment; it does not need to render or simulate the game. Determine whether
the existing Splonks service can be reused, with game/version separation, before
creating another server. Do not assume an old deployment is still running.
Punching alone is insufficient for restrictive NAT; include relay fallback.

Gauche GView flow: Host Room -> named party with code -> friends browse or enter
code -> see members, connection status and agreed settings -> ready -> host starts.
Keep party membership across death, rewards, floors and restarts. Show useful
connection failures and version mismatch messages, retry/cancel and rejoin.
Keep direct-IP entry available. Prefer clear primary actions and a separate Back
button; do not bring back the old oversized internal menus.

## Diagnostics and lobby rules

Capture version/build, session and peer identity, transport transitions,
connection/rejoin lifecycle, input acknowledgements, corrections, confirmed hash
mismatches and snapshot transfers. Use bounded logs and replay capture with an
initial snapshot plus input history, expected/actual hashes and first-divergence
tick. Save evidence before resync. Add useful ImGui latency/loss/rollback/pending
snapshot information and a clear path to files friends can send back. Exclude
credentials and room authorization secrets from logs.

Add friendly-fire ON/OFF to host lobby settings, ON by default. Store the agreed
rule in deterministic run state, snapshots and hashes; preserve it on restart.
Audit melee, projectiles, explosions, reflected attacks, traps, damage-over-time
and pushing. Source ownership must survive delayed effects; don't infer shooter
identity from whoever happens to occupy an old attacker cell. Explicitly specify
self-damage and environmental accidents separately from teammate damage.

Include the previously requested team-pause versus local-menu policy. Party
settings must agree before the run starts, and a local menu must not pretend to
freeze a live multiplayer world.

Build and use focused existing checks as needed, without another large test
suite or autonomous playthrough. Provide forced direct/punch/relay paths for the
user and friends to verify across actual networks. Track real-machine connection,
rejoin, floor/reward/death transitions and divergence evidence honestly before
claiming multiplayer playtesting is ready.
