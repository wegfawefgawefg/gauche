# Ice implementation notes

## Ice terrain foundation (2026-09-14)

Existing floors 9–12 now choose reservoir, fishing hut, bathhouse, quarry,
observatory, shelter and echo-tunnel roles alongside caches. Native slate,
snow, ice and two-depth water sprites replace tinted forest ground. Snow is
walkable/buildable; reservoir banks, pools and room shapes retain protected dry
paths and the existing gated leaf boundary. Warm shelters use real campfires;
there are no new invisible roof lights. Shared supplies and sparse crates/pots
support the rooms while their regional catalog is implemented.

Four offline-generated snow/ice footing sounds play on grounded steps. Terrain
impacts use the cold wall sprite; the shared crack/contour lighting remains.
TileKind::Snow is appended, handled by existing tile hash/snapshot serialization;
layout stays 27, gameplay compatibility advances to DC. Forest RNG consumption,
terrain selection and room pools stay unchanged. Run order remains forest/fire/ice
until the full four-zone integration. Ice still uses the old five-tick step
penalty; slipping, grit, thawing and the catalog enemies are subsequent work.

Strict game/render builds pass. Static reservoir and generated ice-floor captures
were inspected; the reservoir was adjusted to one basin and a broad southern
bank. Four OGG files decode to finite non-silent samples. No live playtest ran.

## Rime Skater and Grit Pouch (2026-09-14)

Rime Skaters now spawn on reachable ice in reservoirs, quarries and fishing-hut
rooms. They have 50 HP, a 30-tick push-off, five-tick cardinal glide steps and a
12-cell maximum commitment. Their direction is saved before the push. Rough or
gritted landings stop them; walls/props/actors cause a 60-tick recovery and brief
stun. Contact strikes for 18, including another creature or a straw decoy. External
displacement cancels the original lane. They reposition toward visible ice lanes
when not aligned; no body-position interpolation or infinite sliding is involved.
Three native poses and four blade/impact/death cues accompany the behavior.

Ordinary grounded movement on bare ice now takes one extra real cell, stopping
at blocked landings, using the existing bounded oil-slip path. The previous
five-tick ice step penalty is removed. Sticky Boots resist it; living sap/honey
and spent sticky resin also provide traction. Skaters own their glide cadence,
so they do not receive the ordinary extra ice step. Oil keeps its existing slip
rule, including when spilled over grit. Ice slipping has its own short scrape.

Grit Pouch has six uses, price 6 and 30-tick cooldown. It scatters a three-cell
facing strip on clear, dry, ungritted ice. A wholly inapplicable use costs nothing;
a partial strip spends one use. Big covers five cells; Durable supplies twelve
uses. Placed grains stay visibly lit on the ground and stop ice slips/glides.
Water clears them. The empty pouch has a cloth cue rather than a box-breaking
sound. Definition-driven cards show traction area, remaining uses and pattern.
Cold room supplies, occasional rewards, shops and a 20% skater drop provide it.
The later skate-blade drop from the design catalog remains unimplemented.

Surface::gritted is hashed and snapshot-serialized; layout advances to 28 and
gameplay compatibility to DD. The existing codec round-trip includes placed grit
and a partially spent Big pouch and passes. Regional item dispatch now lives in
items/catalog.cpp, with forest and ice definitions kept in their own domains.

Strict game/render builds pass. Static normal/Big comparison cards, the grit
strip and idle/push/glide poses were inspected. Seven new OGG cues decode to
finite non-silent samples. No live playtest or new test suite ran. Ice catalog
completion remains open: this is one of twenty enemies and one of fifty items.

## Frost Bat and Ice Needle (2026-09-14)

Frost Bat has its own entity module and no longer dives or bites using the forest
bat's code. It hangs, inhales for 30 ticks along a committed cardinal lane,
exhales a six-cell frost puff, then relocates toward a nearby wall perch and
rests for 50 ticks. Health is 24. A bounded perch search prefers clear approaches
and firing lanes; route steps remain authoritative cells. Blocked approaches
fall back to rest instead of retrying forever. Hearing still makes it flee.
Displacement during inhale cancels the tell. Three new bat poses include a
pale swollen throat; wing animation never interpolates the entity position.
The old arbitrary bright blue bat light is removed.

Frost puffs are saved projectile entities, moving one cell every six ticks.
They strike the first non-owner actor for 6 damage and two seconds of chill;
shields block both. Walls, closed fixtures and solid props stop the puff, including
when that hit breaks a prop. Exposed fire dissolves it. The existing CHILLED HUD
reports duration and half-speed movement recovery; inventory use stays available.
The shared chill helper rejects dead/anchored actors, flames and Frost Bats.

Ice Needle is a stack-eight recoverable throw: 8 damage, range 7, 30-tick cooldown,
60-tick chill and price 4. A throw removes one from the stack; its real projectile
carries the exact modifier-bearing item until landing. Strong, Agile, Heavy and
Long work through the shared effective pattern. All Piercing can carry it through
actors; parries preserve the existing finite flight deadline. Shielded hits do
not chill. Hot landings melt it before a nearby item cell can relocate it away
from the heat. Other landings restore the physical needle. Heat means lava,
burning surfaces/actors, live campfires and exposed torches/lit sticks, not the
color or brightness of an arbitrary lamp. This is not yet general biome thawing.

Echo tunnels supply needles; Frost Bats drop one on a 15% roll. Ice rewards and
shops can select needles or grit. The formerly unreachable quarry pickaxe supply
now runs inside the quarry branch. Cards show effective damage, chill duration,
stack capacity and pattern; three modifier cards have static capture support.
Eight new offline cues cover inhale, exhale, wings, death, frost dispersal,
melting, needle throw and needle hit, with small local impact particles.

Strict game/render builds and static frost/needle captures pass. All eight OGGs
decode to finite, non-silent samples below clipping. No live playtest or new test
suite was run. Existing fields already serialize/hash the projectile and AI state;
snapshot layout remains 28, gameplay compatibility advances to DE. Ice now has
two implemented catalog enemies and two items; other quotas remain open.
