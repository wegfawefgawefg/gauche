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


## Snares, spring traps and acorn mines

These three finds bring the forest item catalog to 37. Place on an empty dry
walkable adjacent cell, then arm for 20 ticks. Intact props, fixtures and actors
prevent placement without consuming an item. Grounded actors trigger them;
flying creatures do not. All traps affect friends. Setup/catch/release/launch/
break/burst cues have eight new offline sound sources and seven sprite states.

A rope snare has 8 HP and roots its captive for 180 ticks without disabling
attacks or item use. Its generation handle releases safely on death, removal,
a shove/teleport away, expiry, or cutting the rope. After ordinary release it
becomes a recoverable ground item in the same entity slot. A destroyed rope
is lost. The general root timer is shared actor state, independent of AI slots;
movement rejects voluntary steps, while forced movement remains possible.
A rope overlay and ROOTED timer explain the restriction on the player's HUD.

A spring has 20 HP, fires once, and applies up to two cardinal shoves along its
visible arrow. Ordinary actor blockers stop travel; hard surfaces crush. It
removes itself before landing contacts, so chained springs are finite. Each
landing processes hazards; outer contact does not double-trample a fire, and
footstep sounds use the final cell rather than the originally requested step.

An acorn mine has 8 HP and bursts on contact or destruction, including fire,
melee and explosions. It hits its cell and casts four 22-damage splinter lanes,
each two cells long, stopping at walls, blocking props or the first body.
Other mines can chain. Big extends lanes to three cells; Strong/Heavy affect
damage. Cards and debug previews use a cross pattern. Splinters and shell husks
also enter local movable debris. None of those cosmetics changes gameplay.

Root duration enters hashes and snapshots (layout 22, compatibility D1). Trap
state reuses existing counters, timers, facing, handles and copied items.
Workshop/cache finds and reward/shop pools contain the new items. Strict game
and render builds plus static armed-state, root-HUD, base/Big comparison captures
pass. No live playtest or new gameplay test suite was run.


## Nets, gripping boots and retreat charms

These three finds bring the regional forest item count to 40. Room finds,
rewards and shops include them. Five generated sprites and seven offline cues
cover throwing/catching/falling cloth, sticking/releasing boots, retreat and oil.

Throwing nets travel one cell every four ticks, three cells wide, up to three
cells forward. The first occupied row roots eligible actors for 120 ticks;
attacks remain available. Walls and blocking fixtures stop the center lane and
tear off edge lanes. Big widens to five lanes; Long extends reach to seven.
The net is consumed; local cloth scraps remain. Root overlays distinguish nets
from snares, and a snare cannot release a newer net root.

Sticky boots have four uses. Each provides 360 ticks of grip, blocking shoves
and oil slips while doubling ordinary movement recovery. Freeze and haste
compose with that delay. Crushers still crush; swaps remain teleports. The HUD
shows the grip duration and tradeoff, and item cards show actual step deltas.

Rabbit charms have three uses. Each retreats up to three cardinal cells opposite
aim, applying every intermediate contact. Obstacles, death, rooting, or a spring/
oil displacement stop the retreat. A fully blocked attempt consumes no use.
Actors stay on their real cells; this introduces no visual movement interpolation.

An ordinary grounded step onto oil slips one extra cell in its direction if
that cell is clear. Slip does not recurse across an oil lake or crush actors
against blocked tiles. Landing hazards still apply. Grip/root prevent slipping.

Grip and root kind enter hashes and snapshots (layout 23, compatibility D2).
Net payload, lanes and timers reuse existing saved projectile fields. Strict game
and render builds pass. Static item comparisons and world captures were inspected,
including Big Net, step deltas, charge counts and status overlays. No live
playtest, network session or new gameplay test suite was run for this slice.


## Bells, firecrackers and creature hearing

Two more finds bring the regional forest item count to 42. They enter room,
reward and shop pools with three generated sprites and five new offline sounds.
The bell uses an inharmonic brass ring; crackers have paper toss/landing, quiet
fizz and a sharp bang. Local rings, sparks and loose scraps do not affect state.

