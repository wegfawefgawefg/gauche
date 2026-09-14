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


## September 14 feedback implementation notes

Feedback implementation: the puffball causes 75 ticks of sleep, now explicitly
shown. Status badges show countdowns/effects; burning shows current rate and
remaining damage. Blocked walking uses a latched wood/stone bonk with no debris;
movement turns before explicit aim. Fire wisps render between fixtures and actors
and originate higher. F1 hides/restores debug windows; F2 toggles the categorized
selector while visible. Combat has independent player/enemy world-grid gates,
both off by default; inventory patterns remain. Builds and static status/debug
captures checked. Controller handling and contact feel await user playtesting.

Stack/cooking implementation: ammo max stack is ten; partial pickup merges what
fits and leaves excess on the ground. Shops/rewards/crafting remain all-or-none.
Used tools cannot merge; stacks preserve the longer cooldown. Non-stackable gear
has no fake quantity/quantity meter, and detail cards label stackability alongside
uses/condition/ammo. Cooking processes one portion per 45 ticks with a new sizzle,
without consuming raw meat if the result cannot fit. Static inventory capture
and strict builds checked; user owns interaction/balance playtesting.

## Forest water scenes

Brook rooms now contain walkable shallow channels and a small northern-wall
spring/pool scene. Deep water remains impassable. Standing or stepping in
shallows clears weak/strong burning before its next damage tick. Grounded
actors splash; flying creatures do not. Local expanding rings inherit light
and clip at dry banks. The ambience placement recognizes both depths and stops
a water source if its tile changes. Snapshot terrain IDs and gameplay version
were updated. Strict builds and a static spring capture checked; user owns
interaction and balance playtesting. Poison/residue washing remains pending.

## Chicken follow chains and food cues

Families spawn in a chain. A shared trail helper follows departed cells after
a five-tick delay; dead leaders are replaced by a free preceding chain tail
without cycles. Chicks use nine-tick steps, five while scared, with a faster
body wobble. Nearby adults defend the family with pecks; chicks retain eight
seconds of alarm and renew it while the attacker remains close. Cooking and
eating now have separate sizzle and munch cues. Strict build checked; follower
spacing, protective aggression and sound feel await user playtesting.

## Bow and fused bomb projectiles

Splonks' bow arms on press and fires on release. Gauche follows that interaction:
20-arrow quiver, no reload command, a drawn sprite/twang, 16-tick recovery. Each
arrow travels one tile per three ticks, damages on contact and splinters into
local wood/feather debris. Bombs throw up to three tiles, land against obstacles,
and detonate after a 150-tick fuse measured from use; visible sparks, a small
light and separate throw/landing/fuse sounds communicate it. Gameplay projectile
state uses documented shared entity slots and snapshot serialization. An explicit
input cancel bit handles inventory/menu/stun/death; missing network inputs retain
a bow draw instead of inventing release. Wire version increased. Strict builds
and static projectile/inspection captures checked, without playtesting. General
bullet tracers, rocket travel and persistent spells remain to implement.

## Melee action timing

Melee starts with a committed facing/spec and a short windup: fist 3 ticks,
knife 2, spear 5, stick/rake 6, hatchet 8, pickaxe 10, maul 14. Heavy/Agile
modify preparation alongside cooldown. Swing/exertion plays when preparation
starts; actor impact plays on contact. A completed whiff still spends the swing
and its use count. Opening UI, changing the committed slot, dropping, stun,
sleep or death cancels preparation; fixture interactions such as cooking take
priority. Shared player action slots and serialized item storage preserve the
committed attack across rollback. Details expose windup; held art pulls back
along its committed direction. Strict build checked; user owns timing playtests.

## Overhead canopy visibility

Forest overhead foliage uses quiet oak/pine silhouettes anchored to seeded wall
positions. Camera parallax supplies movement; the silhouettes do not drift.
Slow clouds remain in the arena and industrial/fire floors. Both share a wide
central ellipse cleared through the overhead target, with a two-pixel dithered
edge. Ground dappled lighting/shadows remain independent. Foliage inherits local
light rather than making dark areas glow. A flat-background static capture
caught software rendering discarding transparent geometry; cached unblended
rectangle fills now clear the alpha correctly. Scene/inspection captures and
strict builds checked, without gameplay runs.

## Meat as bait

Wolves, dogs, boars and calm bears investigate reachable raw/cooked meat within
six/nine tiles. Bounded path checks reject sealed-away bait; occupied food cells
are not valid destinations. They eat one portion, recover a little HP, chew
briefly, then remain sated. A player directly beside them or an incoming hit
interrupts feeding; committed attacks and an angry bear take priority. Satiety
and feeding use documented shared slots, with deterministic resource consumption.
Strict build checked; user owns attraction and encounter-balance playtesting.

## Forest materials and utility items

Torch, lighter, oil flask, sap jar, water flask, mushroom spores, smoke pot and
honey pot bring the forest regional implementation count to sixteen. Each has
an icon, description, shared pattern, use/stack rules, sound and room/shop/reward
pool entries. Toads can drop spores; calm bears eat placed honey as well as meat.

