# World interaction and playtest review

Expanded from the user's brother's playtest and follow-up, 2026-09-15.
[Original wording and attachments](user_messages/README.md) are archived separately;
[later balance feedback](BALANCE_REVIEW.md) expands progression and reward requirements.
[MASTER_TASKS.md](MASTER_TASKS.md) owns execution status. This is the design
specification, not another checklist or a claim of implementation.
[Scene candidates](content/SCENE_CANDIDATES.md) defines proposed new count targets.

## Principles: revisit old content, not just add more

- Catalog counts do not establish readability, usefulness or balance. Reopen
  completed quality judgments when human feedback contradicts them.
- Build opportunities alongside mechanics: mobility needs terrain worth crossing;
  reflection needs useful geometry; material tools need different barriers.
  Give major mechanics multiple sources, uses and counters across biomes.
- Names and silhouettes should suggest behavior: rabbits run/hop, lamps illuminate,
  levers look operable. Include familiar creatures alongside strange inventions.
- Dangerous optional enemies need both a reason to avoid them and a worthwhile
  reason to engage when prepared. Review frequency, behavior, loot and habitat.
- Preserve strong cross-biome carry: forest fire helps in Ice; Ice tools help in
  Industry. Weak toys and obviously broken finds are also welcome.
- Multiple material-based solutions should make players feel clever: axe, fire,
  shovel, pick, bridge, leap and portal. Intentional shortcuts are desirable;
  randomly generated unwinnable required routes are bugs.
- Keep simple native pixel art, dark readable lighting and negative space. More
  ecology does not mean noise everywhere. Render actors at their true positions.
- Tiny gameplay objects may use compact structs, but size does not excuse absent
  synchronization. Cosmetic motion must never influence authoritative rules.

## Current-source findings and limits

- Rabbit Charm currently has three retreats of up to three ordinary movement
  cells, triggering intermediate hazards. This design is superseded below.
- Swap Seed currently travels then swaps two actors; change to Blink Seed below.
- Mimic has 160 HP and can re-hide, but drops only a 50% chance of an ordinary
  supply/tool. Prior toughness/hiding work does not settle the new feedback.
- Bear has 140 HP, small initial detection and an eight-cell territory leash.
- Storm Lantern is specified as a fueled, shuttered lamp with 120 seconds of
  fuel. Reported empty/unusable appearance needs an acquisition/state/UI audit;
  do not assume the players simply operated it incorrectly.
- Pocket Door places two paired thresholds; living actors share entry logic.
  Thrown wall passage and projectile traversal are additional work.
- IceFlag is scenery. Its current appearance was mistaken for a pickup.
- Ordinary generation has individual connected rooms, special roles and a
  separate haunted-house floor, not a general multi-room landmark framework.
- The ~two-minute pursuer is requested in the conditions brief but no general
  floor pursuer timer was found in code. Haunted-house encounters are separate.
- Biome lists and shared supplies exist. Pocket Door appears in broad reward/shop
  lists. A unified universal/native/rare-import policy is not established merely
  by those lists; review all actual drop/acquisition paths.

## Mobility and terrain

Firm direction: replace Rabbit Charm's retreat with faster movement while held
or actively used. Recommended starting design: hold Use to run faster, occupying
the selected item slot; stop the benefit on stowing/release. Choose resource or
cooldown tuning with weapon switching in mind. Record the held-versus-active
decision explicitly. Preserve spatial escape under a more appropriate item theme.

Separate candidates: Rabbit Foot two-tile hop, leap attack, wings that sustain
brief flight while moving, and a luck item that sometimes hops out of an incoming
attack. Automatic evasion is a candidate, not the new Charm requirement. Use
deterministic RNG and actually evade the particular attack before damage resolves.

Generate small, double and large gaps, rewarding detours, tar that holds prey and
attracts hunters, injury pits and lethal chasms. Give different consequences distinct
visuals. Flight ignores appropriate ground contact, not all attacks. Define valid
landings, occupied destinations, stopping, duration expiry, shoves and airborne
hazard rules. Show real path/airborne state, never misleading sprite-only motion.
Early crossings can guard optional rewards; mandatory crossings need a guaranteed
usable solution, sufficient charges and a return/rescue policy.

One-way ledges use explicit directional edge rules. Slow ladders/steps can exist
on one plane without a second height layer. Define enemy, shove and shot traversal.

