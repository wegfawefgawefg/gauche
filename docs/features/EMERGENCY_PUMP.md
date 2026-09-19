# Emergency Pump — 2026-09-15

Industry now has seventeen of twenty catalog enemy behaviors. The Emergency
Pump is a 100-HP mobile machine with a finite three-pour tank. It starts with
water in incidental encounters; cooling works choose water or oil with saved
RNG. The cooling-works pump replaces the rat beside the finite-fuel boiler,
raising that encounter's cost from two to three threat. Its safe margins,
protected center, lava trench, coolant spill and player supply remain intact.

## Behavior and interactions

A loaded pump approaches visible players or decoys. Within a cardinal three-cell
lane it commits its origin and nozzle direction for a 36-tick warning. The jet
then pours up to 600 retained surface ticks into each of three successive cells.
It recovers for 72 ticks. A full portion deals six pressure damage and one safe
push; partial quantities scale damage down, and less than 300 does not push.
Victims are captured before effects and processed farthest first, once each.
Shields block pressure. Root/grip, anchored actors, airborne actors and sled
riders cannot be pushed; blocked landings never invoke wall-crush damage.

Walls, closed doors and solid cover stop the liquid. Grates pass it. The near
face of an anchored machine can receive liquid, but it still blocks cells behind
it. This lets coolant reduce boiler pressure or interrupt a crane/hoist.

When empty, the pump searches within five cells for an accessible finite spill.
It takes one second to draw from its own or an adjacent cell. Source legality is
checked again when the refill completes: moving a blocker into place, removing
or burning the spill, or interrupting the pump leaves no invented liquid. Water,
oil, sap, honey, rot, brine and coolant retain their shared surface behavior.
Terrain lakes, lava, frozen spills and burning fuel cannot be drained. Quantity
transfers are exact, including partial fills and capped destination puddles.
Floor evaporation continues normally; only sealed storage preserves the liquid.

Heavy health hits of ten or more, cold/control/root effects, and displacement
cancel a fill or warned jet into recovery without consuming the tank. Small
hits do not reset its warning. Fired jets remain committed even if their original
target moved. A pump cannot refill itself from an infinite decorative water tile.

One saved-RNG loot roll gives 25% Pocket Pump, retaining the machine's exact
remaining liquid/quantity, a reserved empty 15% Nozzle Elbow slot, and then 15%
Coolant Can. The unimplemented elbow is not replaced with unrelated loot.

## State and presentation

Uses shared Entity phase/timer/counter/point fields; no new per-entity fields or
wire payload. Header comments document slots. Snapshot validation rejects bad
phases, amounts, liquids, directions and phase clocks. Gameplay compatibility
version is `0x2026091559`.

A fluid-colored tank stripe, vibrating pressure buildup, directional nozzle and
brief visible jet communicate its state without enabling debug attack patterns.
Its metal casing leaves local chain-link debris. Four original offline OGGs
cover suction, pressure buildup, the jet and breakage; source script is
`tools/sound/emergency_pump.py`, peaks 0.27–0.36. No music changes.

Original transparent sprite: `assets/graphics/emergency_pump.png`, built-in image
generation. Source retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-4f96d45d-f39c-4a2e-8fc3-7ae14abc98a1.png`.
Prompt: isolated squat gray-green wheeled industrial pump, brass motor and thick
right-facing hose/nozzle, ivory pressure dial, muted sage/charcoal/rust/brass,
sparse top-down pixel clusters, transparent background, no floor/text/shadow.

## Verification

Strict release build passed. Temporary direct checks covered the full warning,
fixed aim, single-hit push, blocked landing, exact partial transfer, interrupted
refill/attack, cover/grates/doors, oil ignition, water quenching/lava conversion,
coolant/boiler contact, retained loot, snapshot continuation and invalid-state
rejection. A full cosmetic sound buffer did not change the simulation hash.
Sixty-four generated Industry floors preserved reachable routes and required
locks; 25 pumps were placed. Four sound assets loaded, and an SDL dummy static
render at `/tmp/teeming-emergency-pump.png` was inspected.

The first direct check caught doors receiving liquid because their fixture HP is
one; closed door/gate exclusion was corrected before the passing run. No
interactive playthrough or permanent test suite. Balance awaits player feedback.