Hand Bell has 80 uses and a 90-tick cooldown. Each ring wakes living sleepers,
including teammates and stationary creatures, and calls curious species to its
current cell. Memory lasts at most 300 ticks. Wolves, bears, boars, dogs, zombies,
stacks, unladen goblins and mimics can investigate. A hidden mimic reveals itself.
An adjacent visible player, arriving at the location, losing a route or being hit
ends investigation. Species-specific food/carrying/defense priorities remain.

Firecrackers stack to five and throw four cells at eight ticks per cell. The
90-tick fuse starts on throwing, continues after landing, and does not depend on
the owner's survival. Walls stop the throw at the previous cell. The bang wakes
sleepers and draws investigators along ten cells of open path. Within three cells
of open path, small species receive 30 ticks of stun and 150 ticks of startle
memory, including that stun time. Birds, bats, small insects, toads and rabbits
flee once their current committed behavior allows it; players are not stunned.
Big expands startle to four cells, Long throws five. Neither version deals damage.

Gameplay hearing uses an explicit bounded cardinal flood around the noise, with
walls, blocking props and closed doors/gates stopping propagation. Bodies and
smoke do not stop it. The same footprint feeds debug pattern rendering. Ordinary
ambient and audio playback events never invoke hearing: mute settings and local
cosmetic sound randomness cannot change an enemy's behavior.

Shared point_c/label_c/timer_c retain origin, investigate/startle mode and memory.
Each participating species calls the helper at an interruptible point instead
of applying one stepper to all actors. Existing bite/dive/charge tells and chicken/
wasp trail recording remain in their own steppers. These fields enter snapshots
(layout 24, compatibility D3) and hashes; spawned actors initialize them empty.

Strict game/render builds and static world/item captures pass, including the Big
startle footprint and the bell's centered radius diagram. No live playtest,
network session or added gameplay test suite was run for this slice.


## Scent, rotten fruit and pitch

Three more regional finds bring the forest count to 45. Five generated sprites
and eleven offline cues distinguish soft fruit, ceramic scent jars, sniffing,
nausea and sticky burning pitch. Room/reward/shop pools include all three;
rotten logs have a 15% fruit drop. Static captures cover haze, rot, flames/fuses,
item comparisons, nausea and the actual bound secondary-action prompt.

Stink bombs fly four cells, then leave a radius-one scent patch for 600 ticks.
Hungry wolves/bears/boars/dogs, crows and unladen goblins investigate reachable
scent within eight cells. Food/combat/noise retain their species priorities.
Sniffing spends 90 scent ticks and holds attention briefly. Water and fire clear
scent. Follow/sniff use the shared c slots; neither steals attack or flock state.

Rotten fruit also flies four cells and splats a radius-one patch for 600 ticks.
Grounded contact refreshes 180 ticks of nausea: one damage per second on a saved
independent beat. Refreshing cannot postpone that damage forever. The HUD shows
its timer/rate and water cure. Scavengers nibble the finite patch; crows also carry
and eat loose fruit. On the reload/secondary binding, players can instead eat one
for 3 HP and 360 ticks of nausea. Full-health/empty/cooling attempts consume none.
The item card uses the active device/profile glyph for this contextual action.

Pitch bombs fly six cells with a 120-tick fuse starting on use. The radius-two
splash deals 12 damage, chips exposed props and deposits burning sap. Its covered
cells are captured before destruction, so breaking cover cannot expand the same
burst. Burning floor sap lasts six seconds, then becomes nonfuel spent sap for
the remaining residue lifetime (15 seconds total). Both forms add eight recovery
ticks to ordinary steps. Wood uses its own ignition lifetime. Water quenches and
washes residue; friendlies and existing traps remain vulnerable to the splash.

Big widens all three patches; Long extends throws; damage attributes affect pitch.
Scent/rot rendering is local, while liquid kind/lifetime, scent lifetime, nausea
and its damage beat are hashed and saved (snapshot 25, compatibility D4). Debug
footprints use the same bounded wall/cover-aware splash cells as gameplay.
Strict game/render builds pass. No live playtest or new gameplay test suite ran.