## Overheads, tall props and actual height

Build grouped overheads on a single walkable plane first: hollow log/root tunnel,
ice arch/sculpture, overhead metal grating. Reserve a connected reveal group;
fade/dither it for each local viewer underneath. Restore on exit, keep entrances,
actors and collision legible, and retain the user's preferred dark canopy. Remote
players must not fade unrelated overheads on another client's screen.

True over/under crossings are a separate decision. Two actors sharing x/y on
different decks require explicit layer state for occupancy, navigation, attacks,
effects and snapshots. Entry direction may select a ramp but must not secretly
change a tile's rules. Do not delay hollow-log passages for multi-deck machinery.

Tall props can initially have one occupied base cell with a taller sorted sprite.
Separate base occupancy, art height and falling footprint. Examples: column shakes
ceiling icicles; ice pillar topples or melts into surrounding water; tree burns
upward; spore tree dusts passersby; sakura drops short-lived drifting petals; hive
tree drops a disturbed hive; streetlight falls and shatters, extinguishing its light;
fixed crane leg/truss suggests a large structure without secretly occupying its art.

Falling props need direction, a shadow/footprint warning, delay, impact and final
terrain/debris. Severe or instant crushing needs an escape opportunity. Shelter
must affect ceiling falls consistently. Fixed supports must look immovable.
Damage, drops, falling hit areas, water and light state are authoritative; shakes,
petals and shards can be local. Room configurations matter as much as variants.

Water/ice reflections remain exploratory polish. Start with a clipped reflected
sprite pass and mild distortion if useful; full screen-space reflection is not
required. Reject effects that muddy hazards or duplicate important silhouettes.

## Material routes, fire and scenes

Create fallen giant root masses opening into nearby rooms through axe work or fire.
Build forest spider caves with burnable webs, little spiders, adults/mother
candidates and trapped rewards. Required web gates need alternate solutions or
guaranteed usable ignition, not a random chance of finding a lighter.

Extend wall materials: shovel-soft earth/snow, axe-cuttable roots, pick-hard stone
and ore, unbreakable boundaries. Route compatibility through existing break rules.
Shallow holes dug by a shovel are a candidate: decide slowing/trapping/drainage
instead of assuming digging a hole always produces a lethal pit.

Timber Forest: a dangerous connected-fuel variant, with roughly 80% burnable
coverage in the stress scenario. Water, firebreaks and cutting ahead should matter.
Use deterministic active-cell propagation with bounded work and bounded latency;
cap cosmetic fire separately. The user explicitly requests a performance check:
temporary noninteractive stress measurement of simulation, render load and network
state/snapshots. This does not require live autonomous playtests or a large suite.

Bear scenes: sleeping bears on nests, bone-pile breakables, droppings, fish remains,
and a bear swatting fish from a stream and eating them. Noise and danger wake
nearby sleepers. Catchable/edible/bait fish are gameplay; decorative minnows can
use cheap local presentation.

## Items: identity, useful toys and connected rules

Replace Swap Seed with Blink Seed in behavior, name, art, descriptions and pools.
Keep thrown travel; blink the caster to a legal impact destination. A wall hit
should resolve to the last valid near-side landing, not inside the wall. Define
range exhaustion, occupied impact, moving/dead caster, hazards and sealed encounter
boundaries. Use matching departure/arrival flashes. Old actor-swapping may survive
on a separately themed future item; do not silently retain it as Blink behavior.

Expand citrus, pomes, berries, snappy bushes and seed piles with small/medium/large
harvest yields. Snap Seeds are a candidate cheap contact pop, initially around
five impact damage and two fire damage, with explicit ignition behavior. One use
still means one seed regardless of pile size. Connect fruits to existing food,
cooking, bait and remedies where appropriate.

Rename/re-present Storm Lantern as an unmistakable lamp; working candidate:
Shutter Lantern. Audit spawn/reward/drop/shop/snapshot fuel and shutter state.
Show fuel, open/closed status, visible light and device-correct controls. Empty
finds explicitly say empty. Choose refill sources deliberately; don't assume a
boiler refill exists. Other candidates: stackable glowsticks, flashlight, visibly
dead flashlight. A field guide covers creatures, items and places: explain basic
operation immediately while unusual combinations can remain discoveries.