Thrown jars use visible deterministic flight and break at their landing cell.
Oil/sap ignite from sparks, torch strikes or a burning grounded actor. Fire
spreads one neighbor beat at a time, consumes growth, emits light/flames/smoke
and gives actors a weak five-second burn. Sap/honey slow steps. Water clears
fire/spores and cools lava; smoke interrupts creature sight without blocking
attacks. Sleep clouds catch allies too. Surface state is in tile snapshots and
hashes; wisps, jar scraps and sound remain local. Snapshot version is fifteen.

Strict builds and static world/item-card captures checked, with joined puddle
silhouettes and softer smoke after visual inspection. This is not a gameplay or
network playtest. Oil slipping, honey/wasp interaction and natural sap-bearing
scenes remain follow-up work in the forest catalog.

## Gunfire and projectile aftermath

Crossbows launch a real bolt at three ticks per cell; rockets fly at two ticks
per cell and explode on collision or maximum range. Ammunition is spent only
after allocation succeeds. The launcher has a distinct report, exhaust, smoke
and light; bolts splinter using the arrow's local impact debris. Bombs retain
their landing/fuse behavior. Gameplay version changed; shared projectile slots
already serialize flight and survive owner loss.

Pistol, musket, shotgun, SMG and blunderbuss remain immediate shots. Their local
shot events draw four-tick tracers, muzzle light and impulses, with one brass
case for cartridge guns. Source/endpoint events never enter snapshots or hashes
and are deduplicated during presentation. Projectile/shot paths cross deep water
and stop at physical obstacles. Ten new offline-generated reports/reloads replace
generic cues for these weapons; empty magazines retain the restrained dry click.
Strict builds and a static ballistic scene checked; no combat/network playthrough.
Persistent spells and other regional throws remain catalog work.

## Earlier completed slice notes

Rust Gauche did not have a sequence of cracked wall sprites: damaged breakable
tiles had a health bar and shake, hits threw debris, and a destroyed wall became
Ruin. Before this pass the C++ port let any damage lower wall HP and drew a dark red
rectangle over damaged walls. The new implementation replaces this with shared
branching cracks, a lit HP bar, and explicit impact events for shake/fragments.
Strict release build and the existing snapshot codec check pass, including nondefault
wall HP/dig thresholds and fire state. A static 1080p terrain/fire scene was inspected
with shared cracks, damage bars, ash, smoke, flames and footprints. Reconnect and
gameplay feedback remain with the user; the master goal remains in progress.

Canopy implementation: an authored 16-pixel grayscale mask projects slow-moving
leaf gaps into the shared light field, including walls and actors. Broad cloud
shadows dim only that sunlight. Soft diagonal shafts land on the patches. The
interim generator now retains fewer forest openings and removes them from later
biomes. Static 1080p capture inspected; semantic opening placement remains part
of the room-role generator work below.

Current implementation: nine forest prop kinds, twenty forest scrap silhouettes,
local spatial buckets, step/blast/train/wind impulses, axis-wise terrain collision,
4096-piece budget, per-cell cap and settled piles that scatter on contact. Broken
props and partial HP round-trip in snapshots; reconnect reconstructs settled litter.
Static 1080p prop/debris capture inspected. Placement now uses role-specific clusters around reserved paths and objective
clearance; room content has floor-wide supply and threat budgets.

UI implementation: 0.72 scale for HUD and all modal/menu geometry. Inventory and
rewards render to a larger intermediate canvas so the smaller text keeps its strokes;
click regions use the inverse modal transform. Static 1080p HUD/inventory/reward
and a one-frame lobby capture inspected. Controller cursor switching is implemented;
physical controller feel and click feedback await user playtesting.

Generator implementation: a seeded 10–18-room route tree with optional loops,
an accessible objective detour, a gated exit leaf and occasional weak-wall secret
cache. Eight authored shape rules produce varied sizes, courts, galleries, twin
caves and bent halls; twelve room roles select current enemies, loot, props and
lighting. A protected dry route excludes blocking props and water/lava. Supplies
roll on connected interior floor, and healing/equipment have a floor budget.
Four static seed overviews (1, 72, 22991, 90731) and a normal camera capture were
inspected, with valid objective/exit route summaries. New starts under NoRespawn
now correctly spawn their first player. The first authored unique and wave gates are implemented below; richer biome pools
and user playtesting remain open.

Haunted house implementation: a rare Forest 3 replacement with an outdoor approach,
two ponds and a central 76x68 mansion layout. Its lever gathers living connected
players, gives a 1.5-second warning and closes the gates. Three waves emerge from
announced grave openings; later waves include toppling stacks. Ammo and bandages
arrive between waves, and clearing every linked survivor releases the gates and
exit. Empty-room recovery opens the gates without deleting remaining enemies;
joining/respawning players get a free interior cell during an active encounter.
Linked ownership is separate from AI target slots and included in snapshots/hashes.
Room navigation, four new sounds and gate/grave art are integrated. Strict build,
the existing codec check, a mansion overview and a normal camera capture pass.
The static route check establishes lever access; co-op timing, combat difficulty
and complete wave playthroughs await the user's playtesting.

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
