# Observatory optics implementation

## Lens carbine, mirrors and splitting lenses (2026-09-14)

Three more regional items are implemented. The Lens Carbine fires an instant
18-damage cardinal beam to 12 cells, with three loaded charges, six spare,
0.4s recovery and 1.33s reload. Strong, Heavy, Agile, Long and Piercing use the
shared effective item definition; All Piercing also passes actors. Opaque cover
always stops light, including a prop destroyed by the current shot. Clear water
is transparent. Pan bullet reflection does not turn light beams.

Mirror Shards place an 8-HP diagonal reflector, stack to three and consume one
per placement. Secondary/reload rotates an adjacent mirror while holding a
shard or an empty slot, including immediately after placing the last shard.
The carbine's reload remains a reload even when facing a mirror. Crystal Lenses
place an 18-HP splitter, consume on placement and do not stack. They turn an
incoming beam into two perpendicular branches with half its damage, rounding
down. Both props are nonblocking, survive footsteps, and use existing dry,
empty-cell placement checks. They are glass, not idle fuel; ordinary damage,
bullets and explosions can break them into local mirror chips/crystal splinters.

The pure beam trace snapshots geometry and generation-checked targets before
resolving damage. Each ray advances cardinally, a mirror starts a fresh leg,
and each branch alternates progress through a shared budget of four original
ranges (maximum 128 cell contacts). Splits below two damage terminate. Repeated
contacts apply only the strongest hit to a cell once per pulse; a mirror loop
cannot multiply damage. Reflections can return to the shooter, and friendly
fire remains possible. The attacker's incoming cell supplies shield-facing
logic after a turn. The world debug preview uses this same trace.

Observatories now scatter a few mirrors/lenses instead of generic crates and
can supply a Lens Carbine. All three items enter cold reward/shop pools at the
catalog prices. The existing ice generator, equipment budgets and protected
route cells still apply; designed beam puzzles and mirror enemies remain open.
Five 16px images and six offline-generated OGG cues cover equipment, debris,
placement, turning, glass damage/breakage, firing and reload. Beam ribbons and
one muzzle flash are local cosmetic events, never simulation state.

Snapshot layout remains 31: new item/prop enum values append to existing values,
and mirror orientation uses the already serialized prop variant. Gameplay
compatibility advances from E8 to E9. Strict game/render/codec builds pass;
existing snapshot codec check passes. Static optics, mirror-card and lens-card
captures were inspected. No live playtest or new test suite was run.
