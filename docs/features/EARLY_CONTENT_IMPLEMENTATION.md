# Early content implementation notes

Historical decisions; active work remains in [MASTER_TASKS](../MASTER_TASKS.md).

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

