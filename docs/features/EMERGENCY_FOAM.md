# Emergency Foam

Implemented September 15, 2026. Industry now has twelve catalog enemy behaviors
and fifteen regional items. This adds a useful temporary defensive tool against
fire and firing lanes, rather than another single-condition remedy or feed item.

## Use and interaction

Two cans stack together. Throwing spends one can, with a 45-tick cooldown; travel
is one cell per eight ticks, up to four cells. Solid cover lands the can on its
near side; it passes above low cover while airborne. After landing, a sealed
sixty-tick expansion begins, with a hissing sound and a visibly growing mound.
Water cannot cancel this internal reaction. Neither flight nor expansion creates
instant damage or unannounced solid cover.

The burst quenches its cell and four cardinal neighbors through existing shared
rules. This douses exposed fire, burning actors, temporary lit sticks, campfires,
candles, stoves and exposed fuses; it also invokes the existing Stoker, Pressure
Rat and Walking Kiln quench responses. It does not delete a machine's fuel or
convert lava to safe ground. Burning wooden wall faces can be doused; walls and
sealed doors prevent reaching a far side. If a door or solid prop closes over
the can during its delay, expansion cannot spray outward through that enclosure.

Empty solid floor in the burst receives a 12-HP mound for 600 ticks. Existing
props, actors, loose loot and fixtures are preserved; they can be quenched but
are not buried under cover. Foam can sit on ordinary ground, ruins, snow, ice
and existing bridges. It never bridges water/holes, covers lava or changes terrain.
Another can does not refresh or replace a live mound.

Mounds block walkers, narrow projectiles and broad heat, while thrown objects can
pass overhead using the existing low-cover rule. Bullets/melee tear their actual
HP; the shot that destroys a mound still hits that cover. Nearby flame, lava or
residual warmth costs two HP per half-second, staggered across cells. Suppression
prevents the covered cell from igniting while the mound lasts; fuel underneath
remains available to burn again afterward. Foam shrinks visibly in its last two
seconds and collapses on expiry. It produces no wooden fragments or loot.

This pass also excludes Pay Cages and Tension Springs from the ordinary dry-growth
ignition list. Metal alone no longer becomes fire fuel; burning oil or cloth can
still heat those objects. Springs retain their existing heat-damage behavior.

## Supply and state

The master supply table adds one Industry-native row: weight three, stage one,
bundle two, reward/shop/cache/secret/workshop sources, starting price eighteen.
No item modifiers are advertised. The folded metal can is magnetic. Kiln courts
now offer either the existing Coolant Can or two foam cans, equally weighted,
without changing the protected footprint or equipment budget. Broader container
variants, including the catalog's Coolant Locker, remain unimplemented.

The projectile uses shared fields: `label_a` kind, `label_b` flying/landed,
`counter_a` remaining cells, `attack_interval` original range, `timer_b` travel
beat, `timer_a` landed expansion, `point_a` launch, `entity_a` owner handle and
`ground_item` exact one-can payload. Its physical update runs in the timer phase.
Mounds use existing compact props with `growth_ticks` as remaining life. No new
per-entity payload or snapshot field was added. Validation checks payload, phase,
range, clocks, facing and prop lifetime/HP/variant. Gameplay version is
`0x2026091549`; snapshot format remains 50. Rendering and sound remain local.

## Art, sound and validation

Built-in image generation produced these transparent originals, inspected and
copied unchanged to the project:

- `assets/graphics/emergency_foam.png`: `exec-66c7bc58-7813-448e-bbae-d52bd462e707.png`
- `assets/graphics/foam_cover.png`: `exec-9d446231-e389-4958-8e5f-7d0e64a967cf.png`

Original directory:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.
Can prompt: single minimalist top-down 16-logical-pixel item, squat rusty-red
pressurized can, cream band, small black capped nozzle and handle, chunky flat
muted palette, centered, transparent alpha, no text, floor, shadow or scene.
Foam prompt: one low dense mound with four or five joined bubbly lobes, muted
sage-grey cream with darker green-grey base, three or four flat tones, sparse
chunky pixels, transparent alpha, no wispy smoke, snow, glitter or tiny bubbles.

Six original offline cues are reproducible using `tools/sound/emergency_foam.py`:
`foam_throw`, `foam_land`, `foam_expand`, `foam_douse`, `foam_tear`, `foam_collapse`.
They combine pressurized noise, wet low resonances and soft tearing; durations
0.20–1.00s, peaks 0.17–0.30. No music was added or replaced.

Strict release build passed. Temporary direct checks covered real stack consumption,
flight and landed timing, saved-state roundtrips/malformed rejection, actual
projectile cover damage, heat/expiry, fire/fuse/actor/prop quenching, occupied
cells, water/lava refusal, near-side landing, closed-over-can containment, sealed
reaction and native supply membership. Sixty-four generated Industry floors
retained reachable routes and necessary exit locks; four foam bundles appeared.
SDL dummy loaded both sprites and all six sounds; the inspected static capture
is `/tmp/gauche-emergency-foam.png`. No interactive playtest or permanent test
suite was added. Balance and combat feel still await the user's playtesting.