## Scarecrow wards and straw debris

The forty-eighth regional find is a stackable pair of scarecrows, priced at 16
gold, with a 45-tick placement cooldown. Each plants a blocking 28-HP prop on an
empty dry buildable cell. It uses the existing six-byte Prop; no actor slots or
new simulation structures are allocated. The prop can be attacked, burned and
broken. New bundle/standing sprites and offline stake/cloth-tear sounds accompany
it. Breakage leaves local straw and cloth. Straw is a new pale, wind-sensitive
loose material (21 forest debris types), and debris sprites now have an explicit
lookup table instead of assuming contiguous sprite enum indices.

Chickens, crows, owls, woodpeckers and rabbits avoid the ward. Its four-tile
Manhattan radius requires clear sight: walls, cover, closed hard fixtures and
thick smoke interrupt it. Big extends the radius to five. Inside a ward, creatures
choose a free neighbor with lower pressure or an equal-pressure escape step;
a penned creature hesitates until a route opens. Existing flock, threat, carrier
and perch state stays intact. Chicken retreats still record their follower trail.

Ordinary AI steps refuse increasing pressure, and bird/rabbit path searches mark
covered cells once in their occupancy mask. The physical movement function stays
unchanged, so a shove, tether, slip or teleport can force an animal into danger.
Larger predators and insects ignore wards. Already committed owl dives, beak
charges and crow thefts finish before fear applies; new attacks into a covered
cell are refused. The ward is useful shelter, not protection from attacks already
in motion. Breaking or obscuring it immediately removes its influence.

Cards show radius, prop HP, stack/consumption and cooldown, with the ordinary/Big
pattern comparison. The debug footprint uses the same sight-aware coverage as
AI. The general Big attribute label now says wider effect area, since utility
wards and noise are neither strikes nor explosions. Cache/reward/shop pools
include the item. Existing prop serialization/hashing covers all ward data;
snapshot layout remains 26 and gameplay compatibility is D7.

Strict builds and static card/world captures pass; no live playtest or new test
suite was run. Straw Decoy, Wolf Whistle, Pocket Door and Thunder Acorn remain
unfinished forest catalog work; the other biome catalogs remain separate work.


## Straw decoys and attacks on cover

The forty-ninth regional find is a stackable pair of straw bodies, priced at 12
with a 45-tick placement cooldown. A body is a blocking 40-HP Prop placed on an
empty dry buildable neighbor. Its stored variant holds attention radius six;
Big extends it to seven. Placement, hits and destruction have separate generated
sounds. Standing/bundled sprites and local straw/cloth breakage distinguish it
from the scarecrow ward. It has no loot or entity allocation.

Target acquisition can return a living actor handle or a dummy cell. Susceptible
predators, undead, root/guard enemies, owls, woodpeckers and Ember use this chooser.
A visible dummy scores as two tiles nearer than a player, within both its lure
radius and the creature's own detection range. Walls, cover, fixtures and thick
smoke interrupt attention; player ties and dummy scan order are deterministic.
Insects, passive animals and food-stealing crows retain their existing targets.
The spore toad can waste a sleep puff on straw without inventing damage for it.
Pack references, territory, hearing, foraging and committed windups remain in
those enemies' existing functions. Dummies never rewrite another actor's state.

Attack sight permits a blocking prop only at the ray endpoint. Committed attack
coverage is captured before damage, so breaking front cover does not expose
cells behind it to the rest of that attack. Bites, boar charges, bat dives and
owl swoops damage props they actually reach; pathfinding allows a live dummy as
a solid goal without allowing travel through it. Zombie stacks stop approaching
once adjacent instead of stepping away from their bite target.

Cards show lure range, prop HP, stack and cooldown. Debug coverage follows the
same sight ray; ordinary and Big inspection patterns scale to fit. Room caches,
rewards and shops include the item. Prop state already hashes and serializes;
snapshot layout stays 26 and gameplay compatibility is D8.