Arrows crossing actual fire gain authoritative burning state, visible flame,
appropriate ignition on impact and water quenching. Deliberately extend this to
other combustible projectiles rather than keeping a one-off visual trick.

Pocket Door family:
- Preserve two placed pads under an appropriate name/theme if replacing the item.
  Throwable placement should visibly unfold and obey valid landing rules.
- Wall-cast door finds an eligible exit beyond the wall, however thick. Map-edge
  wrap is a deliberately powerful variant, not permission for ordinary digging
  to breach boundaries. Specify landing and progression consequences explicitly.
- Evil door picks a deterministic random eligible destination with distinct warning.
- Heaven door reaches the exit. Decide whether it reaches the near side, bypasses
  a lock, or spends a rare progression skip; other portals need not share that rule.
- Actors and projectiles can pass; preserve shot direction, remaining range and
  fuse time. Bound chains and prevent immediate ping-pong. AI should be able to
  follow a route through a door, not only stumble through incidentally.

## Shop/reward presentation and transactional input

Prominently announce SHOP and CHOOSE ONE OF THREE with Gauche's angled styling,
strong focus and a merchant/cart/greeting at shops. Bottom-left text is inadequate.
Show the offer's benefit and owned artifacts. Levers separately need unmistakable
operable silhouettes, position changes and physical latch/mechanism sounds;
wall levers and protected glowing-box buttons suit different biomes.

Physical South confirms, East cancels in menus (Xbox A/B; actual icons elsewhere).
Gameplay West/X picks up/interacts, including keys/progression fixtures. Review
South/East world assignments rather than making menu accept actuate world objects.
Left-trigger reload is a proposed mapping needing review alongside secondary/use.
Right-trigger attack must never buy, select a reward or leak into a new screen;
use separate action contexts and fresh presses.

Flow: choose offer -> display result/cost -> if it cannot fit, NO ROOM and a
purpose-specific slot replacement picker -> choose droppable item -> confirm
old/new/cost -> atomically exchange. East/B backs out a step and returns to offers
without spending, claiming or dropping. Do not open a generic inventory and expect
the player to figure out dropping. Respect merging/partial stacks, bound items,
variants, prices, vanished offers and legal placement of displaced items. Validate
the final transaction once on the authority so cancel/retry/co-op cannot double
charge, duplicate stock or consume an unconfirmed reward.

## Enemy review, bosses and readable Ice archetypes

Audit every enemy: name/silhouette, notice, windup, strike, recovery, sound/effect,
counterplay, habitat and drops. Prioritize snail, zombie and the legacy fiery
circle with gun. Identify inherited art/behaviors for rework or retirement. New
Ember Stoker work may supersede the observed old build; assess current code rather
than dismissing the report. Restore zombie swipe and missing attack effects.
Ordinary enemies should not deliver unavoidable first-frame notice damage;
clearly recognizable contact hazards can have a separate explicit rule.

Mimic should be less routine, more threatening, and reward preparation above a
normal box. Retain damage when hiding, drop once. Bear should be an intimidating
optional predator with commitment and terrain interaction, not only more HP.

Ice candidates: territorial yeti throws adjacent intruders with little immediate
damage; wall collision adds brief stun/damage, open space permits useful throws.
King yeti, mammoth and ice-blowing penguins add readable archetypes. Twenty is a
minimum, not a reason to reject these or discard all unusual existing creatures.
Beam enemies/boiler porter need threatening habitat compositions and functional
behavior outside those rooms, even when less effective. Place reflectors/cover
thoughtfully; indiscriminate beam clutter makes a poor encounter.

Boss choice remains open: room seals shortly after entry for a miniboss, or a
large discoverable boss region embedded in the biome's fourth floor. Allow world
interactions; decide per encounter whether digging out is valid escape. Handle
party entry, split players and deterministic release. Giant bear, spider mother
and wolf-leader fights are candidates. Large art does not mandate multi-cell
collision; if 2x2 occupancy is needed, define navigation, narrow passages, targeting,
shoves, hazards and snapshots. One hit overlapping four cells must not quadruple.

## Haunted-house follow-up (2026-09-15)

User played the house and likes it as a promising mode, but easily punched through
the encounter. Preserve that foundation while adding tactical tools and variety.
This feedback is summarized selectively, not archived verbatim.

