# Gauche master task list

This is the **single live checklist**. [Content brief](design/CONTENT_FARM_BRIEF.md): scale/constraints; [forest ideas](design/FOREST_CONTENT_IDEAS.md): candidates; [NEXT_TASKS.md](archive/NEXT_TASKS.md): history. Preserve dark, readable lighting.

Execution started 2026-09-14. The user owns playtesting and balance feedback. Use builds and focused render/asset inspection, without large new test suites or autonomous playthroughs. Unchecked work stays visible; note uninspected visuals.

## Balance and original feedback (2026-09-15)

[Verbatim user-message archive](user_messages/README.md) preserves the full 1–13 message, IDEA/A–C interludes, preceding screenshot notes and latest balance feedback. Keep originals intact; this file alone tracks execution. [Balance review](design/BALANCE_REVIEW.md) specifies the work below. Balance proceeds alongside content, preserving abundant toys and the enjoyable Forest experimentation.

- [ ] Priority from latest playtesting: Ice and Industry/lava feel barebones and boring beside Forest. Improve encounters, meaningful optional spaces, useful gear and interactions; catalog quotas alone do not establish a good biome. Keep content work moving after the current bounded music batch.
- [ ] Audit the excess feed/fuel and narrow short-status-removal items against broadly useful tools/weapons and actual acquisition frequency. Preserve some odd/useless toys (the user welcomes these), while adding worthwhile applications for fun tools such as Fishing Line. See the latest priority in [balance review](design/BALANCE_REVIEW.md).
- [x] First practical-use pass: optional Ice salvage islands and hot Industrial pockets, valuable native weapon rewards derived from the master table, a matching three-cell bridge span and real-item fishing retrieval. Reserve one Ice/Industry equipment slot for a native combat tool before role-specific supplies spend the budget; remove their Forest Stick/forced Bow fallback. See [salvage pockets](features/SALVAGE_POCKETS.md). Broader biome/loot review remains open.

- [ ] Audit every normal loot source for biome identity/power/availability: SMG, machine guns and rockets must be exceptionally rare Forest imports; bows, blowguns and fitting woodland tools lead native pools. Evaluate aggregate biome/run chances and starting ammo, not just per-roll rates; exempt explicit debug loadouts.
- [x] Substantially reduce SMG Ammo refills: now eighteen spare rounds/use, reduced starting reserves, per-weapon budgets, explicit all-carried-gun/duplicate behavior and actual quantities in details. See [first supply balance pass](features/SUPPLY_BALANCE.md); human tuning remains ongoing.
- [ ] Audit underpowered throwables: tune actual damage/utility and useful acquisition stack sizes together with recoverability, timing, price and slot value; apply quantities consistently across drops/containers/rewards/shops and preserve stack merging.
- [x] Heavily nerf baseline Fist: damage 10→4 with its three-tick windup/twelve-tick cooldown preserved. Real damage/timing checked; live feel and encounter balance remain human feedback work.
- [ ] Design earned punching equipment/augments (heavy glove, lightning/light/heavy/fire/poison candidates), with explicit acquisition, slot/permanence, stacking, material interactions and counters.
- [x] Replace Hearth's unlimited stacking regeneration with finite cooked-meal sharing: +3 HP to the eater and eligible nearby friends, once per consumed portion; no recipient multiplication or passive recovery. Inventory/offer text and focused co-op checks are in place; see [Hearth balance](features/HEARTH_BALANCE.md).
- [ ] Continue artifact/attribute/buff balance for absolute strength, price and earliest availability, including Hearth acquisition/offer value; expand creative conditional augments and plan growth across the intended sixteen-floor run.
- [ ] Rework shop/three-choice reward valuation so same-stage offers are meaningfully competitive; bundle/replace weak toy offers appropriately without making every toy overpowered. Review loot, ammo, melee and permanent-power balance during each content slice using focused calculations and human feedback.

## Content architecture maintenance

- [x] Keep item definitions in their domain modules with one catalog referencing them; replace the growing provider-search chain with direct item-kind lookup as the catalog expands. The 140-entry table checks enum order/count at compile time, including explicit legacy entries. See [item catalog maintenance](engineering/ITEM_CATALOG_MAINTENANCE.md).
- [x] Replace duplicated general loot memberships with one explicit item-supply catalog, referenced by rewards/shops/caches/secrets/workshops; keep themed room, enemy and container sources explicit. Native stage/weight rules, rare imports and shared bundle previews/purchases are integrated; see [supply balance](features/SUPPLY_BALANCE.md).
- [x] Split item-use routing, substantive behavior and completion/resource handling by responsibility as needed. Industry one-shot tool dispatch and shared successful-use completion have separate owners; substantive behaviors remain in their domain modules. Keep ordinary switches and split further when growth warrants it. See [item catalog maintenance](engineering/ITEM_CATALOG_MAINTENANCE.md).

## World interaction review (2026-09-15)

Latest direction: [expanded specification/source audit](design/WORLD_INTERACTION_REVIEW.md), [scene candidates/counts](content/SCENE_CANDIDATES.md). Supersedes conflicting Rabbit/Swap Seed advice and reopens quality judgments below; implemented counts do not certify readability or balance. This remains the single live checklist.

All counts are starting baselines, never content ceilings. Newer proposed per-biome targets: eight tall prop families, eight small-life types and six multi-room landmarks. Boss/unique-floor quotas were previously unset; provisional planning proposal is three boss encounter designs and two unique whole-floor designs per biome, with final counts/rosters open. Boss alternatives need not all appear in one run; multi-room landmarks are distinct from whole-floor uniques.

Layering follow-up adds **six proposed cross-room feature families per biome**, separate from rooms/landmarks: rare post-geometry formations crossing existing room boundaries, such as burnable buried logs opening shortcuts. Usually absent; initially at most one major feature per ordinary floor, with run-level rarity rather than independent per-tile rolls.

[Level-feature distinctions](design/LEVEL_FEATURES.md) clarify floor variants, triggered disasters and infrastructure. Reactor exit meltdown is an objective-triggered event; Timber Forest is a variant that enables wildfire; a station is a landmark attached to map-crossing railway infrastructure. Categories compose and do not imply new arbitrary quotas or separate engine frameworks.

