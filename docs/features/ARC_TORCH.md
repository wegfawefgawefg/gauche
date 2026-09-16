# Arc Torch — 2026-09-15

Industry gains a ninth regional item: a sustained close-range weapon and metal
cutter. It draws on existing electrical terrain rules, giving a combat tool
uses around water, wire, grounding and the grates already in Industrial rooms.
The broader request for stronger native choices and richer encounters remains
open; this does not complete the biome.

## Behavior

Hold Use for 0.3s to prime. Each subsequent pulse deals seven electrical damage
at the adjacent aimed cell, every 0.25s. Aim/movement remain live between pulses.
Release stops immediately; using again requires a new prime. Switching items,
pickup/drop, secondary action, stun or cancellation discards the primed state.
An interrupted prime costs no battery. An arc into empty air or a wall does.

The normal battery has 32 pulses, approximately eight seconds of continuous
output after priming. Exhaustion consumes the tool with an electrical sputter;
Ammo does not refill it. Partial batteries survive dropping and trading through
ordinary item uses. No new ammunition item was introduced. The detail panel
shows both prime time and use count.

Water and copper wire conduct reduced damage up to three connected cells from
the contact, including toward the user. Dry/ice interruptions stop a chain;
Glass Eels retain their established immunity. A ready grounding spike sinks one
whole pulse, then becomes spent; it does not grant indefinite protection against
a held tool. The same shared conducted-shock trace decides victims and displayed
arcs. It is not an instant long-range beam or an area attack through walls.

Metal grates, scrap bins and ore bins take twice the pulse damage. A normal
60-HP grate needs five pulses. Trace happens before cutting: an arc cannot pass
through bars destroyed by that same pulse. Walls and objective seals are not
cuttable with it. Nonmetal props are not silently given a new welding rule.

Strong/Fragile/Heavy modify damage; Agile/Heavy modify pulse cooldown; Durable
has 64 pulses and Fragile 16. Big extends conduction by one connected cell.
Long, Piercing and Restorative are rejected. Priming remains 0.3s for all variants.

## Acquisition and presentation

The master supply row is Industrial, weight 3, stage 2, price 38. Available in
native combat stock, rewards, shops, caches, workshops and secrets. Existing
salvage/secret rules may preview it one stage earlier. Foreign-biome imports use
the existing exceptionally rare import roll; there is no common Forest override.
The later [Arc Welder and repair-bay milestone](ARC_WELDER.md) adds a native enemy drop and a setting for these interactions.

New offline cues cover transformer prime, normal pulse, metal cutting and spent
battery. Prime and pulse/cutting sounds also register existing deterministic AI
hearing. Existing short electric ribbons, sparks and local light flashes render
the discharge. The new item icon uses the ordinary inventory/held-item renderer.

Player action state uses existing shared slots: `label_b` 19–24 identifies the
committed inventory slot, `counter_a` holds prime progress, `counter_b` remains
the cancellation latch, and `ground_item` records the committed variant. The
snapshot decoder validates it. Gameplay version `0x2026091537`; snapshot layout
50 and wire 14 are unchanged. Co-op participants need matching builds.

## Validation

Strict Release build passed. A temporary direct-function check covered no damage
before prime, cadence, release/reprime, exhaustion, metal cutting, conducted and
self damage, grounding consumption, variants, biome membership, slot changes,
and snapshot/hash replay mid-prime. No live playthrough or permanent new test
suite. A static SDL-dummy discharge capture was inspected: the contact arc and weaker
water branches follow the affected cells. This is not a balance assessment.

Audio source: `tools/sound/arc_torch.py`. Bitmap path:
`assets/graphics/arc_torch.png`, made with the built-in imagegen tool. The selected version removes the generated glow/halo from earlier candidates;
alpha is preserved. Final built-in generation prompt:

> A hard-edged pixel art inventory icon, isolated with true transparent background. One small L-shaped handheld POWER TOOL pointing right. Solid flat grey grip under muted TEAL rectangular housing. Tiny COPPER bent nozzle points right. NO SPARKS. NO LIGHT. NO ILLUMINATION. Flat matte plastic and metal. Draw as 16x16 logical chunky pixels, only four flat opaque ink colours: teal, charcoal, grey, copper. Large simple block shapes with no internal surface texture. Object centered in a square frame with 10% transparent padding. An extremely plain old pixel videogame sprite, not concept illustration. Do not draw any glow, smoke, bloom, gradient, shadow, rimlight, bright spot, background, checkerboard or lettering. All pixels outside the geometric object must be fully transparent, no halo.
