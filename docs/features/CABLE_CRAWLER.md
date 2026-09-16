# Cable Crawler and cable trenches

Implemented September 15, 2026. Industry now has ten implemented enemy behaviors
and twelve regional items. This slice gives the existing wire, grounding and
metal-cutting tools another encounter to work in; it introduces no feed item.

## Creature and circuit

A 50-HP, one-cell maintenance beetle crawls on a 20-tick movement beat. Its four
candidate cardinal steps favor walls and forward motion, with target distance
strong enough to leave the wall. It checks alternate exits when blocked, avoids
lava and never moves its image independently of its actual cell.

It lays ordinary, cuttable Copper Wire under itself on vacant floor: eight cells
maximum over its lifetime. Existing wire costs nothing. It does not overwrite
other props or repair broken wire. Wires persist after death and do not become
free recovered loot. Spawned cable-trench wiring is separate authored scenery.

A cardinally adjacent target or one connected within six wire/water steps starts
a 48-tick charge. Braced legs, cyan contacts, a small blue light and accelerating
relay clicks distinguish the tell. The origin and electrode contact are fixed.
On firing, shared conduction retraces the current circuit. Dry retreat, wire
cutting and newly placed grounding all work during the tell. Contact damage is
14, declining with circuit distance (8 at three steps). A ready grounding spike
sinks the whole pulse and becomes spent. A nearby dry target receives a contact
pulse; the enemy still works when it runs out of wire or leaves its trench.

Its own electrode does not shock its chassis. Other crawlers, allies and players
can be hit by its circuit; external electricity can damage it normally. Flying
actors follow the existing distinction between direct contact and carried floor
shock. A straw decoy can attract a contact pulse and takes damage normally.

It recovers for 90 ticks after firing/interruption. Cold, rooted state,
displacement, sleep/stun/toss and health damage of at least eight interrupt the
charge. Body movement never follows a stale visual target. The recovery is set
before applying damage so reactions cannot execute the same pulse twice.

Shared slots: `label_a` phase; `timer_a` charge/recovery; `counter_a` unused wire
cells; `timer_b` claw-sound throttle; `point_a` committed origin; `point_b`
electrode contact. No species-specific entity allocation. Network reader checks
phase/timer/reserve/contact bounds. Gameplay compatibility is `0x2026091544`;
snapshot 50 and wire protocol 14 remain unchanged.

One loot roll: 25% Copper Wire, next 15% [Insulated Boots](INSULATED_BOOTS.md),
otherwise empty. The boots slice also gives cable trenches a 50% alternative
to their grounding-spike supply.
Destroyed crawlers leave cosmetic copper curls and washers.

## Cable-trench scene

The optional maintenance room chooses equally among repair bay, cooling works
and cable trench when that room is selected. The trench puts a crawler at the end
of five wire cells, a shallow-water branch below the wire and a diggable rock
shelf above. A scrap bin and loose grounding spike sit on the opposite side.
Dry paths around the ends and the room's central cross remain available.

The whole footprint is checked before mutation, tries mirrored placement, and
rejects reserved/occupied/lava cells. It costs two threat and one equipment
budget; a failed footprint falls back to an ordinary crawler. Incidental
Industry encounters also include crawlers. No objective depends on a surviving
random item or an intact circuit.

## Assets and checks

Two original transparent sprite assets, copied unchanged from:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:

- `exec-dd79808e-7bcf-4804-9343-430d6c2310fb.png` → `cable_crawler.png`
- `exec-8821aa9d-d666-4ce8-84a2-876000ae5dcc.png` → `crawler_charge.png`

Four original offline sounds come from `tools/sound/cable_crawler.py`: charging
relay, pulse, claw movement and death. No new runtime synthesizer or music.

Strict release build passed. Temporary direct checks verified no pre-tell damage,
retreat from the committed contact, circuit-distance damage, last-second wire
cutting, one-shot grounding, cold/damage/displacement/sleep interruption, the
eight-wire lifetime limit, snapshot roundtrip and malformed reserve rejection.
Reserved-footprint failure leaves the game hash unchanged. Across 64 generated
Industry floors, all remained reachable and properly locked; 38 crawlers and
16 complete five-wire trenches were found. SDL dummy loading covered the four
sound assets. Static scene/charged-pose capture: `/tmp/gauche-cable-trench.png`.
No interactive playtest or permanent test suite was added; tuning awaits feedback.