- [ ] Rework haunted-house pressure: weaker/simpler ordinary zombies in larger crowds, readable delayed swipes, spawn/crowd budgets and solo/co-op pacing coordinated with the Fist nerf. Preserve the mode's promising foundation.
- [ ] Give haunted houses crowd-control tools: build/repair barriers, blockable routes, zombie breach behavior and marked lever fire/spike lanes with costs/cooldowns, friendly-fire rules and no stranded-wave softlocks.
- [ ] Add distinct haunted-house layouts, meaningful internal rooms, outside hut/outbuildings and compound candidates; define encounter boundaries, placement/supply budgets and debug variant/repeat access. See the [house follow-up](design/WORLD_INTERACTION_REVIEW.md#haunted-house-follow-up-2026-09-15).
- [x] Repair shop/three-choice transactional replacement: NO ROOM slot picker, explicit exchange confirmation, cancellation and atomic co-op stock/cost validation; South/Enter confirm, East/Escape cancel, West gameplay pickup/interact including keys, LT default reload. No attack-trigger purchases; corrected Pack/Continue and Equip/Drop mouse regions. Angled titles and static captures are in place; see [offer exchanges](features/OFFER_EXCHANGES.md).
- [ ] Finish shop/reward arrival presentation: merchant/cart, distinct arrival cues/music and fitting animation. Continue the separate offer valuation/artifact balance pass; the transactional repair does not complete this presentation work.
- [ ] Audit every enemy's notice/windup/strike/recovery, attack effects and first-hit counterplay; prioritize snail, zombie swipe and legacy circle/gun enemies. Rework inherited art/behavior; compare current Stoker to the reported older build.
- [x] Replace Rabbit Charm retreat with persistent held movement speed; no charges, no teleport, no haste multiplication, ordinary roots/slowing/hazards preserved. See [movement toys](features/MOVEMENT_TOYS.md).
- [ ] Design separately themed escape, hop, leap, moving-flight and optional reactive dodge alongside useful gaps/tar/pits and solvable required routes; the Rabbit change does not implement these terrain/generation candidates.
- [x] Replace Swap Seed with thrown impact-destination Blink Seed: delayed arc, legal near-side landing, hazards, matching flashes, native supply and serialized flight. Revalidate moving/occupied/dead-caster and sealed-arena cases; actor swapping remains a separate future candidate. See [movement toys](features/MOVEMENT_TOYS.md).
- [ ] Audit/rename Storm Lantern as a readable fueled lamp, including all acquisition paths; add light-toy candidates and a creature/item/place field guide distinguishing operation from discoveries.
- [ ] Revisit mimic frequency/threat/valuable loot and bear aggression/commitment; populate sleeping/fishing dens with nests, bones, droppings and fish ecology.
- [ ] Make levers unmistakable through silhouette/state/sound and mechanism response; design biome-appropriate wall levers/protected-button variants.
- [ ] Build grouped overhead reveal starting with hollow logs/root tunnels/ice arches; add directional ledges and slow steps. User follow-up: proceed on one gameplay plane, defer stacked decks and multi-cell moving enemies; neither is a prerequisite.
- [ ] Build tall reactive props with sorting, light breakage, warned falling/crushing, melting and debris; proposed eight families per biome in scene candidates. Reflections remain exploratory.
  Industry streetlights now flicker, warn and tip after a lethal hit, damaging a three-cell lane and leaving cuttable metal wreckage. Native workrooms and Freight Exchange place them; bodies sort around the base and the pole fades over players behind it. Other families remain pending.
- [ ] Add smooth phase-based tipping/falling art with ground anchors, vertical offsets and clear impact cues; landed logs become connected blocking sections with explicit occupant/loot/terrain resolution, without requiring multi-cell actors.
- [x] Prototype shared stylized height/contact shadows independent of lights: creatures, pickups, solid props, projectile flight/falling survivors and most loose debris including sticks/leaves/casings. Footprints/flat stains/wisps stay excluded, existing owl landing warnings remain separate. F1 Presentation toggles, matching static comparisons and dense software-render measurements are in place; see [contact shadows](features/CONTACT_SHADOWS.md). Live visual feedback and future tall-object coverage remain iterative content work.
- [ ] Add material routes: axe/fire root masses, shovel-soft and pick-hard walls, shallow-hole candidates, populated burnable-web caves and alternate/guaranteed solutions.
- [ ] Implement Timber Forest connected-fuel variant and the requested bounded noninteractive fire performance check, including cosmetics/network load and deterministic propagation.
- [ ] Implement Industrial reactor-exit event: crank/activation and gate-opening rules, clear warning, bounded radioactive-fire spread and/or structural-blast variants, intentional escape routes and authoritative co-op/pause/reconnect state. Expand the existing conditions-brief task using [level-feature rules](design/LEVEL_FEATURES.md).
- [ ] Build rare infrastructure fitting over base geometry: station plus active trains, Forest two-way road with occasional cars/trucks and breakable streetlights. Define geometry cutting versus live ramming, route lanes/endpoints, warnings, protected-objective intersections, compatible population and final validation; share large-feature rarity budgets.
- [ ] Design/implement rare fire-engine response event: approach siren/lights, legal entry/parking, firefighters disembarking, finite buckets, reachable fire targets, truck refill and tank/stream/hydrant supply options. Shared wetting/quench rules, crowd yielding and co-op state; very rare Ice freezing-water and joke fire-spraying variants stay explicit candidates. See [response crew](design/LEVEL_FEATURES.md#rare-fire-response-crew).
- [ ] Add biome-boundary scenery/material profiles: snowy/icy Forest 4, woodland remnants in Ice 1, thaw/water/heat in Ice 4 and residual ice in Industrial 1. Retain primary biome identity and native loot/enemy policy; audit local material rules separately from global climate.
- [ ] Give uniques/landmarks/cross-room features/events biome-stage placement windows plus environmental requirements and whole-window rarity caps; e.g. a transition feature eligible across Forest 3–4/Ice 1 rather than everywhere in either biome. Add forced profile/event inspection to debug tools.
- [ ] Add compact dwelling scenes: Forest house, isolated tent, hovel, tree house/hut with one-plane entrances and overhead reveal. Reuse appropriate structures with biome-specific frequency/context, including potentially common tent camps in the provisional military fourth biome; supply fitting inhabitants/furnishings/loot without treating every small dwelling as a unique floor.
- [ ] Expand fruit/berry sources, bushes and seed-pile stack yields; design contact-popping Snap Seeds and implement shared burning-arrow/fire/water interactions.
- [ ] Build compact source/condition-based small life, proposed eight types per biome; local cosmetics versus authoritative gameplay follows effects, not struct size.
- [ ] Add multi-room landmark reservation, sockets, internal population/loot and debug selection; proposed six per biome, including legal 1x2/1x3/2x2 regions and overhead groups.
- [ ] Design/implement rare cross-room feature placement after base geometry and before ordinary population: six proposed families per biome in [scene candidates](content/SCENE_CANDIDATES.md#cross-room-features-six-proposed-families-per-area). Use actual material/terrain edits, useful conditional links, deterministic floor/run budgets, explicit overlap/lock-bypass rules, final solvability checks and forced-feature debug inspection.
- [ ] Add frozen loot/creatures and fragile-ice crack propagation, bounded deterministic updates, release-once contents, falls/rescue and melting effects; distinguish IceFlag from pickups.
- [x] Add the first territorial throwing Yeti: committed grab windup, real-cell airborne tosses, wall-impact counterplay, shared height/shadows and Ice cliff placement. See [Yeti notes](features/ICE_YETI.md).
- [ ] Revisit Ice threat compositions and readable archetypes: throwing yeti, king yeti, mammoth and penguin candidates; beam/boiler enemies must also function outside authored sets.
- [ ] Choose/implement biome-fourth-floor boss areas/minibosses, world interactions, digging escape policy, co-op entry/release and meaningful rewards; giant bear/spider mother/wolf leader candidates.
- [ ] Implement requested ~two-minute floor ghost with warning, timer/pause/reward/shop/reconnect policy and unique-floor exceptions; haunted house does not implement this.
- [ ] Expand Pocket Door into thrown/shared actor-projectile passages, wall/evil/heaven candidates with explicit boundary/lock rules; retain paired pads under an appropriate theme.
- [ ] Audit all loot sources for universal/native/very-rare-import policy and cross-biome usefulness; review danger/reward alongside catalog quotas.

## Conditions and world feedback

- [ ] Implement [conditions and world feedback](design/CONDITIONS_AND_WORLD_FEEDBACK.md): water/campfire and owl bait fixes; ~50 conditions and useful splints; ice momentum/cold-water rescue; excavation crews and shoot-through barriers; optics/dev-mode gate; compound objectives, escape hazards, pursuer and team-pause policy. Modifier counts remain exploratory.
## Tile rules and damage feedback

- [x] Give tiles one explicit break rule: **unbreakable**, **damageable**, or
  **dig-required**. A dig-required tile also has a minimum dig power; a
  damageable tile needs only ordinary damage. Keep current/max HP for anything
  that can break. This avoids contradictory `diggable` and `unbreakable` flags.
- [x] Give items an optional numeric dig power (zero when absent). Damage still
  determines how quickly an eligible tile loses HP; dig power determines
  whether the hit can start digging. A fist and ordinary gun have no dig power;
  a pickaxe does. Make generated forest walls dig-required and map boundaries
  unbreakable. Reserve damageable for boxes, weak barriers, and suitable built
  barricades. Define bombs, rockets, and the train's wall-cutting exception
  explicitly so their behavior does not depend on incidental attack damage.
- [ ] Route melee, shots, blasts, and train impacts through the same tile-rule
  decision. A rejected hit must leave tile HP unchanged and give clear hard-hit
  feedback. Keep objective rooms and exits reachable with available tools.
- [x] Replace the C++ wall's flat red damage rectangle with the Rust feel:
  impact shake, material debris, hit/break sound, a small HP bar while damaged,
  and a final ruin tile. Add a shared crack or chip overlay that grows with lost
  HP and is tinted per material, if the bar alone is too subtle. Do not require
  separate half-broken art for every tile. Check visibility under colored light.
- [ ] Cover all three break rules, dig thresholds, explosions/train exceptions,
  tile HP, and tile state through rollback, snapshot, and reconnect checks.

## Campfires and movement effects

- [x] Add distinct small flame and smoke particles above an active campfire.
  C++ already emits smoke every 12 ticks, but it is faint; tune its opacity and
  scale. Let flame particles read brightly in a dark room without turning the
  entire room into a uniform glow.
- [x] Resolve campfire contact **once when an actor enters its cell**, not each
  stationary simulation tick. A susceptible actor catches fire for about five
  seconds and takes about 20 damage over that duration. Keep this deterministic
  and separate its strength from existing Ember and lava burns so their balance
  does not change by accident. Decide refresh behavior when entering another
  fire; repeated contact should not multiply damage unpredictably.
- [x] On ignition, play an appropriately panicked scream in short, controlled
  bursts and attach flame/smoke particles to the burning actor as it moves.
  Audition the existing `ape_scream.ogg` before using it; provide a fitting
  source sound if it does not work. Particle timing remains local cosmetics;
  burn duration, damage, and fire state belong in deterministic game state.
- [x] Count one trample per entry into a lit campfire. Each trample plays a
  distinct sound and briefly lowers the fire's own light strength; it recovers
  unless the fifth trample extinguishes it. Show an extinguished state, stop
  flame/smoke and light emission, and stop campfire cooking once it is out.
- [x] Draw the campfire base below actors and its flame effects above them, so
  a player on the same tile stays visible. Fire cores render under actors; small attached
  burning flames and smoke render above them. Use intentional world layers rather
  than entity-slot order. Check items, traps, fire, actors, and held items when
  they overlap.
- [x] Restore visible left/right footstep sprites behind walking actors.
  Keep them subtle and cosmetic, readable on the forest floor.
- [x] Preserve original tile footprints; snap actors to real cells and smooth only the camera.
- [ ] Test entry-only trampling, five-entry extinguishing, burn damage/death,
  cooking before/after extinction, and deterministic rollback/reconnect. Check
  particles, sound cadence, overlap, and footprints in a captured scene.

## Light sources and visibility

- [ ] Audit generated roof lights against room geometry with a temporary
  diagnostic view or capture. Generation currently adds a light to each main
  room and branch without a visible skylight or lamp. Give each retained source
  a visible canopy gap or cave opening; remove or move sources that make an
  apparently empty room glow. Check neighboring rooms and border tiles after
  changing fixture positions.
- [x] Shape forest sunlight into dappled, irregular patches rather than plain
  circular pools. Use small authored grayscale canopy silhouettes as masks at
  the game's pixel scale, with gentle, slow movement. The same projected light
  pattern must affect floor, walls, and actors standing within it; keep dark
  negative space between patches.
- [x] Add a few translucent shafts above the world where sunlight enters.
  Their bright ends should meet the projected ground patches, and foliage or
  haze should make their shape readable without obscuring actors. Broad,
  slower cloud shadows can cross outdoor rooms separately. Existing drifting
  cloud sprites are weather visuals; they do not currently cast shadows.
- [x] Remove the separate 12-tile distance fade from non-player entities.
  Lighting should determine whether an actor is visible. Preserve deliberate
  invisibility or fog effects only where a game rule asks for them.
- [x] Compare dark and lit rooms in the same capture: an unlit room stays dark,
  a canopy opening or campfire has a clear local pool, shafts align with their
  ground patches, and actors/particles use the same light field as the tiles.

## Forest ground art

- [x] Replace the generic forest wall's platformer-style bright top strip and
  repeating brick rows with a minimal top-down wall mass. It must tile in both
  axes; show the contour only where wall meets open space, under the same smooth
  lighting. Keep damage cracks, HP feedback and final broken terrain readable.
  Replaced the repeated ledges with a quiet shared surface and a neighbor-aware,
  corner-lit contour on all exposed sides. Strict build and static normal-camera,
  canopy and damaged-wall captures checked (2026-09-14).

- [x] Simplify the three forest floor textures and grass. Use broad, calm
  color areas with sparse, purposeful marks; remove the repeated high-frequency
  speckles that make every tile equally busy. Preserve negative space around
  players, items, footprints, and attack previews.
- [ ] Put visual variety at room scale with occasional tufts, roots, paths, and
  larger patches rather than random detail in every 16-pixel tile. Check tiled
  repetition, cave/outdoor distinction, and readability under both deep shade
  and canopy light. Keep the source art native to Gauche's simple pixel style.

## Reactive forest props

- [x] Add a light **prop** layer for small world objects over the floor. Props
  are not entities: use compact per-instance state such as cell, kind, HP, and
  broken state, rather than one of the 512 full actor slots. A prop can be
  passable or blocking, with an explicit response to hit, step, or both. Keep
  ordinary visual scatter separate from props that change collision, drop
  loot, or have persistent state.
- [x] Start with a few distinct interactions: dry leaves crunch and scatter
  when stepped on; twigs snap underfoot; ferns or tall grass bend/trample and
  can be cut; mushrooms burst into spores when struck or stepped on; a rotten
  log or crate blocks a tile until attacked. A rare nest or supply crate may
  drop something, but most props should leave only a broken sprite or debris.
- [x] Place props by room role and small clusters, leaving clear paths and calm
  ground between them. Make their silhouette, material, sound, and one-hit or
  low-HP response readable at the 16-pixel scale. Resolve contact once on
  entry, and route weapon hits and blasts through the prop before or alongside
  the underlying tile according to an explicit rule.
- [ ] Save persistent breakage, collision, and loot rolls in deterministic
  floor state; keep flying leaves, dust, spores, and sound playback cosmetic.
  Check co-op rollback/reconnect and ensure props cannot hide critical loot,
  objectives, players, or attack previews.

## Loose debris

- [x] Add a small floor-bound **loose debris** collection, distinct from the
  existing short-lived particles and from props/entities. A piece has a
  material/sprite, sub-tile position, small velocity, and settled state. It
  can rest on the floor for the level rather than disappearing on a timer.
- [x] Breaking a prop releases material-specific pieces: leaves, twigs, wood
  chips, mushroom bits, or similar scraps. Apply local impulses from actor
  steps, blasts, trains, and gentle outdoor wind. Pieces slide or tumble a
  short distance, slow with friction, avoid passing through solid walls, and
  settle again. Draw them above floor tiles but beneath actors, using the same
  lighting as the ground.
- [x] Let debris collect naturally without unlimited sprite growth. Cap loose
  pieces per room or cell and merge older settled pieces into small litter
  piles; a later step can scatter a pile again. Leave clear space around
  objectives, loot, attack previews, and important actor silhouettes.
- [x] Keep motion and pile presentation local while scraps have no collision,
  damage, loot, or AI effect. Deduplicate break events across rollback, clear
  the collection on floor change, and reconstruct a basic settled scatter from
  saved broken-prop state on reconnect. If a future scrap becomes an actual
  pickup or obstacle, synchronize that specific gameplay object.

## UI and pointer

- [x] Start each session with selected and ground item detail cards collapsed.
  Preserve the current control that expands them; inventory and comparison
  should still open full cards when requested.
- [x] Reduce the overall visual footprint of UI by roughly 25–30%: quick
  slots, HP, floor/zoom labels, item cards, reward/shop/inventory text and
  icons, main menu/lobby/settings, and related shadows/offsets. Check actual
  1080p captures for room to see the world and for legibility. Retain generous
  mouse/controller targets even if their artwork shrinks; update pointer hit
  rectangles with the layout.
- [x] Track the most recently used pointing device locally. Any meaningful
  gamepad button, stick, trigger, or D-pad input hides both the OS cursor and
  Gauche's drawn pointer, including in menus and inventory. Mouse movement or
  click brings the pointer back. Prevent stick noise and synthetic mouse motion
  from making it flicker; this state must not enter lockstep input or hashes.
- [ ] Exercise the compact default, expanded detail toggle, inventory/reward
  comparison, and controller-to-mouse switching in menu and gameplay captures.

## Level generation and unique floors

- [x] Replace the fixed five- or six-room corridor and three side rooms with a
  seeded route graph: spawn, exit, required objectives, optional branches,
  occasional loops, secrets, and guarded detours. Use Zelda / Binding of Isaac
  as route-structure inspirations without forcing every node into one box.
- [ ] Build a pool of authored room shapes with matching entrances, varied
  sizes, irregular boundaries, and Barony-like interior complexity. Combine
  authored rooms with procedural connections; allow entire authored floors and
  rare unique layouts. Forest floor one should have distinct places and pacing
  rather than the current five floor-fill variants.
- [ ] Support keys, levers, doors, appearing gate tiles, closing doors, and
  lock-in encounters with clear tells and completion/release rules. Validate
  key-before-lock order and an ordinary solvable route without requiring rare
  digging, explosives, or the train. Handle split co-op parties, death,
  disconnect, and reconnect before a room can seal.
- [ ] Populate by room role and local pools for enemies, props, light, loot,
  and containers, with floor-wide supply and threat budgets. Give enemies and
  each chest/box/container type sensible drop tables. Put calm paths between
  fights and landmarks, keeping the newly simplified floor art visible.
- [ ] Review many seeded floor captures and route summaries for variety,
  readability, viable supplies, and reachable objectives. Extend deterministic
  tests beyond basic reachability to locks, room placement, co-op transitions,
  and damage-created shortcuts.
- [x] Design and build the haunted-house unique: a large outdoor approach with
  trees, grass, perhaps water, and a central mansion fortress. A switch inside
  seals the exit and starts classic Call of Duty Zombies-inspired survival
  waves; a defined completion condition releases the party. Author its layout
  and co-op entry/reconnect behavior deliberately.
- [x] Add the [Freight Exchange](features/FREIGHT_EXCHANGE.md), an authored
  Industry 3-2 floor with two loaded cart lines, connected switch points,
  sorting belt/sleeper, crane salvage, guarded control booth and hoist cache.
  Seeded one-in-five selection at this one stage; explicit dev selection,
  start override and repeat work. Switch/exit route, four-player carryover,
  party exit/reward/shop/progression and snapshot replay checked. This is a
  whole-floor unique, not completion of the multi-room landmark system or
  the Last Shift reactor event; wider quotas remain open.

## Biome content farm

Enemy implementation follows bespoke init/step functions and shared entity
storage. Reusable `counter_a`, `label_a`, timers, and related slots are explicitly
approved (Splonks style); document their meanings per enemy instead of growing
the shared struct with a separate state field for every species.

Also approved: reusable `entity_a`/`entity_b` generation-checked handles and
`point_a`/`point_b` cells, with labels describing their roles. Use these creatively
for patrol routes, territory/home positions, investigation points, remembered
attackers, flock or worm links, and bodyguards interposing between a threat and
their protected ally. Ducklings/chicks following a leader can scatter from a
remembered attacker when attacked. Stale leader/target handles need an intentional
fallback (rejoin a flock, choose another ally, retreat home, or resume wandering).

Enemy idea to implement: **zombie stack**. Render several zombies perched on one
another; killing one topples the surviving stack into individual zombies. A shared
counter can hold the compressed stack population until it splits, avoiding a
special member array on every entity. Define what happens in crowded cells so
toppling preserves survivors without overlapping impassable actors or silently
deleting zombies. This belongs in a fitting haunted/undead encounter pool.

Shared behavior slots are now in the entity struct, gameplay hash and snapshot
codec: two generation-checked handles, two cells, counters, labels and countdowns.
Chicks seek an adult leader; flockmates flee a remembered attacker and retain its
last known position. The zombie stack now has bespoke init/step code, stacked
rendering, a generated falling groan/thump sound, and surviving zombies that tumble
out one at a time. Blocked neighbors or a full entity pool retain unsplit bodies.
It currently appears in late forest undead side rooms; the haunted-house pool will
reuse it. Strict build, existing snapshot codec check and a static stacked/falling
sprite capture pass; behavior balance awaits user playtesting.

The minimum target is **20 genuinely distinct enemies, 20 loose-debris types,
and 50 biome-specific items per biome**. Across four biomes this is at least
80 enemies, 80 debris types, and 200 items, plus a small shared item pool.
Distinct behavior, encounter role, silhouette, sound, and sensible drops matter
more than different colors or HP values. Room, tile, prop, hazard, and container
variety belongs to every biome too. Runtime order is now Forest → Ice → Industrial/mine/lava, with named biome rules, migrated debug selections and matching visuals/stock/populations ([progression notes](features/BIOME_ORDER.md)). Adding the fourth biome remains pending. Preserve cross-biome fire/ice utility as specified in the [content brief](design/CONTENT_FARM_BRIEF.md).

- [x] Design a full forest catalog to those quotas, including enemies with
  tells and drop tables, strange or weak-but-useful items, prop/debris families,
  room roles, tile types, chests/boxes, and new sound requirements.
  See [Forest catalog](content/FOREST.md): 20 adversaries, 50 regional finds,
  20 loose materials, room sources, container pools and sound requirements.
- [ ] Implement, asset, sound, test, and playtest the forest catalog in coherent
  slices until it meets the quotas and works with the new generator.
Forest item slice: fifty-two regional finds are implemented, including the
original fifty-item list and two egg foods. Shared definitions supply icons,
patterns, descriptions, sounds, room/reward/shop pools and rare attributes.
Traveling weapons, digging/route tools, heat/liquids, bait/hearing, traps/roots,
food/remedies, decoys/wards, lanterns/parries, conductive thunder and linked
Pocket Doors have concrete interactions. See the [catalog](content/FOREST.md)
and [implementation notes](engineering/IMPLEMENTATION_DECISIONS.md) for the actual rules.
Natural sap sources and broader forest ecology/generation remain unfinished.

Forest enemy slice: all twenty catalog adversaries have bespoke behavior,
recognizable sprites, tells, drops and room-pool integration. Shared handles
support worm chains, guards, stolen items and finite wasp broods. Forty-three
new enemy cues are integrated. See the [forest catalog](content/FOREST.md) and
implementation notes for per-species rules. Strict builds/static captures pass;
behavior and balance playtesting remain the user's.

- [x] Design a full ice catalog to the same quotas and quality bar.
  See [Ice catalog](content/ICE.md): the drowned observatory, with twenty
  adversaries, fifty regional finds, twenty debris materials, twenty ambient
  cues, container pools and four-floor room progression. Heat, water, footing,
  sound and reflection are shared interactions; implementation remains below.
- [ ] Implement, asset, sound, test, and playtest the ice catalog.
  Fifteen cold room roles host twenty-one enemy types, including [shard colonies](features/ICE_SHARDS.md),
  [Pilgrims](features/ICE_PILGRIM.md), Lens Wardens and [Echo Hounds](features/ICE_HEARING.md). Regional items include
  [observatory optics](features/ICE_OPTICS.md). Twenty-one enemy behaviors (initial twenty plus [territorial Yeti](features/ICE_YETI.md)), fifty of fifty items; [circuit tools](features/ICE_CIRCUITS.md) connect shocks and grounding. [Storm Lantern](features/STORM_LANTERN.md) and [Echo Pebble](features/ICE_ECHO_PEBBLE.md) add directional light and recorded lures; [Harpoon Gun](features/ICE_HARPOON.md) adds traveling shots and held reeling. [Doorstops and sluices](features/ICE_SLUICES.md) add reusable gate jamming and reservoir chambers. [Borrowed Summer](features/BORROWED_SUMMER.md) adds moving warmth with shared thaw/fuel/enemy reactions. [Heat Siphon](features/HEAT_SIPHON.md) transfers finite fire/fuel into reusable flame charges. [Thaw Charges](features/THAW_CHARGE.md) open ice routes with a sealed fuse and dangerous steam. [Folded Bridges](features/FOLDED_BRIDGE.md) add burnable, individually breakable water crossings. [Tuning Forks](features/TUNING_FORK.md) shatter crystal veins and disrupt shard nodes through connected crystal. [Stillwater Bells](features/STILLWATER_BELL.md) calm stream currents, floating cargo and slippery ground. [Effigy Masks](features/EFFIGY_MASK.md) add a stationary rear gaze with finite wear, sharing effigy sight and cover rules. [Ice Anchors](features/ICE_ANCHOR.md) add breakable retreat points and real-cell tether returns. [Snow Shelters](features/SNOW_SHELTER.md) add two-section low cover, shared thawing and overhead throw rules. [Sleds](features/SLED.md) carry riders and loose cargo at real gameplay positions, with stopped steering, braking, recoverable wear and wood/fire interactions. Twenty Ice debris materials are implemented.
- [x] Design a full industrial / mine / lava catalog to the same quotas.
  See [the Underworks catalog](content/INDUSTRIAL.md): working mines and foundries,
  twenty enemies, fifty regional finds, themed containers, eighteen room roles,
  twenty debris materials and twenty ambient sources. These are design targets;
  implementation remains below.
- [ ] Implement, asset, sound, test, and playtest the industrial / mine / lava
  catalog. [Native terrain art](features/INDUSTRIAL_TERRAIN.md) replaces the tinted
  Forest tiles and repeating lava waves. [Mine crews](features/MINE_CREWS.md)
  add Pickhands and Shift Foremen, a dry workfront on each Industrial stage,
  shared digging, coordinated retaliation, seven poses and twelve sounds.
  [Foreman's Whistle](features/FOREMAN_WHISTLE.md) redirects idle workers until their leader calls again, with finite uses and themed drops/supplies/rewards.
  [Ember Stokers](features/EMBER_STOKER.md) replace the old hidden-pistol Ember with finite coal, traveling hot/cold pellets, a scoop recovery and an empty-sack shovel attack.
  [Quarry Charges and Fuse Scissors](features/QUARRY_FUSES.md) add directional stone cutting, dangerous backblast, exact bomb recovery, and shared exposed-fuse water/cold rules.
  [Powder Monkeys](features/POWDER_MONKEY.md) light finite physical charges, flee their backblast, and leave armed charges behind when killed. A reserved blasting alcove and nearby scissors make those interactions available.
  [Press Hammers and Rubber Mallets](features/INDUSTRIAL_HAMMERS.md) add interruptible heavy blows, low-damage shoves, cover breaking and boiler pressure relief. Both wear down and accept resin repairs.
  [Strikebreakers](features/STRIKEBREAKER.md) guard later workfronts with finite frontal shields and delayed shove/hammer combinations. Traveling direct hits now retain shooter attribution separately from impact direction.
  [Industrial grates and bins](features/INDUSTRIAL_GRATES.md) add shoot-through cover, themed container drops and four native loose-debris materials, placed outside protected routes.
  [Rivet Guns and Gunners](features/RIVET_GUNNERS.md) add committed three-round traveling bursts, a braced enemy with a long reload, safe side firing posts, finite player magazines and local ejected casings.
  [Arc Torches](features/ARC_TORCH.md) add finite sustained electrical attacks, metal cutting and shared water/wire/grounding interactions; available through the master Industrial supply table.
  [Conveyors and assembly rooms](features/CONVEYORS.md) add deterministic roller transport, safe stationary crossings, hand-cranked runs, six-second brakes, powered-belt debris drag and positional roller ambience.
  [Arc Welders and repair bays](features/ARC_WELDER.md) add a visor-down warning, committed electrical lane sweep, metal cutting and wet/dry/grounding encounter choices.
  [Horseshoe Magnets](features/HORSESHOE_MAGNET.md) pull real loose steel gear while moving, preserve item state, and connect native scrap-bin drops to hot salvage pockets.
  [Magnet Cranes and scrap yards](features/MAGNET_CRANE.md) add overhead committed grabs, metal-gear targeting, magnet bait, cold/heavy-hit interruption and optional hot sorting lanes.
  [Folding Barricades](features/FOLDING_BARRICADE.md) add atomic three-section lane blocking, narrow-shot/overhead passage, independent panel damage and shared metal-cutting counters.
  [Pressure Rats](features/PRESSURE_RAT.md) add a visible inflation tell, fixed rush, obstacle-triggered steam burst and shared cold/water/control counters.
  [Coolant Cans and cooling works](features/COOLANT_CAN.md) connect quenching, pressure reduction, lava crossings, slippery conductive residue and an alternate maintenance encounter; this also fixes sorting-yard footprints that previously always fell back to a lone crane.
  [Industry ambience and scraps](features/INDUSTRIAL_ATMOSPHERE.md) add seven local/offline cue families, state-dependent machine sounds, nail/chain debris, and an audit of existing copper/coal sources. Ten of twenty catalog debris materials and eight of twenty ambient families are integrated.
  [Cable Crawlers and cable trenches](features/CABLE_CRAWLER.md) add finite wire laying, wall-biased movement, warned pulses through live circuits, severing/grounding counters, and an optional wet/dry encounter.
  [Pocket Drills](features/POCKET_DRILL.md) add sustained stationary cutting, finite battery, real noise and shared wall/prop/wire interactions, with movement/aim reprime and hurt/control cancellation.
  [Walking Kilns and kiln courts](features/WALKING_KILN.md) add warned flame rows, cold interruption, an exposed mouth, finite fuel and actual wood/coal consumption beside optional oil/stock encounters.
  [Tension Springs](features/TENSION_SPRING.md) add stackable underfoot launch pads, real airborne gap crossings and pursuer traps, heat/destruction counters and supplies at optional hot salvage banks.
  [Audit Clerks and pay offices](features/AUDIT_CLERK.md) add real loose-gold collection, finite breakable pay cages, interruptible alarms and existing-worker responses without reinforcements spawning.
  [Emergency Foam](features/EMERGENCY_FOAM.md) adds traveling sealed cans, a visible expansion delay, shared fire/fuse quenching and temporary shot-blocking soft cover, with kiln-court supplies.
  [Furnace Moths and lamp alcoves](features/FURNACE_MOTH.md) add finite heat theft, light lures, warned committed dives, cold/cover counters and paired flame/electric-light scenes. Dropped lit sticks now work with shared heat extraction.
  [Bolt Pouches](features/BOLT_POUCH.md) add stackable five-bolt fans, close-range concentration, grate passage, parries and finite flight. Assembly cargo and scrap bins provide three-handful bundles.
  [Slag Snails and slag banks](features/SLAG_SNAIL.md) add armored crawlers, fixed warned lunges, exposed recovery/cold states, short hot trails and crackable cooled crust beside optional lava-bank supplies.
  [Chain Hooks](features/CHAIN_HOOK.md) add held cargo/prop hauling, safe fixed-anchor pulls, loaded sled transport and scrap-yard supplies.
  [Nail Boards](features/NAIL_BOARD.md) add melee/ground-trap conversion with shared condition, safe recovery, heat/destruction and assembly-belt acquisition.
  [Ash Sleepers and ash lofts](features/ASH_SLEEPER.md) add noise-triggered waking, warned swipes, quiet/wet settling and manual-belt encounters with useful cargo.
  [Hand Bellows](features/HAND_BELLOWS.md) add directional actor/cargo pushes, gas drift, finite flame fanning and ash/debris disturbance; ash lofts can supply them.
  [Counterweights and hoist shafts](features/COUNTERWEIGHT.md) add warned overhead impacts, baitable prop/creature crushing, cold/heavy-hit interruption and persistent cuttable wrecks.
  [Pocket Pumps](features/POCKET_PUMP.md) add finite spill transport, exact partial quantities, shared water/coolant interactions, tank UI and cooling-works supplies.
  [Emergency Pumps](features/EMERGENCY_PUMP.md) add mobile finite-liquid pressure jets, refillable spill collection, warned lane attacks, retained tank salvage and water/oil cooling-works variants.
  [Nozzle Elbows](features/NOZZLE_ELBOW.md) redirect future boiler/pump jets, preserve warned aim, share removable fittings with Pressure Valves, and retain handedness/condition through pickup and salvage.
  [Mold Thieves and casting floors](features/MOLD_THIEF.md) add real loose-metal theft, interruptible sealing, reusable breakable molds, exact cargo salvage and eight-use keys in optional paired workstations.
  [Steam Lances](features/STEAM_LANCE.md) add refillable two-cell scalding thrusts, shared kettle heating/cooling, interruptible bracing and lamp-alcove water/stove supplies.
  [Tar Flasks and material states](features/TAR_FLASK.md) add traveling three-bottle bundles, sticky fuel, cold-hardened crust, washing/pump transfer and tar-lane kiln variants.
  [Tar Choirs and settling tanks](features/TAR_CHOIR.md) add linked warning/cough rhythms, traveling tar, cover/control disruption, surviving solo singers and paired material basins with practical counter-tools.
  [Rail Shunters and freight carts](features/RAIL_SHUNTER.md) add warned shunting, real cargo, single-impact collisions, hook hauling, cart brakes, cuttable freight track and optional loaded sidings. Generated-floor checks also fixed [scrap-yard snapshot rejection](engineering/SCRAP_YARD_SNAPSHOT.md).
  [Rail Switch Keys and points](features/RAIL_POINTS.md) add redirectable T sidings, breakable selected exits, directional rail drawing and contextual key supplies.
  [Insulated Boots](features/INSULATED_BOOTS.md) add temporary floor-conduction immunity with slower steps, explicit direct-arc vulnerability, a saved HUD timer, contextual cable-trench supply and crawler drops.
  [Glow Slag](features/GLOW_SLAG.md) adds recoverable thrown light/heat, finite reheating, moth/siphon theft, cold/water quenching and lamp-alcove acquisition.
  All twenty initial catalog enemy behaviors and twenty-seven of fifty regional items implemented;
  remaining content slices and player feedback are pending.
- [ ] Choose the fourth biome's identity. Military / robot is a candidate,
  not a locked decision; design its full catalog to the same quotas.
- [ ] Implement, asset, sound, test, and playtest the fourth biome catalog.
- [ ] Make items interact with the world in distinctive ways. Keep room for a
  throwable, recoverable rock; a lighter that needs oil or flammable sap;
  strong weapons that change routes; and intentionally odd finds. Avoid a
  spreadsheet of 200 reskins.

## Audio, gun aftermath, and economy

- [ ] Build an offline sound-making workflow, including Python/LFSR-like
  synthesis where useful and other methods when they fit better. Produce
  actual new audio files for enemies, weapons, props, tile hits, drops,
  hazards, doors, unique rooms, and UI feedback; use variations for repeated
  cues and positional playback for world sounds.
- [x] Add gun-specific firing aftermath: ejected casings, impact fragments,
  impulses on nearby loose debris, distinct reload sounds, and clear dry-fire
  feedback when a magazine is empty. Casings and scraps are local cosmetics.
- [ ] Put tangible gold/currency finds into rooms, containers, and fitting
  enemy drop tables; tune occasional between-floor shops and biome-aware stock
  around what players can actually find.
- [ ] Playtest the core item and room interactions before settling perks,
  artifacts, and three-choice rewards. Decide how much of the current reward
  system remains; artifacts may become central or be cut if the game plays
  better without them.

Currency implementation: collectible gold piles now appear in room stashes,
crates, pots, and zombie pockets. Animals use explicit meat drop chances; the
old automatic five-coins-per-kill rule is removed. Walking over gold collects it
for that player, with a new coin sound and a compact HUD total. Floor stashes
budget roughly 18–36 gold before incidental finds. Existing occasional shops
remain; later biome stock and balance still await the catalog/playtesting pass.
Strict build and a static HUD/pickup capture checked. Damage and enemy loot now
live in their own domains instead of the item-use file.

Palette note: no palette, GPL/PAL, ASE or Aseprite source file was found in the
current `gauche-rs` checkout, including ignored files. Existing PNG colors remain
the available art reference; new source scripts keep a small explicit palette.

## Music and musical cues

Direction and local reference inventory: [music brief](music/MUSIC_DIRECTION.md).

- [x] Initial music loudness fix: measured the old assets and lowered the runtime mix from 0.4125 to 0.12 (about 10.7 dB) in playback/settings paths, preserving saved sliders. Human mix feedback remains part of soundtrack iteration; see [first previews](music/MUSIC_SKETCHES.md).
- [x] Make original Forest timbral/composition sketches: two Forest directions plus an Industry drum/horn sketch are available on the [first listening page](../assets/music/sketches/index.html). The user's listens found these too cheesy/melodic and the sampled strings too artificial. Preserve them; [the revised brief](music/MUSIC_DIRECTION.md) calls for negative space, held gestures, material percussion and stranger feedback timbres.
- [x] Make a broader second comparison pass from scratch: nine atmosphere/title/threat studies and six title/entry/clear/win/loss/shop cues, with a 2:51 comparison reel and [listening gallery](../assets/music/atmosphere/index.html). Custom feedback/material synthesis, editable scores and encoded checks are included. These remain previews awaiting listens, not completed soundtrack quotas or installed playback; see [pass 02](music/ATMOSPHERE_STUDIES.md).
- [x] Respond to pass-02 listens: preserve the approved Shop Arrives cue; treat the other pipe studies as unapproved material for one Ice song only. [Pass 03](../tools/music/contrasts/README.md) supplies that 3:30 arrangement candidate plus four sketches with different instrument/rhythm families, awaiting listens. Stop music iteration here for now and return to game content as requested.
- [ ] Integrate the approved `assets/music/atmosphere/shop_arrives.ogg` on shop arrival, with local presentation-state deduplication and appropriate music handoff; preserve the approved audio unchanged.
- [ ] Build a reproducible offline composition/instrument/render workflow; MIDI, text notation or composition scripts are all optional choices. Develop changing passages, distinctive biome character and seamless loop tails, with deep/mid/high voices (including cello candidates), preserved dynamic range and Industry drum/horn candidates. Install suitable synthesis/audio tools as needed.
- [ ] Author a few exploration songs per biome, normally about 3:30 each; initial planning target three per biome. Forest starts from the requested Fable-like woodland mood; avoid generic space ambience and unchanged short-pattern repetition.
- [ ] Develop shorter boss-loop candidates (initial proposal one per biome), main-menu music, and level-start, level-win and shop-arrival jingles with appropriate intensity and related motifs.
- [ ] Integrate local biome/boss/menu music selection, repeat avoidance, fades, cue handoffs/ducking, settings, pause/reconnect behavior and per-asset mix gains; keep music outside deterministic gameplay state. Validate actual exported loops and audible quality before marking the music pass complete.

## Ambient sound and environmental scenes

- [x] Inspect Splonks' ambient audio and reuse suitable loading/mixing pieces;
  give Gauche separate, simple ownership for persistent loops, area triggers,
  and occasional level-wide ambience rather than stuffing them into enemy AI.
- [ ] Author **at least 20 distinct ambient sound sources/cues per biome**
  (80 across four), with thematic assets, placement rules, volume and falloff.
  Include continuous water/wind/machinery, occasional wildlife/distant events,
  and unsettling one-shot creaks, screams or other discoveries. This is an
  additional quota: Forest and [Ice](features/ICE_AMBIENCE.md) each have twenty implemented cues; other biomes remain.
- [x] Add positional looping sources with per-source gain, near/far falloff,
  smooth enter/exit fades, sensible voice limits and listener-aware mixing.
- [x] Add point/area triggers: enter once, optionally rearm after leaving or a
  cooldown; allow delayed/distant responses such as a creak or scream.
- [x] Add a per-level list of occasional ambient events, each with a local
  random chance, cooldown and sound variations. Frame-rate-independent timing;
  avoid replay bursts, immediate repetition and overlapping loud cues.
- [ ] Author small environmental scenes inside room/prefab pools: wall-fed
  stream/waterfall into shallow pools, wind through a broken wall, creaking
  ruin, etc. Combine terrain/decor, local audio and local visual effects.
- [x] Shallow-water steps produce expanding puddle rings and splashes, with
  appropriate water footsteps; streams/ripples stay restrained and readable.
- [ ] Keep ambient playback, random schedules, trigger-consumed flags and
  ripples in local presentation state, outside snapshots/gameplay hashes.
  Scene placement may come from the generated map; collision, depth, damage
  or other water/terrain rules remain deterministic gameplay. Reconnect should
  rebuild nearby loops without replaying every one-shot trigger.

## Playtest feedback: status, contact and debug visibility

- [x] Show named player status indicators with remaining timers. Distinguish
  sleep, stun, chill/freeze and burning; show burn rate and remaining damage,
  and explain what each status stops or slows. Keep them compact but legible.
- [x] Replace repeated blocked-walk beeps with a restrained first-contact bonk;
  suppress repeats while holding into the same obstruction. Distinguish wood
  and hard surfaces where appropriate; remove impact particles from mere bumps.
- [x] A blocked movement attempt still turns aim unless explicit aiming from
  the right stick, keyboard aim or mouse overrides it.
- [x] Inspect campfire flame anchors/layers: wisps should rise from the burning
  top, not below the base. Preserve correct actors-over-ground-fixtures order.
- [x] Keep item inspection patterns. Default player/enemy world attack grids
  off; expose independent categorized ImGui debug checkboxes for them.
- [x] Inspect and follow Splonks' F1 global ImGui visibility and F2 debug-window
  selector behavior. Local debug state must not alter gameplay/network state.

## Playtest feedback: stacks, uses and cooking

- [x] Make ammo packs stack and merge matching pickups into a carried partial
  stack before taking another slot. Preserve independent ammo reserves per gun.
- [x] Stop showing `x1` on non-stackable equipment, including fists. Clearly
  label stackability in item details; distinguish stack count/max, remaining
  uses, durability and magazine/reserve. A ten-use lighter is one tool, not a
  ten-object stack, and used tools must not merge or refill one another.
- [x] Inspect repeated campfire cooking on held use: cook one portion per beat,
  add a recognizable sizzle, preserve raw-to-cooked counts and inventory capacity.
  Keep discovery hidden for now per the user's later preference; an interaction
  popup (meat -> cooked meat/pan icon) is an optional future direction, not a
  requirement to add tutorial prompts everywhere.

## Playtest feedback: creatures, melee and forest visibility

- [x] Give cooked meat a distinct munch/chew cue, separate from bandages.
- [x] Add dropped meat as bait for appropriate hungry animals: bounded scent
  range, reachable targets, competing threats and consumption. Avoid every
  animal magically knowing about every piece of meat across the floor.
- [x] Rework chicken families into generation-checked follow chains: first
  chick follows mother, others follow the preceding chick. Follow previous
  positions with a small delay; yield down blocked chains to free the mother. Make
  frightened chicks visibly scurry with fast little steps and animation.
- [x] Give mothers a protective response when their chicks are attacked;
  survivors remember the attacker and do not immediately wander back to danger.
  Reuse the chain-following helpers for suitable later creatures.
- [x] Walkable puddles/streams extinguish burning.
- [ ] Define which water washes off poison/residue when those statuses and
  materials are implemented.
- [x] Replace forest's washed-out overhead cloud veil with fixed world-anchored
  canopy silhouettes and camera parallax. Retain slow cloud effects where they
  fit (especially industry/lava); use a large soft/dithered central cutout so
  overhead foliage/clouds preserve the play area. Keep ground shadows separate.
- [ ] Improve creature identity/readability; the user could not identify a
  round pursuing creature with a delayed attack/vocalization. Verify the sprite
  and behavior before identifying it; preserve that dodgeable attack timing.
- [x] Redesign bow input around hold-to-draw/release-to-fire, taking Splonks'
  bow feel as reference. Eliminate the manual magazine-style bow reload; add
  audible draw, tension/release and impact, with visible traveling arrows.
- [x] Replace instant-use bomb explosions with thrown travel, a landing cue,
  visible burning fuse/sparks, audible fuse and delayed detonation. Give the
  thrower time to escape; resolve the blast at the bomb's final position. Keep
  fuse/travel deterministic and document timing/radius in its item details.
- [ ] Separate instant-hit cosmetic tracers from deterministic traveling or
  persistent projectiles (arrows, grenades, spells). Travel/hit timing must match
  damage timing; arrows splinter into local debris on impact. Retain explicit
  ammo use, charge cancellation and safe behavior on switching/death/reconnect.
  Arrows, bolts, rockets and fused bombs now travel; gun tracers are local.
  Persistent spells and the remaining regional throws still belong to their content slices.
- [x] Add brief item-specific melee windups and swing/exertion sounds: fists
  very quick, sticks/heavier tools longer. Ordinary guns fire immediately;
  launchers/throws may have a deliberate preparation beat where appropriate.
  Store pending attacks deterministically, commit aim/pattern for resolution,
  consume resources once, and handle interrupts/item switches/death explicitly.
  Show windup alongside cooldown in descriptions; keep combat responsive.

Forest ambience: 20 generated cues, dedicated loop/event voices and local
schedules. Build checked; shallow-water scenes and later biomes remain.
## Playtest feedback: camera, controls, menus and world detail

- [x] Fix locks bypassed by ordinary walking; verify locked door separates entry from exit.

- [x] Smooth the camera guide while actors snap to their authoritative tiles;
  keep camera motion continuous across simulation ticks, frame rates, stops
  and direction changes without delaying bodies, held items or attack origins.
- [x] Smooth the camera staircase during alternating diagonal input; preserve actor tile truth.
- [x] Alternate cardinal steps while diagonal movement is held, as in Adventures
  with Chickens; preserve explicit aim overrides and deterministic movement.
- [x] Lower campfire flame anchors slightly so their bottom overlaps the upper
  half of the wood sprite. Preserve the flame/base/actor ordering deliberately.
- [x] Give rockets real travel; apply delayed impact to future weapons that need
  flight. Keep deliberate instant-hit guns, with visible muzzle flashes/tracers.
- [x] Replace the axe-like pickaxe icon with a recognizable pickaxe. Rotate the
  fist artwork clockwise so neutral/right-facing use reads as a forward punch.
- [x] Fix left-facing held art without upside-down weapons: mirror around the
  appropriate sprite axis/center. Verify all four directions and held enemy items.
- [x] Add wood/tree terrain with recognizable material and break rules; forest
  obstacles should include actual trees/wood, not only nondescript green walls.
- [x] Make the rake sweep loose leaves/debris into useful-looking piles locally;
  greatly increase its available uses. Keep its current recognizable silhouette.
- [x] Make chicken nests drop eggs rather than raw meat; integrate eggs as finds
  with sensible stack/use/cooking rules and fitting art/sound.
- [x] Support igniting a held stick into a temporary burning weapon, initially
  about 30 seconds; spread fire to susceptible actors/materials and communicate
  its remaining burn time. Keep item state deterministic through drop/swap.
- [x] Diagnose visible tile-center/vertex bias in lighting using canopy-on/off
  captures against Splonks. Preserve intentional dappled patterns; fix unwanted
  interpolation seams/facets without flattening the dramatic contrast.
- [x] Contextual pickup/drop on the configured interact button (Xbox X by default):
  empty ground drops the selected droppable item; a pickup merges/fills a free
  slot; full inventory swaps with selected equipment if legal. Preserve counts,
  cooldowns, uses and attributes atomically; blocked/cursed drops must fail safely.
- [ ] Sweep every HUD, inventory, interlude and menu hint for active-device input:
  keyboard keycaps only in keyboard mode; controller glyphs only in pad mode.
  Resolve prompts from actual bindings, not hardcoded default letters.
- [ ] Detect SDL controller layout (Xbox, PlayStation, Nintendo), including the
  active mode reported by third-party pads. Show recognizable button glyphs and
  Xbox colors; keyboard letters sit on small offset keycaps. Handle device changes.
- [x] Hide the OS mouse whenever Gauche draws its own pointer. Render the custom
  pointer above menus/configuration panels as well as gameplay; preserve pad hiding.
- [ ] Give all menus a usability/layout pass: consistent small bottom-left Back,
  primary actions apart from lists, Create Profile above/right of the profile list,
  Quickplay above Play, readable controller settings with relevant help text.
- [x] Default bindings are read-only. Create editable profiles from defaults;
  save profile-name edits automatically on blur. Show the active assigned profile
  and allow assigning/selecting it from the profile list, with clear local-player scope.
- [x] Preview bindings beside profile selection/editing, preferably a labeled pad
  or keyboard diagram; include a compact controls reference beside the pause menu.
- [x] Add restrained menu focus/hover, activate, change and back sounds; avoid
  repeated hover spam and ensure keyboard/controller changes receive feedback too.
- [x] Fix the enabled FPS display reporting zero and its overlap with the floor HUD.

## Playtest feedback: canopy, choices and enemy presence

- [x] Make forest canopies greener and more opaque while keeping the large
  dithered viewing mask and anchored parallax. Avoid dark smoke-like foliage.
- [x] Reduce the now-distracting canopy to roughly 20% of its current brightness;
  preserve the green silhouette, anchored parallax and dithered center mask.
- [x] Clarify pick-three selection: the angled red banner must consistently mark
  the focused choice, with stronger focus animation and a clear choose prompt.
- [x] Expose owned artifacts and their effects in the HUD/inventory.
- [x] Let adult chickens survive about four ordinary punches; investigate mothers
  getting stuck despite an open route, including repeated blocked direction choices.
- [x] Make mimics substantially tougher and frightening when revealed. Preserve
  enemies worth avoiding until prepared, alongside nuisance/flavor enemies.

## Suggested order

- [ ] Prioritize [internet multiplayer playtesting](engineering/MULTIPLAYER_PLAYTESTING.md): Splonks/Gubsy room discovery, punch/relay fallback, party lifecycle, saved desync evidence, FPS-independent transport timing and friendly-fire ON by default with a lobby opt-out.
Then finish active playtest fixes and biome content. Keep Rust as the parity
reference and current C++ lighting as the visual baseline. The user owns live
playtesting; use builds and focused static captures for implementation checks.
## Playtest tools and stage presentation

- [x] Implement the [playtest tools specification](guides/PLAYTEST_TOOLS.md): ImGui level/unique selection, saved temporary start override, independent repeat toggle, immediate/spawn loadout editor with actual item options and progression presets.
- [x] Controller icon override in settings; set this user’s preference to Xbox.
- [x] Small upper-right angled stage announcement, subtle entrance and fade.

- [x] [HUD and frame pacing](engineering/HUD_FRAME_PACING.md): shared angled slot/HP panels, exclusive game cursor, high refresh rendering with 60 Hz simulation. Desktop flicker/144 Hz feel awaits user feedback.

### Multiplayer recovery and local layout feedback (2026-09-15)

- [x] Fix false resync when host corrections extend beyond the client's simulated
  tick. Queue the future tail; verify any overlapping snapshot baseline.
- [x] Order canonical updates by timeline revision, reliably acknowledge/retry
  correction bundles, coalesce redundant input replay, and prevent recovery
  snapshots from being constantly replaced by long corrections.
- [x] Use measured network RTT for client prediction lead; share catch-up behavior
  between visible and headless clients. Retain true gameplay positions when drawing.
- [x] Add recovery reasons and periodic tick/RTT/traffic progress to saved netlogs.
- [x] Default local launcher: human workspace 3, three full-width bot windows stacked
  vertically on workspace 4. Verify i3 placement; preserve 16:9 game rendering.
- [ ] Human recheck with the launcher and Japan–Houston internet playtesting after
  every peer rebuilds to wire version 13. Automated lossy four-player sync is not
  a substitute for judging movement responsiveness.

### Water and owl feedback repairs (2026-09-15)

- [x] Poured water uses the same quench path as cold flasks: campfire fuel state,
  light, cooking and contact ignition stop together; burning actors, held sticks,
  candles and stoves in the poured cells are quenched too. Add one douse/steam cue.
- [x] Held utility use is no longer intercepted by campfire cooking from the pack.
  Explicit fixture interaction still cooks carried ingredients; fists/melee and
  selected ingredients retain their familiar cooking shortcut.
- [x] A returning owl notices nearby reachable seed before flying home. Committed
  attacks, landing recovery and fright still take priority. Seed behavior and
  quenched state passed a focused direct-call/snapshot check; no live playtest.
