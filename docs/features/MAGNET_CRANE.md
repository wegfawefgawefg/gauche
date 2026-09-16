# Magnet Crane and scrap yard — 2026-09-15

Implemented Industrial catalog enemy 7 and an optional scrap-yard composition.
Industrial now has eight catalog enemies and ten regional items. This adds a
working setting for the magnet and for carrying useful steel tools into danger.

## Behavior and counterplay

A 90-HP anchored pedestal searches within five Manhattan cells. It prioritizes
held/dropped Horseshoe Magnets, then players carrying substantial steel gear
and steel-equipped Pickhands, Rivet Gunners or Strikebreakers, then loose steel
items. It uses the shared `magnetic_item` property. Nonmetal equipment, food and
Copper Wire do not qualify. Equal priorities prefer nearest range, then stable
entity order. No separate allegiance exception protects the workers.

The head travels for 30 ticks (0.5s), then stays over its committed target cell
for 45 ticks (0.75s), with an amber mark and rising coil sound. Only the original
generation-checked target can be grabbed. Moving away, dropping/switching from
steel, new cover or target removal makes it miss. It never follows you silently
during the warning. Afterward it retracts for 60 ticks (1s).

A successful grab pulls one real cell toward the base. Loose loot retains its
whole Item state and cannot merge into another loose object or pass cover.
Floating loot deflates; strapped sled cargo and active-flight items are excluded.
Actors keep their inventory and take six damage. Their equipment is not detached.
Grip, rooting and riding a sled resist the pull. A wall, blocking prop, actor or
the pedestal itself stops actor movement; this gives only the six-damage pinch,
not the shared shove helper's instant hard-wall crush. Successful movement uses
normal entry/hazard rules, so a hot sorting lane is dangerous.

A hit of at least twelve actual damage cancels the head cycle. Cold stalls the
motor, including though it is anchored and therefore immune to ordinary walking
slowdown; the shared chill entry point handles this explicitly. Sleep/stun/toss
state, if applied, also cancels an active cycle through the common timer path.
The stationary base is the damageable entity. The head/boom are presentation;
there is no separate cuttable chain hitbox. This preserves the one-cell actor
model rather than adding a second damageable body for this enemy.

One death roll: 25% Horseshoe Magnet, next 20% Copper Wire, otherwise nothing.
Local steel-washer debris accompanies the collapse. Bait/other carried loot is
not copied into that roll.

## Scrap yard

Up to one eligible non-objective room is reserved with a half selection chance,
without replacing workfronts, blasting alcoves, assembly rooms or repair bays.
The open clearing has a pedestal, a real pickaxe worn to six condition, two hot
sorting cells, a magnet beyond the initial crane radius, and scrap/ore bins.
Dry perimeter routes remain available. The crane can start sorting the loose
pickaxe before the party reaches the room.

Every footprint cell is checked for room bounds, protected routes, existing
occupancy and spawn clearance before mutation; three free entity slots are
required. The two supplied tools count against the ordinary equipment budget.
If the authored layout cannot fit, an ordinary crane uses the same two-point
threat budget. The planned Scrap Effigy item is not implemented yet.

A later generated-scene audit found that the original footprint overlapped the
protected central cross, so generation used the lone-crane fallback. The static
fixture capture had not exposed this. [Coolant/cooling-works follow-up](COOLANT_CAN.md)
moves the whole yard into legal side cells and verifies 34 complete sorting
yards across 64 generated Industrial floors.

## State, visuals and audio

`label_a`/`timer_a` hold phase/timing, `entity_a` the checked marked target,
`point_a` the anchored origin, `point_b` the committed contact, and `counter_a`
the retract-start fraction in thirtieths. That fraction preserves the head's
visual position if travel is interrupted. Only the decorative boom/head slides;
actor positions and contact resolution stay at real cells. The base cannot be
shoved. The snapshot decoder validates phases, timers and reach. Gameplay version
`0x2026091540`; snapshot layout 50 and wire 14 remain unchanged.

Two generated sprites depict the pedestal and lifting disc. Code draws a simple
boom, suspension line, head shadow and warning mark. Head rendering happens above
actors; its shadow/mark underneath. Four offline sounds in
`tools/sound/magnet_crane.py` cover motor slew, coil rise, tug and collapse, and
register ordinary hearing. Cosmetic trails reuse the magnet movement event.

## Validation

Strict Release build passed. Temporary direct-function checks covered complete
travel/warning/retract timing, fixed-cell misses, switching away from steel,
cover, safe blocked-pinching, grip, heavy-hit/cold interruption, real loot state,
magnet priority, worker pulls, stale handles, and snapshot/hash continuation.
Protected footprint rejection left the world unchanged. Sixty-four generated
Industrial floors retained reachable objectives and required exit locks and
contained 32 cranes. Static SDL-dummy warning and scrap-yard captures were
inspected. No live playthrough or permanent test suite; balance awaits feedback.

## Bitmap provenance

Built-in imagegen, alpha preserved. Pedestal source
`exec-f47550dc-ce7d-4876-990d-c28ad76723d9.png` copied to
`assets/graphics/magnet_crane.png`:

> One isolated minimalist chunky pixel art game enemy BASE: an anchored industrial magnet crane pedestal viewed from above at a slight angle for a top-down roguelike. A squat square charcoal steel mounting plinth with four broad feet and a short vertical muted ochre mast at the back, small teal motor box on one side. No overhead arm, no chain, no magnet head, game draws those separately. 16x16 logical pixels, broad flat simple shapes, four flat opaque colours (charcoal, grey, ochre, muted teal), no texture or shading gradients. Occupies 85 percent of a square transparent canvas. Large clear silhouette at tiny size. True transparent alpha; no background, glow, shadows, text or scenery.

Lifting disc source `exec-f2a4da17-6530-451c-8da6-bee238a5f43d.png` copied to
`assets/graphics/crane_head.png`:

> One small pixel-art electromagnet lifting HEAD for a top-down industrial crane in a minimalist roguelike. A broad squat circular steel lifting disc viewed obliquely from above: flat muted teal top plate, chunky dark grey lower rim, two pale grey pole blocks on its underside, one tiny ochre attachment eye on top. No cable or arm, game draws that separately. Logical 16x16 chunky pixel blocks, only four flat opaque colours, hard stepped silhouette with transparent background. Fill 90 percent of square width and 65 percent height. No texture, glow, shadows, floor, text or extra objects. True transparent alpha.
