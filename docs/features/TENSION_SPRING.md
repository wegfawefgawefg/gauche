# Tension Spring

Implemented September 15, 2026. Fourteen of fifty Industrial regional items now
work in the game; enemy count remains eleven. This addition addresses useful
toys with actual applications: cross a salvage gap, escape on an aimed launch,
or step away and leave the spring to throw a pursuer against a wall.

## Use and integration

Use places the spring underfoot, facing the current aim. It arms in 18 ticks
(0.3s), with a ratchet and latch cue. Stay on it to launch when ready; step away
to leave it armed. The first susceptible grounded actor gets the existing
24-tick, four-cell toss. This deliberately replaces the initial catalog's
three-cell shove: four cells spans the existing three-cell salvage gaps, and
actual airborne movement skips their lava/water/ground traps. Positions remain
authoritative tile positions; the existing height arc and shadow convey flight.

A new stack contains three springs. One successful placement consumes one;
failed placement consumes nothing. The 36-tick use cooldown prevents repeatedly
laying pads before the first arms. No modifiers are offered. Sprung or smashed
pads become cosmetic metal scraps, never additional recoverable items.

Walls/solid props stop flight with the shared twelve-damage, thirty-tick stun.
Actors also obstruct the lane. Landing applies ordinary ground contact; landing
in deep water remains fatal, and landing on lava still exposes the actor to it.
Flying, rooted/gripping actors, sled riders and anchored fixtures cannot launch;
refusal leaves the spring intact. Carts are not implemented and are not claimed
as supported here. Existing Woodland Spring Traps retain their two-step shove.

Placement requires walkable, non-burning standing terrain with no active prop
or anchored fixture. Lava and rails refuse; it cannot be placed while airborne.
Ordinary ground loot can share the cell. This underfoot placement also permits
returning from a small salvage plinth using the next spring in the stack.

The live mechanism is a six-HP, non-blocking compact prop. Ordinary strikes and
metal cutting break it. Exposed fire or residual heat removes two HP per half
second. The coil is metal: water alone does not break it, and it does not gain
magical fire immunity from coming from Industry. Placement and launch have
separate sound/visual events; fragments remain local only.

Native stage-one Industrial reward, shop, cache, secret and workshop tables offer
three springs for a starting price of fifteen. The folded item is magnetic.
One-quarter of suitable hot salvage banks now offer a three-spring bundle; other
banks retain their existing chance of a magnet for a steel reward. Ice salvage
still offers Fishing Line. Main routes do not depend on finding a spring.

## Storage and presentation

No new Entity subclass or actor slot: the existing eight-byte Prop holds it.
`variant` low two bits store direction (right/down/left/up); upper bits store
player owner plus one, or zero for a neutral spring. `growth_ticks` holds the
18-tick arming delay. Damage attribution resolves that player seat to its current
generation-safe actor handle at launch, then the shared toss owns the flight.
Entry triggers immediately; armed springs check stationary occupants at a
staggered 10 Hz so releasing grip can also launch without full-rate actor scans.

Snapshot validation bounds direction/owner, arming time and HP. Existing prop
and toss serialization/hash fields suffice; compatibility is bumped to
`0x2026091547`. Actor ground contact returns as soon as a spring launches, so
later floor/trap contacts do not run during the new flight.

The directional arrow is part of the sprite; placed variants rotate by ninety
degrees. The built-in image tool generated `assets/graphics/tension_spring.png`,
copied unchanged with alpha from
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-c360a35f-e164-4411-9a6c-4d60cd636c30.png`.
Prompt: one minimalist top-down roguelike sprite, compressed iron coil between
square steel plates, muted ochre arrow pointing right, charcoal/steel palette,
large pixel blocks, transparent background, no floor/shadow/text.

`tools/sound/tension_spring.py` produces `tension_set`, `tension_ready`,
`tension_launch` and `tension_break`: ratchet, latch, descending spring resonance
and snapped coil. Peaks 0.18–0.32, durations 0.15–0.65s. Existing toss landing and
wall-impact effects remain shared. No music work in this slice.

## Checks

Strict release build passed. Temporary direct-function checks verified exact
arming delay, stack consumption, snapshot equality before and during flight,
out-and-back crossing over three lava cells, wall collision, pursuer launch,
flyer/grip refusal, midair ground-hazard skipping, real landing hazards, lethal
water landing, failed placement, heat damage and malformed snapshot rejection.
Supply and modifier rules passed. Nine of 32 authored salvage placements offered
spring bundles; 32 generated Industry floors retained reachability and required
exit locks. SDL dummy loading covered the sprite and four sound assets; inspected
static capture: `/tmp/teeming-tension-spring.png`. No interactive playtest or new
permanent test suite; balance and feel await user feedback.
