# Gauche master task list

This is the **single live checklist**. [Content brief](CONTENT_FARM_BRIEF.md): scale/constraints; [forest ideas](FOREST_CONTENT_IDEAS.md): candidates; [NEXT_TASKS.md](NEXT_TASKS.md): history. Preserve dark, readable lighting.

Execution started 2026-09-14. The user owns playtesting and balance feedback. Use builds and focused render/asset inspection, without large new test suites or autonomous playthroughs. Unchecked work stays visible; note uninspected visuals.
- [ ] Implement [conditions and world feedback](CONDITIONS_AND_WORLD_FEEDBACK.md): water/campfire and owl bait fixes; ~50 conditions and useful splints; ice momentum/cold-water rescue; excavation crews and shoot-through barriers; optics/dev-mode gate; compound objectives, escape hazards, pursuer and team-pause policy. Modifier counts remain exploratory.
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
variety belongs to every biome too. Runtime order is now Forest → Ice → Industrial/mine/lava, with named biome rules, migrated debug selections and matching visuals/stock/populations ([progression notes](history/BIOME_ORDER.md)). Adding the fourth biome remains pending. Preserve cross-biome fire/ice utility as specified in the [content brief](CONTENT_FARM_BRIEF.md).

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
and [implementation notes](IMPLEMENTATION_DECISIONS.md) for the actual rules.
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
  Fifteen cold room roles host twenty enemy types, including [shard colonies](history/ICE_SHARDS.md),
  [Pilgrims](history/ICE_PILGRIM.md), Lens Wardens and [Echo Hounds](history/ICE_HEARING.md). Regional items include
  [observatory optics](history/ICE_OPTICS.md). Twenty of twenty enemy behaviors, fifty of fifty items; [circuit tools](history/ICE_CIRCUITS.md) connect shocks and grounding. [Storm Lantern](history/STORM_LANTERN.md) and [Echo Pebble](history/ICE_ECHO_PEBBLE.md) add directional light and recorded lures; [Harpoon Gun](history/ICE_HARPOON.md) adds traveling shots and held reeling. [Doorstops and sluices](history/ICE_SLUICES.md) add reusable gate jamming and reservoir chambers. [Borrowed Summer](history/BORROWED_SUMMER.md) adds moving warmth with shared thaw/fuel/enemy reactions. [Heat Siphon](history/HEAT_SIPHON.md) transfers finite fire/fuel into reusable flame charges. [Thaw Charges](history/THAW_CHARGE.md) open ice routes with a sealed fuse and dangerous steam. [Folded Bridges](history/FOLDED_BRIDGE.md) add burnable, individually breakable water crossings. [Tuning Forks](history/TUNING_FORK.md) shatter crystal veins and disrupt shard nodes through connected crystal. [Stillwater Bells](history/STILLWATER_BELL.md) calm stream currents, floating cargo and slippery ground. [Effigy Masks](history/EFFIGY_MASK.md) add a stationary rear gaze with finite wear, sharing effigy sight and cover rules. [Ice Anchors](history/ICE_ANCHOR.md) add breakable retreat points and real-cell tether returns. [Snow Shelters](history/SNOW_SHELTER.md) add two-section low cover, shared thawing and overhead throw rules. [Sleds](history/SLED.md) carry riders and loose cargo at real gameplay positions, with stopped steering, braking, recoverable wear and wood/fire interactions. Twenty Ice debris materials are implemented.
- [x] Design a full industrial / mine / lava catalog to the same quotas.
  See [the Underworks catalog](content/INDUSTRIAL.md): working mines and foundries,
  twenty enemies, fifty regional finds, themed containers, eighteen room roles,
  twenty debris materials and twenty ambient sources. These are design targets;
  implementation remains below.
- [ ] Implement, asset, sound, test, and playtest the industrial / mine / lava
  catalog.
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

## Ambient sound and environmental scenes

- [x] Inspect Splonks' ambient audio and reuse suitable loading/mixing pieces;
  give Gauche separate, simple ownership for persistent loops, area triggers,
  and occasional level-wide ambience rather than stuffing them into enemy AI.
- [ ] Author **at least 20 distinct ambient sound sources/cues per biome**
  (80 across four), with thematic assets, placement rules, volume and falloff.
  Include continuous water/wind/machinery, occasional wildlife/distant events,
  and unsettling one-shot creaks, screams or other discoveries. This is an
  additional quota: Forest and [Ice](history/ICE_AMBIENCE.md) each have twenty implemented cues; other biomes remain.
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

- [ ] Prioritize [internet multiplayer playtesting](MULTIPLAYER_PLAYTESTING.md): Splonks/Gubsy room discovery, punch/relay fallback, party lifecycle, saved desync evidence, FPS-independent transport timing and friendly-fire ON by default with a lobby opt-out.
Then finish active playtest fixes and biome content. Keep Rust as the parity
reference and current C++ lighting as the visual baseline. The user owns live
playtesting; use builds and focused static captures for implementation checks.
## Playtest tools and stage presentation

- [x] Implement the [playtest tools specification](PLAYTEST_TOOLS.md): ImGui level/unique selection, saved temporary start override, independent repeat toggle, immediate/spawn loadout editor with actual item options and progression presets.
- [x] Controller icon override in settings; set this user’s preference to Xbox.
- [x] Small upper-right angled stage announcement, subtle entrance and fade.

- [x] [HUD and frame pacing](history/HUD_FRAME_PACING.md): shared angled slot/HP panels, exclusive game cursor, high refresh rendering with 60 Hz simulation. Desktop flicker/144 Hz feel awaits user feedback.

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