Strict game/render builds pass. Static normal/Big cards and placed/broken prop
captures were inspected. No live playtest or new test suite ran. Wolf Whistle,
Pocket Door and Thunder Acorn remain unfinished forest catalog work.

## Wolf whistle

The fiftieth implemented regional find is an eight-use bone whistle, priced at
20 gold with a 90-tick cooldown. Its cardinal 1–6 call stops at cover, thick smoke
or the first solid actor. Long extends the call to ten cells; Durable doubles
uses. A failed call spends nothing. A target wolf must be outside a pack and not
already biting/recovering; nearby wolves and live leader/follower links count as
a pack even between pack refreshes. This is a distraction, not permanent taming.

The nearest visible hostile within eight cells becomes its prey, with stable
slot-order ties. Players, wolves, dogs, passive chickens/rabbits and fixtures are
excluded. The five-second call reuses label_c/timer_c for WhistleHunt and entity_b
for the generation-checked prey. Home, hunger and pack references remain intact.
The normal 18-tick bite tell, 11 damage, 32-tick recovery and pathfinding perform
the hunt. Noise, actual damage, lost sight, distance beyond twelve, expired time
or invalid/dead prey ends the redirection. Already committed bites still finish.

A small lit musical mark and draining timer identify a called wolf. Bone-whistle
art and breathy two-note/answer sounds are generated offline. Item cards show
uses, range, cooldown and the five-second lone-wolf rule; rewards, room caches
and shops supply it. The new attention mode is accepted by snapshot decoding;
existing fields cover its hash/save state. Layout stays 26; compatibility is D9.

Pocket Door and Thunder Acorn remain in the original forest catalog despite the
50-find count, which includes added eggs and fried eggs. Other biomes and the
remaining master systems still require implementation.

Strict game/render builds pass. Static ordinary/Long cards and the wolf's call
marker/timer were inspected. Both OGG files decode to finite, non-silent samples
with headroom. No live playtest or new test suite was run.


## Thunder acorn

The fifty-first regional find is a three-acorn stack priced at 30, with a
60-tick cooldown. A real projectile travels one cell per four ticks along a
cardinal line, up to five cells, and discharges at the first body or obstruction.
It survives its owner leaving or dying. Empty-range impacts produce sparks and
can ignite fuel, but do not invent an area attack.

A discharge captures at most four distinct generation-checked victims before
applying any damage. The first hit deals 24; subsequent hits deal 18, 12 and 6.
Each jump chooses the nearest eligible actor with stable slot-order ties. Dry
bodies reach one Manhattan cell; grounded bodies in shallow/deep water or a
poured-water patch reach three. Flying creatures do not become wet from the floor.
Big adds one to both distances, Long extends the initial flight to nine, and
normal damage modifiers scale the whole chain. Players, including the thrower,
can be later victims; no actor is selected twice. Electricity bypasses a facing
block. Cover, walls and closed doors block arcs, including diagonal corner gaps;
water and smoke do not. Capturing first prevents breakage or zombie topples from
adding fresh targets during the same discharge. Sparks ignite existing fuel
through the shared surface rules and damage props on struck cells.

The pattern has a chain flag: inspection shows the first-hit lane and outlined
potential wet jump area rather than promising a filled explosion. Cards show
all four damage values and both jump ranges. World debug preview uses the same
chain selector. Two 16px seed sprites and two offline sounds accompany blue
jagged ribbons, short spark forks and local light flashes. Electrical endpoints
reuse presentation-only shot events; cosmetic jitter and flashes never affect
hashes or snapshots. Projectile state uses existing shared slots. Snapshot layout
stays 26, and gameplay compatibility is DA. Rewards, shops and room caches supply
the item. Pocket Door remains unfinished from the original forest catalog.

Strict game/render builds pass. Static normal/Big cards, traveling seed and
dry-to-wet chain captures were inspected. The first impact drops local acorn
fragments. Both sounds decode to finite, non-silent samples with headroom.
No live playtest or new test suite ran.
