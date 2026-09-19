# Chain Hook — cargo and anchored movement

Implemented 2026-09-15 as useful Industry equipment following the feedback that
Ice/Industry need purposes for their tools, not only feed and narrow cures.
Music remains parked after the bounded third sketch pass.

The five-cell physical hook catches loose crates, pots, scrap/ore bins, fishing
creels and lunch tins. Hold use to haul one cell every ten ticks; release pauses,
secondary cuts. Walls and fixed blocking structures instead pull the user.
The line lasts at most four seconds and costs one of 24 casts at launch. Long
extends normal pattern reach, Durable doubles casts. It deals no direct damage.
Ground hazards still matter when the user is pulled. It does not cross gaps or
replace the Harpoon Gun's hostile-actor tether.

Actual props are relocated with their health, cover and variant intact; contents
are rolled by the existing container destruction path only when broken. Loose
gear keeps its generation and complete item state. Sleds haul the actual rider
and actual item together before applying landing effects. Blocked destinations
cut without crushing or deleting cargo. Ordinary actors block a thrown hook;
a sled's own rider does not prevent catching the sled. Stale handles, changed
prop signatures, independently moved cargo, stowing, incapacitation and blocked
lines release their reservations. Picking up/dropping cuts before that action.
The final cast keeps its zero-use inventory reservation until the line ends.

Native stage-one master supply entry: uncommon weight 3, price 22, one item.
Scrap bins have a 20% Chain Hook range, replacing an empty range. Scrap yards
place one beside the movable bins, with an extra entity-capacity check and an
extended protected-footprint check before spawning anything. Cranes recognize
the steel hook through the shared magnetic-material list.

State uses ordinary projectile slots, documented at the implementation, and the
existing item-flight reservation. Snapshot validation covers hook state and
owner reservations; gameplay version is `0x2026091553`. No new snapshot fields.
Domain files: `items/chain_hook.*`, `projectiles/chain_hook.*`; sled transport
stays in `items/sled.cpp`.

## Presentation

Original transparent generated icon at `assets/graphics/chain_hook.png`;
source retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-2b2be5ec-f991-4f7c-a90b-e62afbc85c94.png`.
Prompt requested one muted steel J hook and three or four chunky chain links,
transparent alpha, crisp pixel clusters, no scenery, text or texture noise.
Flight uses the existing hook-head sprite; the tether is a segmented gray chain.
Cast/latch/haul/release/spent sounds are short original inharmonic iron rings
and filtered impacts from `tools/sound/chain_hook.py`, peaks 0.14–0.23.
Casting, catching and hauling use the shared world-hearing path.

## Checks and limits

Strict release build passed. Temporary direct checks (not a committed suite)
covered prop condition and RNG preservation, cargo identity, pause/resume/cut,
wall pulls, obstruction without crushing, stowing/pickup cancellation, last-cast
reservation, saved mid-haul continuation, invalid phase rejection, loaded/ridden
sled transport, full entity-pool refusal and protected-footprint rejection.
64 generated Industry floors retained reachable routes and required locks and
contained 34 hooks. SDL dummy drivers decoded the five OGGs and rendered the
live tether to `/tmp/teeming-chain-hook.png`; that static render was inspected.
No autonomous playthrough. Cast count, haul pacing and acquisition balance still
need human play feedback. Powered carts beyond the existing sled remain future
content; this does not claim to implement the planned transport network.
