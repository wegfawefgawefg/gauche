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

## Mirror Knight (2026-09-14)

The eighth ice enemy now guards observatory rooms, costing three threat points.
It has 90 HP and a 22-tick movement beat. With a visible target aligned within
six cells, it spends 18 ticks raising its shield before a 60-tick stationary
guard. Shield direction stays fixed through that stance. It lowers the shield
for a 27-tick committed adjacent sword strike dealing 22 damage, then recovers
for 48 ticks. An unused guard lowers into 24-tick recovery. Guard reuse has a
120-tick cooldown; during advance it pursues visible threats or investigates
noise and wanders using the existing free-neighbor behavior.

Frontal eligible projectile contacts use the same finite flight deadlines and
leg budgets as pan reflection. Bullets, arrows, rockets and recoverable direct
throws can return to their shooter. Thrown explosives and area effects retain
their own rules. Light beams also reflect: the pure beam trace captures the
raised stance before resolving any damage, and shares the original finite
travel budget. A branch can hit the knight from another side even if another
branch reflected. Such a flank hit is not discarded as a duplicate reflection.
Melee still damages it; this is a projectile shield, not universal immunity.

Actual damage, sleep and stun interrupt raise/guard/swing into 48-tick recovery.
A displaced knight's shield fails immediately because its committed origin no
longer matches; its next step cancels the old attack. All strikes and debug
previews use the same fixed target cell. Shared label/timer/point fields own the
stance, with the c-slots left to hearing. No per-species struct or body-position
interpolation was introduced.

Five body poses and two shield images distinguish lowered/raising/ready states;
the separate shield sits on the guarded cardinal edge. Six generated sounds
cover raising, readiness, windup, slash, reflection and armored death. One saved
loot roll gives 20% Mirror Shard, 20% 3–6 gold, otherwise nothing.

Gameplay compatibility is EA; snapshot layout stays 31. Strict game/render/codec
builds and the existing codec check pass. Static stance/directional-shield and
reflected-beam captures were inspected. Live behavior/balance remains for the
user's playtesting; no live playtest or new test suite was run.

## Prism Bomb (2026-09-14)

The seventeenth ice item is a thrown light bomb: three-cell flight at eight
simulation ticks per tile, a fixed 90-tick fuse from launch, 0.75s use recovery,
stack two, price 28. Cover stops the throw on its near side; bodies may pass
under it. Landing does not reset the fuse. The final half-second uses brighter
charge seams/light and quicker fuse cues. Big extends each burst beam from
four to five cells; Long throws four instead of three. Strong and Heavy modify
beam damage; Agile modifies recovery. All Piercing is saved at launch.

At fuse expiry it releases four cardinal 16-damage beams and hits the center
once. The four beams use a single pure snapshot and shared finite queue; actor
and prop damage is deduplicated across the entire burst. Mirrors, splitting
lenses and guarding knights change their paths. A split halves damage, opaque
cover stops a ray, and contact at the bomb's center cannot be shielded by facing
away from it. Cover created on the source cell absorbs the outward burst.
Friends and the thrower can be hit. The bomb deals no contact damage in flight,
and no invisible explosion goes off at the aimed location on use.

The shared trace now accepts either one initial ray or four. The latter gets
four normal ray budgets plus its center, capped at 128 contacts overall. This
keeps mirror loops finite and advances split paths fairly. The world debug
preview uses the actual landing and beam geometry; the item card shows the
unobstructed cross and effective beam length/fuse. Three new images and four
OGG cues cover the item, charge, throw, fuse, landing and burst. Broken prism
pieces use existing local crystal-splinter debris.

Observatory supplies alternate carbine and prism bomb by round, and ice shops
and rewards can offer the bomb. Snapshot layout remains 31, with appended item
and projectile kinds; gameplay compatibility is EB. Strict game/render/codec
builds, existing codec check and image/audio checks pass. Static flight/fuse,
beam-burst and modifier-comparison captures were inspected. No live playtest
or new test suite was run.
