# Multiplayer playtesting priority — 2026-09-15

User needs friends to discover, join and remain in a party over the internet,
without forwarding a port on the hosting machine. This takes priority over the
next content slice. This document records the implementation scope and audit;
it does not claim that the missing integration is complete.

## Current Teeming state

- Direct IPv4 UDP hosting/joining; four players, one local player per machine.
- Host-canonical inputs, client prediction, bounded rollback, confirmed hashes,
  chunked snapshot recovery and reconnection using a retained local identity.
- Same-machine session, rollback, codec and lossy transport checks exist under
  `tests/`. Their existence is not proof of current internet playtest stability.
  No live multiplayer test was run for this audit.
- GView now exposes Online Rooms, named hosting, public discovery, code joining,
  party membership and ready/start controls. HTTPS defaults to the deployed
  Los Angeles service at `https://45.77.123.14`; direct address/port remains available.
- Native authenticated punch and relay transport is wired through the game's
  UDP socket. A forced-relay connection check through the actual VPS completed
  snapshot transfer, room membership and party start. Automatic mode fell back
  to relay successfully on this local network; direct punching across different
  home networks remains unverified.
- Bounded session logs and `.grpl` recovery captures now preserve the oldest
  retained snapshot, input history and local/host hashes before resync. An offline
  replay reader, richer ImGui diagnostics and cross-machine validation remain.
- Networking retry/timeout counters now use monotonic elapsed time. Snapshot
  transfers are paced and retain progress on retries. See the implemented
  [foundation milestone](NETWORK_FOUNDATION.md) and its validation limits.
- Friendly fire is currently enabled by behavior: melee and bullets do not
  exclude teammates. There is no lobby disable setting. Requested rule is ON
  by default, with a host-controlled pre-run option to disable it.

## Native room playtesting route

Both players build the same revision (`libcurl4-openssl-dev` is now required on
Debian/Ubuntu). Open **Play → Host Game**. A generated name is filled in automatically (editable
with a Shuffle button). Name the room, host it, and
share its six-character code. The friend opens **Play → Join Game**, selects the room from the automatically
refreshed public list or enters its code, then readies up; the host starts the run. Automatic connection tries
punching before relay. Force Relay and direct IP/port entry live under **Advanced Connection Options**.
See [Local Multiplayer Workshop](../guides/LOCAL_MULTIPLAYER.md) for four-window, split
workspace and headless-bot launch scripts.

The [public dashboard](https://45.77.123.14/) lists public rooms and refreshes every
two seconds. The service setup, private credential locations and operation are
recorded in [services/roomd/README.md](../../services/roomd/README.md).

Existing direct/lossy session checks pass, and the native relay handshake was
checked without running a gameplay session. This is the first internet-room
integration, not a claim of completed multiplayer hardening. Pending work includes
fresh traversal after NAT changes, per-member ready labels, host-agreed friendly
fire/pause rules, replay reader/diagnostic UI and friend-machine gameplay feedback.

## Direct-address alternative

Reachable LAN or publicly reachable friend host can use the current direct path.
An overlay network such as Tailscale can supply reachability without router port
forwarding. Both players install it and authorize access to the host device; then
join the host's Tailscale IPv4 address. Teeming has not been live-tested over this
path yet. Host OS firewall/access policies must permit the game's UDP port.

Both players should use the same Teeming revision. Example explicit-port launch:

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
  workspace's menu integration: adapt its working connection flow to Teeming UI.
- `Splonks/splonks-cpp/src/network/net_lobby_input_lockstep.cpp`: bounded desync
  replay capture; `.sdrp` artifacts and a LAN verdict exist in its logs directory.
  Recorded Splonks validation is useful reference evidence, not Teeming validation.

Implement a Teeming transport adapter using direct UDP, authenticated punch and
relay fallback without coupling the service to Teeming simulation. Use the same
socket for endpoint discovery and gameplay. Preserve distinct relay peer routes,
packet-size limits, cancellation, retry and reconnect state.

One publicly reachable service deployment can provide room discovery, punch
coordination and opaque packet relay. A small Linux VPS is a suitable starting
deployment; it does not need to render or simulate the game. Determine whether
the existing Splonks service can be reused, with game/version separation, before
creating another server. Do not assume an old deployment is still running.
Punching alone is insufficient for restrictive NAT; include relay fallback.

Teeming GView flow: Host Room -> named party with code -> friends browse or enter
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
