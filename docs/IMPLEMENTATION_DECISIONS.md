# Gauche implementation decisions

- Keep the old Rust commit/history intact. Do not transplant Rust history into
  the new C++ repository; link it as the source reference.
- Do not port dead placeholders merely to match enum names. Replace the old
  settings stubs with Gubsy settings; the new exit objective supplies a real
  win/transition rule in the first run slice.
- Preserve the old control semantics first (movement, use direction, mouse,
  inventory, pickup/drop, zoom). Gubsy remapping can expose those same actions.
- The first pass should use the original authored PNG/OGG assets. Load each
  PNG directly by its `Sprite` name. Keep asset metadata in code unless a
  concrete Gauche asset later needs richer data. New-content placeholder
  sprites may be generated with a small Python script at the source's usual
  16×16 scale.
- Pin the Gubsy dependency rather than relying on whichever of the two local
  Gubsy checkouts happens to be on disk. They currently differ.
- The Rust history remains in `gauche-rs`; the new C++ history is published as
  `gauche`. Both local checkouts track their matching GitHub repository. The
  `port-finish` and `direct-netcode-finish` tags mark the playable port and
  direct rollback milestones.


Earlier completed content notes are archived in
[early content implementation](history/EARLY_CONTENT_IMPLEMENTATION.md).

## Camera, facing and cursor feedback

Adventures with Chickens transitions its displayed actor to a finite guide point
and centers the camera on that same displayed position, then interpolates across
simulation ticks. Gauche now uses that approach: local actor poses finish each
step over the movement beat (capped at twelve ticks); the render remainder
interpolates positions and the local camera together. Teleports/new generations
reset their guide immediately. Actor poses remain outside game state. A static
fractional-position/four-direction capture and strict builds checked; actual
smoothness across the user's monitor/frame rate awaits their playtesting.

Left-facing held art now mirrors its local vertical axis before the 180-degree
rotation, keeping the top upright. The custom cursor draws once after menus and
ImGui at window resolution, with high-DPI conversion; the OS/ImGui cursors stay
hidden. Pad mode still hides Gauche's cursor. Removed duplicate casing emission
from the old pose observer; actual shot events own firearm casings.

## Contextual ground items and input prompts

The pickup binding is now one contextual press: merge/fill carried stacks and
empty slots first, otherwise exchange the ground item with selected droppable
equipment. On empty ground it drops the selected item. Swapping reuses the
ground entity and preserves the entire item; oversized pickup leftovers reserve
an extra entity before committing. Fists are currently the non-droppable item;
future attached/cursed gear must extend the shared eligibility function. Pickup
is edge-triggered so holding X cannot swap repeatedly. It cancels a pending bow
or melee action before exchanging equipment. Gameplay version changed.

HUD/inventory/reward prompts now resolve from the active Gubsy profile and last
keyboard/mouse versus controller input. Both legacy Gubsy IDs and encoded device
bindings are supported. Keyboard keys have offset caps; SDL's actual face-button
labels select letters or PlayStation shapes, with layout-specific shoulders and
triggers. The ground hint distinguishes pickup, swap, drop and full pack. Profile
menus, pause diagrams and remaining shortcut handling are still the menu pass.
Strict builds and static keyboard HUD/inventory captures checked; controller
hardware/layout switching and pickup feel await user playtesting.


## Profile management and menu reference

Defaults are read-only in name editing, binding capture/removal, and controller
settings. Creating a profile copies Gauche's default bindings and selects it for
the indicated local player; duplication copies both bindings and tuning. The
profile list marks Active, selects on its main row, and exposes Edit/View as a
separate action. Names commit on Enter, navigating away, outside click or ending
editing with Back. Invalid/reserved/duplicate names retain the previous name.
GView's Confirm-only text commit is handled at the Gauche integration boundary.

Profiles, editing and pause have a device-specific bindings legend and schematic.
Hover/focus previews a profile without selecting it. Create is above/right of the
list; shared Back buttons sit bottom-left; Quick Run precedes Play. Lists scroll
rather than pushing navigation offscreen. The entire menu usability task remains
open for continued polish, including FPS placement and remaining shortcut hints.

Four new offline synthesized menu sounds give dry ticks for navigation/change,
a short confirm and a descending back sound. A separate local mixer voice follows
master/effects volume, deduplicates rapid hovering and never enters gameplay state.
Strict executable/render-target builds and isolated-data, dummy-SDL static menu
captures checked defaults, new-profile presentation and keyboard/controller
legends. These are render checks, not hardware input or gameplay playtests.


