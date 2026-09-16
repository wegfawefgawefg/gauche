# Furnace Moth and lamp alcove

Implemented September 15, 2026. Industry now has thirteen catalog enemy behaviors
and fifteen regional items. The moth links light placement, exposed finite fuel,
cold tools and shot-blocking cover, without adding another dedicated feed item.

## Behavior

An 18-HP flying moth searches within eight cells every twenty-four ticks. Visible
entity/item/prop emitters and surface flames compete by intensity, distance and
directional-light shape. Other Furnace Moths are excluded so their charged wings
cannot form a self-attracting cluster. It approaches and circles the selected
source in actual cardinal steps. Dropped lights, cold electric lamps, held lamps
and temporary burning sticks are all useful lures. Extinguished sources stop
competing on the next search; an already committed dive keeps its lane.

An empty moth next to exposed finite fuel pauses thirty ticks, then extracts up
to 300 actual heat using the same operation as Heat Siphon. Candles, stoves,
surface flame, campfires, burning bodies, flares and temporary lit sticks can
supply it. Partial sources give exactly their remaining amount. The source cell
and any entity handle are rechecked; moving/stale sources and interruptions
cannot transfer remote heat. Permanent torches and cold lamps attract it but do
not provide free fuel. Incidental moths start with 180 heat for one modest attack;
the feeder in the authored alcove starts empty.

A charged moth aligned with its chosen light within five cells folds its wings
and warns for forty-five ticks. It then commits all stored heat to a cardinal
dive, moving one cell per six ticks. It cannot track a sidestep after warning.
The first body or solid obstruction stops it; otherwise it flares at the original
range endpoint. Damage is `4 + heat / 30` (ten from its initial reserve, fourteen
at full heat), with up to 180 ticks of ordinary ignition after real health damage.
Wet targets retain their existing ignition resistance. An impact may ignite
ordinary fuel or a wooden wall face. Existing solid cover protects any body on
its cell even when that cover breaks from the hit.

The spent moth rests ninety ticks and needs more fuel before another attack.
Any real health damage, sleep/stun, rooting or unexpected displacement interrupts
feeding/warning/dive. Unspent stored heat survives ordinary interruption; spent
dive heat is lost even on a miss. Cold or a direct quenching splash empties it
and imposes a 120-tick rest, held while chilled. The moth ignores ground slipping,
ground-water contact and toss/spring launching through the shared flying rule.
Burning status cannot sustain damage on it; physical hot impacts still can.

Two sprite poses distinguish cold soot wings from charged ember markings. The
warning folds the wings and brightens its small emitter; a rising papery cue
precedes the dive. Debug attack previews show the remaining committed lane only.
The implementation does not add multi-cell hitboxes or render-position sliding.

## Scene, drops and saved state

Lamp Alcove is an equal sixth alternative in the existing optional maintenance
selection, alongside Repair Bay, Cooling Works, Cable Trench, Kiln Court and Pay
Office. The dry offset station contains two moths, a stove with 1800 fuel ticks,
a cold electric Beam Lamp and two Emergency Foam cans. One moth begins empty
beside the stove; the other starts charged beside the lamp. Players can douse or
break lights, empty the stove, draw the moths toward another source or deploy
foam cover. The whole footprint and three entity slots are checked before any
mutation; mirrored placement preserves the central protected route. Budget:
two threat, one equipment. Safe-entry and water-tap room variants remain pending.

Incidental Industry encounters can also place moths at cost one. Their single
drop roll gives a Heat Capsule on 25–34 (10%); the first 25% is reserved for the
unimplemented Glow Slag item and stays empty. No new regional item count is
claimed for the existing capsule.

Shared slots: `counter_a` stored heat, `counter_b` orbit index or dive distance,
`label_a` phase, `label_b` committed heat, `timer_a` phase time, `timer_b` light
search, `point_a` light/fuel cell, `entity_a` source handle and `point_b` committed
origin/expected dive cell. Facing commits the axis. Validation bounds heat,
phases, distance and clocks. Gameplay compatibility is `0x2026091550`; snapshot
format remains 50. Sounds and wing animation stay local.

The checks exposed a shared Heat Siphon bug: ground items conventionally have
zero health, and heat extraction incorrectly skipped them with dead actors.
Dropped temporary lit sticks now transfer their actual remaining flame through
the same operation. Permanent torches remain ineligible.

## Assets and verification

Built-in image generation produced these inspected transparent originals, copied
unchanged into the game:

- `assets/graphics/furnace_moth.png`: `exec-bf4da9d7-1e4a-4e32-bd6d-28e41e1470ce.png`
- `assets/graphics/furnace_moth_hot.png`: `exec-12b97dbe-6bc0-4ab1-8af5-f4c49f80e23f.png`

Original directory:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.
Base prompt: single minimalist top-down 16-logical-pixel ash moth, right-facing
charcoal body, broad angular soot-grey triangular wings, short antennae, a dull
rust abdominal speck, flat muted palette, transparent alpha, no floor, shadow,
text or noise. Charged edit changes only inner-wing markings and the abdominal
speck to ember orange/cream, preserving silhouette, scale, antennae and alpha.

Six original cues are reproducible with `tools/sound/furnace_moth.py`: sip, feed,
warning, dive, impact flare and death. Papery noise modulation and short low
resonances give 0.30–0.75s effects with peaks 0.17–0.29. No music changes.

Strict release build passed. Temporary direct checks covered full/partial finite
fuel, permanent-light lures, source movement, dropped-stick extraction, warned
committed attacks, sidestep misses, real cover damage, cold/hit interruption,
flying/burn behavior, snapshot roundtrip and malformed heat rejection. Protected
placement failure leaves the state hash unchanged. Sixty-four generated Industry
floors retained reachable routes and necessary locks, with 67 moths and nine lamp
alcoves. SDL dummy loaded both sprites and six sounds; the inspected static
capture is `/tmp/gauche-lamp-alcove.png`. No interactive playtest or permanent test
suite was added. Balance and readability in live play remain user feedback work.
