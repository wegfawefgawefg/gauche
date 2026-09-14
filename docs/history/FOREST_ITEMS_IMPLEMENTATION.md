# Forest item implementation

## Woodland tools and planted props

Four further regional items bring the forest to 22 implemented types. Digging
claws deal 8 damage with dig power 1, a three-tick windup, 18-tick cooldown and
18 uses. Resin repairs all missing condition on the carried item with the largest
absolute condition loss (slot order breaks ties); it neither refills uses/ammo
nor consumes itself when nothing needs repair. Its comparison card names the
repair target and shows before/after condition.

A six-use seed bag plants an 8-HP shoot on clear dry ground. Two shoot sprites
show the three-second growth delay before 40-HP blocking root cover. Damage
carries through maturation; occupying the tile holds growth until it is clear.
Actors, items and fixtures cannot be planted over. Lantern seeds stack to four,
consume one per planting and create 12-HP passable flowers with radius-five,
strength-800 yellow-green light. Both plants can be cut or burned. Prop emitters
use shared light data; moth AI searches nearby planted lamps using that same data.
Plants are compact six-byte props, not actor slots. Breakage creates local root
and leaf debris. Growth remaining is hashed, validated and saved in snapshot 19;
gameplay compatibility is CB. Root turrets, bramble guards, woodpeckers and moths
now drop these matching items at the catalog rates. Room/reward/shop pools include
them. Eight sprites and five short synthesized cues have committed offline sources.

Strict game/render builds pass. Static world and comparison captures check plant
stages, lamp color, silhouettes, uses and resin repair preview. No live playtest
or new gameplay test suite was run.


## Forest remedies and temporary benefits

Five new finds bring the regional item count to 27. Herb bags hold four uses;
each heals 18 HP over six seconds without stacking active regeneration. Splints
hold two uses, remove stun and prevent more for three seconds. Bitter root stacks
to five, costs three HP without allowing suicide, wakes the user and prevents
sleep for ten seconds. These two cures can be selected/used while disabled;
other actions still wait until the following tick.

Chili stacks to three and doubles movement recovery for four seconds, followed
by six burn damage over 1.5 seconds unless water quenches it. Attack cooldowns
are unchanged. The camera guide uses the actual accelerated movement beat;
bodies and footprints still use their exact cells. Fungal bread stacks to four,
heals 25 HP immediately and induces two seconds of sleep; bitter root prevents
that sleep. Crows can steal and eat the bread, including its sleep consequence.
Restorative attributes affect healing amounts. Failed uses consume nothing.

New compact status fields store regeneration, sleep/stun resistance and chili
time separately from AI counters. HUD timers explain each benefit; comparison
cards show healing duration, root HP cost and chili movement before/after.
Temporary effects clear on death and floor transition. Snapshot 20 and gameplay
compatibility CC include all fields in serialization, validation and hashing.

Room/reward/shop pools include the remedies; bats and worm heads now drop bitter
root. Healing room budgets choose actual healing supplies; splints use equipment
budgets. Five minimalist sprites and five distinct eating/binding sounds include
offline Python sources. Strict game/render builds and static inventory/root/
status captures pass. No live playtest or new gameplay test suite was run.


## Bird bait and finite thorn patches

Bird seed and thorn caltrops bring the regional item count to 29. Both place
compact six-byte props on clear dry ground, without allocating actor slots.
Bird seed stacks to five; one use puts down twelve beakfuls. Owls, woodpeckers
and empty-handed crows investigate reachable visible piles within eight cells
before starting a new attack/theft. Committed attacks and recovery continue.
Calm adult chickens gather while chicks retain their following chain; family
defense and flight take priority. Birds feed from adjacent cells every 45 ticks,
spending one beakful. Full or unreachable feeding positions do not attract a
permanently stuck bird. No animals spawn from using bait.