## Wildlife movement and mimic threat

Wander now chooses only unoccupied walkable neighbors, with an explicit short
rest choice. Previously each failed random direction consumed an entire movement
beat. Approach also tried blocked moves before its wander fallback, setting
move_wait and suppressing that fallback. It now checks neighbors first; mothers
returning toward home use the existing route search to negotiate corners.
Adult hens/roosters have 45 HP (survive four ordinary 10-damage punches), chicks
remain fragile. Mimics have 160 HP and a twelve-tick movement beat; their existing
committed bite cue and escape opportunity remain. Gameplay compatibility version
advanced. Strict build checked; movement feel and threat balance await user playtesting.


## Canopy color and campfire anchor

Oak/pine overhead art now uses clearer greens and 225/255 opacity instead of
148/255. A subdued canopy daylight floor prevents unlit wall samples from turning
leaves black; this affects only overhead art, not ground illumination or visibility.
Anchored parallax and the broad dithered center cutout are unchanged. Campfire
flames start 0.18 tiles lower so their base overlaps the wood's upper portion;
actors still render above the campfire flame layer. Strict builds plus static
canopy-only and campfire/world captures checked.


## Reward focus and owned artifacts

Item cards previously always drew a red banner while health/artifact rewards did
not, making reward type look like selection. All reward types now share the same
banner: red belongs only to the focused option, accompanied by a short upward
lift and its configured Take/Buy prompt. Mouse hover previews a choice before
clicking; compare and pack actions remain available. Owned artifacts appear as
named HUD badges and a two-column inventory strip with concise actual effects.
A shared artifact catalog keeps reward and owned descriptions consistent, including
Reflector's 25% half-hit return and Hearth's 1 HP/s, four-tile aura. Static reward
focus and four-artifact inventory captures plus strict builds checked.


## Raking and nest food

Rake swings emit local cosmetic sweep events on contact, pulling loose scraps
toward a small pile ahead of the player. Existing debris wall collision and
material friction remain; no debris enters snapshots or gameplay hashes. Rakes
have 400 uses. Nests retain their occasional drop chance but produce eggs. Eggs
and fried eggs stack to twelve and restore 3/12 HP, with distinct sprites and a
new soft bite/swallow sound. Campfires cook one portion at a time, prioritizing
the held ingredient and committing only when the cooked portion fits. Existing
meat cooking uses the same path. Strict game/render builds and a static inventory
capture checked; the user retains playtesting. Gameplay compatibility advanced.


## Temporary burning sticks

A held stick can be lit at a live campfire for thirty seconds. Its flame timer
travels with the item through drops, swaps, snapshots and hashes; stowing keeps
the clock running. Burnout leaves the ordinary stick and its remaining uses.
Wading or dropping it in water quenches the flame early. Contact ignites dry
material and living susceptible actors (20 damage over five seconds, refreshed
without stacking); blocked strikes and fire-native actors resist ignition.
Torches now ignite struck actors through this same path. A dropped lit stick
can ignite dry ground props or spilled fuel. Effective item emitters also feed
moth attraction. Held, ground and UI flames share the wood-tip anchor, and HUD,
inventory and details show remaining fire time alongside uses/cooldown. Snapshot
format is 16; gameplay compatibility advanced. Strict game/render builds and
static world, HUD and inventory captures checked; no playtest performed.


## Held tool silhouettes

The pickaxe has a narrow curved double-ended metal head rather than an axe-like
slab; its placeholder generator matches the asset. The original Rust fist sprite
is rotated clockwise so right-facing use punches forward. Enlarged nearest-neighbor
asset inspection checked both silhouettes; held left-facing mirroring is unchanged.


## Root turrets and bramble guards

Two more forest adversaries bring the implemented roster to twelve. Root turrets
have 65 HP, remain rooted, turn in delayed quarter turns, coil for 42 ticks, then
lash a fixed five-cell lane for 18 damage and recover for 90 ticks. A directional
bud communicates facing and a short visible vine shows resolution without debug
patterns. Fire affects the rooted plant as well as mobile susceptible actors.
Bramble guards have 90 HP, ordinary breakable bucklers, and generation-checked
wards. They choose a cell between a ward and the threat, excluding the ward's
committed attack cells; a 24-tick adjacent swing exposes them before a 42-tick
recovery. Orphans seek another nearby ward or return to territory. Thickets and
shrines can contain roots; guarded pairs start after forest round one and consume
the existing threat budget. Current drops are 20% stick / 15% sap jar; planned
seed-bag/resin-glue finds remain pending. Four new synthesized cues, four sprite
poses and wood/leaf death scraps are integrated. Strict builds and static sprite/
encounter renders checked, with no gameplay playtest. Gameplay version advanced.


