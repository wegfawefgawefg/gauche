# Player-count-independent multiplayer

Baseline: `pre-player-count-agnostic-networking` (`4215402`).

Participants use ordered maps keyed by stable 32-bit PlayerIds. IDs are allocated
monotonically by the host, never compacted or reused during a session, and are
separate from both connection identity and generation-qualified entity handles.
A participant owns run currency/reward state and a `controlled` entity reference.
`bind_player_control` validates that reference and prevents two players owning the
same body. This is groundwork for transformations, not a possession mechanic.

Game simulation, input frames, hashing, snapshots, reward/shop barriers, targeting,
and spawning enumerate actual participants. A disconnected participant and body
leave the world; the host retains reconnect state outside simulation/rollback.
Rejoining the same connection identity restores the same PlayerId. Restart keeps
those IDs but clears the previous run's character state. Spring attribution is
stored sparsely rather than packing player numbers into sprite bits.

The host admission limit defaults to 16 and can be changed with `--max-players N`.
It controls admission, not storage or packet layout. The simulation still has its
existing 512-entity world budget; this work does not promise unlimited actors or
bandwidth. Increasing admission does not allocate empty participant slots.

Input frames encode a count and explicit ordered IDs. The ready list is variable
length. Canonical history fits the datagram budget where possible; larger messages
use checksummed fragmentation with duplicate/reorder handling, bounded allocation,
and expiry. Every game datagram stays within the relay's 1200-byte payload budget.
Existing history/correction retry and snapshot recovery handle loss. Host fan-out
still grows with the number of clients; this is not a network topology redesign.

Wire version 16 / snapshot version 68 / recovery-capture version 2 require matching
builds. Old snapshots are rejected explicitly. The room server does not interpret
these payloads and needs no upgrade. The party HUD pages groups of four; the debug
player-status window has a scrolling full roster.

## Validation

- Local UDP: 4, 8, 16, 24 players; sparse IDs above 1000; matching hashes, late-input
  repair, readiness, admission rejection, departure/rejoin, reward gates, safe
  spawning and revival. Entity-budget rejection leaves no phantom participant.
- 128-input message: fragmentation, out-of-order and duplicate fragments, withheld
  fragment, malicious count, plus sparse-ID control binding/snapshot roundtrip.
- Actual VPS: 16 participants using forced relay; joins and six synchronized ticks.
- Existing rollback and four-player lossy-network regressions pass.
- Four-player bounded benchmark (5000 ticks, five runs, empty 48x24 fixture,
  16-frame history, simulation + state copies + hashing): baseline median 58.735
  microseconds/tick, refactor 58.574 microseconds/tick.
  This is a local CPU comparison, not a guarantee about 16-player internet play.
- Two older broad fixtures still fail identically on the baseline: the game-rules
  test expects immediate pickaxe damage; the codec fixture supplies an invalid
  player inventory. They are separate from the participant-network checks.