Thorn caltrops stack to three. Each handful scatters three facing patches, each
with three contacts at six damage per grounded entry; standing still is safe.
Friends are susceptible, airborne actors pass over, and cutting/burning clears
the patches. Big expands to five facing cells; Long extends placement by one
cell. Damage modifiers persist in the prop's variant byte, which is already
hashed and serialized. A blocked scatter consumes nothing if no patch lands.
Seed husks and snapped thorn debris are local cosmetics.

Owls drop bird seed and thorn snails drop caltrops at their catalog's 15% rates.
Room, reward and shop pools include both. Four PNG sprites and four synthesized
OGG cues include offline Python sources. Gameplay compatibility advances to CD;
the snapshot layout remains 20. Strict game/render builds and static world/
comparison captures pass. No live playtest or new gameplay test suite was run.


## Horn cones and traveling rope hooks

Two further regional items bring the count to 31. Hunting horns have eight uses
and a 100-tick cooldown. Their facing cone covers one, three and five cells at
reaches one through three. Resolution captures visible movable targets once,
then shoves far to near with stable slot-order ties. It wakes affected sleepers
and uses ordinary hard-surface crush rules; friends and loose items count.
The shared pattern carries a cone flag, so cards and debug previews show the
same widening shape. A local outward sweep scatters debris and draws brief
curved gust strokes without syncing cosmetic particles.

Rope hooks have 24 uses and a 75-tick cooldown. The hook travels one tile every
four ticks, latches for eight ticks, then reels up to three cardinal steps at
four ticks per step. A movable actor/item comes toward the owner; walls, blocking
props and anchored actors pull the owner toward them. The user stops before
obstructions and the anchor. Lateral movement, obstructed rope, death, stale
generation handles or a removed/moved anchor releases the tether. Only one
tether per user can exist. Long extends flight range from six to ten cells;
Durable increases uses. Fragile now requires an actual damaging weapon instead
of reducing utility-item uses for a nonexistent damage benefit.

Hook state reuses projectile slots, handles and immutable item data already in
hashes and snapshots. Gameplay compatibility is CE; snapshot layout remains 20.
Closed hard-blocker entities now interrupt the shared sight ray between endpoints,
so closed doors cannot leak creature sight or horn effects through a corridor.
Three sprites and five offline-generated sounds accompany room/reward/shop
integration. Strict game/render builds and static cone-card/tether/gust captures
pass. No live playtest or new gameplay test suite was run.


## Root drills and swap seeds

Detailed rules and validation are in [root relic implementation](ROOT_RELIC_IMPLEMENTATION.md).

## Recoverable flights and returning weapons

Boomerangs bring forest finds to 34. They travel six cardinal cells at three
ticks per cell, dealing 16 damage to each actor once per outward/return leg.
A wall ends the outward leg; a blocked return lands the item. The return tracks
the owner's current cell, so movement can bend its path. It has a bounded life
and drops on owner death or a stale owner handle. A caught weapon has a 24-tick
cooldown. Long/Strong/Heavy/Agile retain their ordinary reach/damage/timing effects.

A generation handle reserves its inventory slot while the physical item flies.
That slot cannot be used, dropped or swapped; cards say IN FLIGHT and the hand
is empty. Catching restores the original instance. Landing clears the reservation
and converts the projectile to a ground item without needing a spare entity slot.
Finishing a floor recalls surviving owners' tools before reward simulation pauses,
allowing inventory comparisons and drops there. A new floor clears old handles.

Throwing rocks now travel at four ticks per cell instead of instant ray damage.
One stone leaves its stack, hits and lands, retaining its attributes. All Piercing
allows continued flight through bodies. Ordered projectile/victim handle pairs
record distinct contacts and reset on the boomerang turn. They and item flight
handles are hashed, snapshotted and validated; layout is 21, compatibility D0.
One sprite and four offline-synthesized cues accompany cache/reward/shop integration.
Strict game/render builds and static in-flight HUD, comparison card and world
captures pass. No live playtest or new gameplay test suite was run.