## Camera/canopy playtest follow-up

Actor interpolation made the body lag behind its actual tile, making attacks and
footprints appear displaced. The temporary footprint compensation is reverted:
footprints use the original authoritative cell placement. Bodies and held items
snap to their real cells; only the camera follows an interpolated guide. Existing
shake, tilt and squash remain. This preserves the cute tile steps without hiding
where collisions and attacks happen.
Canopy RGB modulation is one fifth of the previous result; opacity, green source
art, anchored parallax and center mask remain. A presentation-only capture feeds
recorded cardinal cells into the cosmetic observer and shows the restored prints.
Diagonal input now alternates cardinal axes from the last successful move, like
Chickens' successful-axis selection. A blocked preferred axis falls back to the
other before spending its beat. This preserves speed, bump latching and explicit
aim overrides. Shared counter_c stores the last axis in snapshots/hashes; snapshot
format is 17 and gameplay compatibility advances. Strict game/render builds pass;
no gameplay playtest or new test suite was run.


## Mosquitoes, owls and woodpeckers

The forest roster now has fifteen implemented adversaries. Mosquitoes (12 HP)
hover for eighteen ticks, dart up to three steps toward a committed old position,
drain five HP on contact, heal only actual drained health, then retreat/digest.
A missed dart leaves a 42-tick opening. Owls (38 HP) watch a territory, hoot for
forty ticks and swoop to an old target cell marked by a small wing shadow. They
rest and return to their perch instead of chasing indefinitely. Woodpeckers
(30 HP) rattle before a fixed six-step drill, chip props and dig-power-one terrain
for eighteen damage per peck, and pause ninety ticks after a hard collision.
Their attack on an actor is ten damage and ends the drill. Flying creatures no
longer stomp ground props/campfires or make water footsteps; smoke/sleep still
affect them. Six sprites, eight synthesized cues and distinct flight poses join
clearing, brook and workshop pools. Current 10% water / 15% egg / 15% rock drops
are live; planned bird seed and digging claws remain pending. Strict builds and
a static flier encounter capture checked; no gameplay playtest performed.


## Flock yield pressure

Calm followers use counter_c as a short yield request. When their preceding bird
is boxed in, a chick tries a free cardinal step away from it. If also blocked, its
request passes down the adjacent follower chain until a tail has space. Each bird
still pays its own movement interval and records its departed cell for followers;
no simultaneous recursive push, overlap or teleport. Seeded random direction order
varies the shuffle without creating local-only gameplay. Gameplay compatibility is
C6; the existing snapshot already includes counter_c.

The corrected actor/footprint rendering passes strict game/render builds and a
static recorded-position capture. Flock behavior has not been playtested.


## Finite brood and forest scavengers

Wasp nests (70 HP) hold six births and cap their living brood at three. An awake
150-tick release beat warns for forty ticks; smoke/spores restart it at ninety.
Sleep pauses brood progression. Nests burn and leave an empty papery hive when
spent. Linked wasps (16 HP) have staggered 18–26 tick committed stings for eight
damage, then 36 ticks of recovery. Unalarmed wasps can be diverted with honey;
orphaned wasps scatter for two seconds before becoming short-range nuisances.
Nest drops: 40% honey pot; wasps: 5%, otherwise nothing.

Forager goblins (34 HP) patrol between saved endpoints, take reachable loose
items and carry the actual instance home. They flee nearby players, but a boxed-in
goblin warns for 22 ticks before a six-damage knife strike at its committed cell.
Crows (24 HP) seek meat/eggs, call nearby crows toward a food carrier, and warn for
24 ticks before stealing one portion from a still-adjacent carrier. They visibly
carry it back to their perch and eat it after three seconds; death before eating
returns the food. Goblins return their carried gear and roll 50% for 3–7 coins.
Both use ordinary held-item rendering, preserving ammo, wear and attributes.

Nine sprites, eleven offline synthesized cues, brood/wing poses and orchard,
workshop, ruin and den placement accompany the behavior. Forest roster is now
nineteen; the linked burrow worm remains. Game and static-render targets build
strictly, and a static pose/held-loot sheet was inspected. No gameplay playtest.
Gameplay version C7 rejects peers missing these entity kinds and rules.


## Door bypass at offset room joins

