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