Make ordinary house zombies individually weaker and simpler, with more bodies
creating pressure. Replace instant adjacent swipes with recognizable preparation,
strike and recovery. Coordinate with the baseline Fist nerf: do not compensate
for overpowered fists by turning every zombie into a damage sponge. Crowd danger
should come from encirclement, entry lanes and lost space rather than unavoidable
first-frame damage. Tune counts/cadence for solo and co-op with an active-body
budget; telegraph arrivals and avoid spawning directly onto players.

Provide crowd-control resources before/around activation: placeable or repairable
barricades, doors that can block routes, and clearly marked lever-operated fire
or spike lanes. Give barriers material, HP, placement/repair cost and breaking
feedback. Zombies should batter reachable blocking barriers when their route to
prey is obstructed. They can be tactically dumb without freezing against a closed
door or always knowing the globally optimal breach. Select nearby reachable
breach targets with bounded work; reconsider when a passage opens. One zombie
must not stop the whole crowd navigating toward an available entrance.

Trap controls must visibly correspond to their affected area; telegraph activation,
define cooldown/fuel/reset and friendly-fire consequences, and leave an escape
route. Barricades buy time, not permanent safety. Do not let player construction
softlock a wave by sealing required spawn points or stranding the last zombie.
Preserve encounter death/disconnect/rejoin and gate-release behavior.

Build several house variants with distinct circulation: galleries and chokepoints,
connected small rooms, courtyard loops and multiple entrances, rather than merely
changing props in the same large open shell. Add an outside hut/outbuilding and
consider a small compound of houses as a variant. Supply and danger placement
should reward exploring and preparing. Define the active encounter region across
the yard/buildings so outbuildings are not accidentally inaccessible after locking.

The current authored shell does contain partitions, but the user's experience
still reads as one open area; improve meaningful layout differences and readable
room identity. Reuse the encounter controller and multi-room landmark work where
appropriate. Add variant selection/repeat to existing debug tools. A separate
standalone mode is an opportunity, not an immediate requirement inferred from
the user's enjoyment. Current fixed Shotgun supplies and intermission Ammo also
need the shared biome/weapon-balance audit rather than remaining hidden exceptions.

## Small life, landmarks, frozen contents and ghost

Compact local populations can orbit/follow a home actor/source, scatter from steps,
appear under conditions and fade/flee. Mini bees, moths, ground bugs and fireflies
add life without full actor overhead. Cap/cull and respect negative space. Loot,
AI triggers, damage or blocking require deterministic synchronized gameplay state,
even with compact structs. Do not let local decorative bees secretly deal damage.

Reserve landmarks before normal carving/population: 1x2, 1x3, 2x2 or other explicit
multi-room footprints, external sockets, internal rooms, habitat/reward budgets,
legal rotations and group overheads. Protect required objective relationships.
Snake tunnel, spider cave and giant hollow tree with root openings should appear
occasionally across a run without forcing a landmark on every floor. Add debug
selection. Existing large single rooms do not meet this requirement alone.

Distinguish decorative IceFlag from loot; visibly freeze real items/enemies inside
other blocks. Release exact stored contents once when thawed/broken. Some blocks
can spontaneously crack with warning. Fragile ice spreads step/hit-initiated cracks
to neighboring fragile tiles at high chance using a deterministic bounded frontier.
Define resultant water/hole, falling/rescue and bridge behavior. Not all ice is
fragile. Add cracking/melting chips, water/steam and sound consistent with the
moment collision changes; no invisible blocker or visibly solid walk-through ice.

Implement the requested general ~two-minute ghost: arrival warning, slow pursuit,
lethal contact, floor-clock reset, shop/reward exclusion, explicit unique-floor
exceptions and team-pause/reconnect/co-op targeting rules. Refer to the existing
conditions brief for shared policy rather than inventing a competing timer.

## Delivery and acceptance

Prioritize shop/input transactions and attack readability, then misleading items
and danger/reward. Build mobility with useful terrain, overheads with one hollow
log landmark, then expand ecology/material routes/landmarks/tall props/bosses.
Decide pursuer policy before balancing timed escape variants.
Audit loot sources as universal, native and very rare imported finds; current
lists do not prove the desired policy. Every slice needs actual art, audio,
sources/drops, generated uses, state readability and relevant serialization.
Build/static checks are implementation evidence; live balance remains the user's.