A door was placed at one endpoint of a narrow bent corridor. With jittered room
centers, other cells along that bend could connect both rooms, bypassing the lock.
Exit and secret leaf rooms now get a one-tile boundary wall with exactly one
opening facing their graph neighbor. The outside approach is explicitly rejoined
because the old bend may lie on the boundary. The boundary remains ordinary
diggable terrain; only free walking around the lock is prevented. Secret entrances
keep their chipped barrier and optional nature.

The layout diagnostic now checks both a solvable key-to-exit route and that the
objective is reachable while blocking the door makes the exit unreachable. Static
layouts 1, 2, 7 and 41 pass both checks. Seed 41 caught the approach-bend issue during
the change and passes with the explicit outside join. Strict builds pass; no live
playtest. Geometry changes apply to newly generated floors; gameplay version C8.


## Diagonal camera comfort

Actor positions and footprints remain tile-exact. The camera now averages two
movement beats of actual player cells before following with a critically damped
spring. Averaging the pair turns steady alternating cardinal input into a straight
diagonal camera path. A spring alone retained noticeable side-to-side sway, so it
was insufficient. The sampling window follows the movement beat, including the
step delay on sticky/wet terrain and frozen movement; only presentation owns it.
Teleports and new generations reset the history. Rendering interpolates camera
samples at the display frame rate; there is no body-position interpolation.

A recorded-pose camera plot (straight, diagonal, stop; no gameplay stepping) went
from 0.247 tiles of cross-path sway with the spring alone to below 0.001 with the
paired average, and settled at the player's true cell. Static footprints were
also checked. Strict game/render builds pass. Comfort remains for user playtesting;
the tradeoff is a little more camera follow delay, not delayed controls or hits.


## Burrow worm and forest wood

The twentieth forest adversary is a head (55 HP) and up to three linked segments
(26 HP each). Bodies follow departed cells, keep the chain together, and halve
ordinary blockable damage; piercing bypasses this armor. Severed followers become
heads without healing and pause fifty ticks while exposed. Heads warn for 24 ticks
before a 12-damage committed bite, recover for 32, and visibly dig eligible terrain
for 16 per digging beat after a rumble. Actual tunnels are usable by everyone.
Three sprites/four cues and later thicket/den placement are integrated. Heads
currently roll 20% raw meat; the planned bitter-root drop awaits that item.

Wall tiles now carry Stone/Timber/Tree material, preserving ordinary collision and
corner lighting. Timber has 60 HP and trees 90; both take ordinary damage and triple
hatchet damage, burn down to splinters/stumps, and emit wood/bark debris. Generated
workshops have timber partitions; forest edges/clumps have trees. Reserved routes
and the stone boundaries of gated rooms are preserved. Burning wood lasts up to
ten seconds and takes six damage each half-second; a source in a burning wall can
light its surroundings without relaying light through neighboring walls.

Strict builds and a static woodland render pass. Layout seeds 1, 7 and 41 retain
solvable routes and sealed locks with wood placement. Material is hashed and saved
in snapshot 18; gameplay compatibility is CA. No gameplay or networking playtest.


## Frame-rate display

Gubsy measured FPS only when its ImGui/debug frame began. Gauche owns that frame
itself, so the counter stayed zero. Measurement now counts actual presented frames
over half-second wall-clock windows, including frame caps and vsync, independently
of debug UI. Gubsy exposes that value and lets hosts opt out of its default overlay.
Gauche places a quiet counter beneath its zoom label, clear of the floor title;
the initial unmeasured value is `--`. Show FPS still uses the existing saved setting.
Strict builds pass; a static HUD capture checks placement. No live playtest.


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


## Tile-light gradient facets

The Splonks gview workspace and Gauche both submit four corner colors across two
triangles. With sharply varying canopy samples, that makes an artificial diagonal
brightness crease inside each tile. Gauche now bilinearly interpolates those same
four corner values over a four-by-four subdivision. Shared edge colors, point
source strengths, canopy stencil, grading and ambient darkness are preserved.
This adds geometry within the existing tile draw call rather than additional
light samples or a different gameplay light field.

A static diagnostic enlarges the most affected cell from the canopy fixture and
shows old/new interpolation side by side. Its original diagonal-center green
error relative to bilinear interpolation was 0.09864; subdivision bounds that
error to 0.00617. Canopy-on/off world captures retain the patterned patches with
a softer interior gradient. Strict game/render builds pass. No live performance
or playtest claim; each drawn tile now submits 25 vertices and 32 triangles.


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

Detailed rules and validation are in [root relic implementation](history/ROOT_RELIC_IMPLEMENTATION.md).

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
