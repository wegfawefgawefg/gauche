# Gauche master task list

This is the **single live checklist** for agreed work after the playable C++
port. Keep task status here. The [content brief](CONTENT_FARM_BRIEF.md) records
the new scale and design constraints; the [forest idea pool](FOREST_CONTENT_IDEAS.md)
contains candidate content. The [old item/UI/lighting checklist](NEXT_TASKS.md)
is completed history. The new lighting contrast is the visual starting point;
preserve its dark, readable rooms.

Execution started 2026-09-14. The user owns playtesting and balance feedback;
this implementation pass uses builds and focused render/asset inspection, without
adding a large test suite. Validation/playtest bullets below describe desired
coverage, not a requirement to run autonomous playthroughs. Unchecked work stays
visible until implemented; visual changes awaiting inspection are noted explicitly.

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

Rust Gauche did not have a sequence of cracked wall sprites: damaged breakable
tiles had a health bar and shake, hits threw debris, and a destroyed wall became
Ruin. Before this pass the C++ port let any damage lower wall HP and drew a dark red
rectangle over damaged walls. The new implementation replaces this with shared
branching cracks, a lit HP bar, and explicit impact events for shake/fragments.
Strict release build and the existing snapshot codec check pass, including nondefault
wall HP/dig thresholds and fire state. A static 1080p terrain/fire scene was inspected
with shared cracks, damage bars, ash, smoke, flames and footprints. Reconnect and
gameplay feedback remain with the user; the master goal remains in progress.

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
  Footsteps already spawn for players and zombies, but at 12% opacity and then
  get darkened by tile lighting. Tune size, fade, and brightness for the forest
  floor, keeping them subtle and cosmetic.
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
- [ ] Shape forest sunlight into dappled, irregular patches rather than plain
  circular pools. Use small authored grayscale canopy silhouettes as masks at
  the game's pixel scale, with gentle, slow movement. The same projected light
  pattern must affect floor, walls, and actors standing within it; keep dark
  negative space between patches.
- [ ] Add a few translucent shafts above the world where sunlight enters.
  Their bright ends should meet the projected ground patches, and foliage or
  haze should make their shape readable without obscuring actors. Broad,
  slower cloud shadows can cross outdoor rooms separately. Existing drifting
  cloud sprites are weather visuals; they do not currently cast shadows.
- [x] Remove the separate 12-tile distance fade from non-player entities.
  Lighting should determine whether an actor is visible. Preserve deliberate
  invisibility or fog effects only where a game rule asks for them.
- [ ] Compare dark and lit rooms in the same capture: an unlit room stays dark,
  a canopy opening or campfire has a clear local pool, shafts align with their
  ground patches, and actors/particles use the same light field as the tiles.

## Forest ground art

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
- [ ] Place props by room role and small clusters, leaving clear paths and calm
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

Current implementation: nine forest prop kinds, twenty forest scrap silhouettes,
local spatial buckets, step/blast/train/wind impulses, axis-wise terrain collision,
4096-piece budget, per-cell cap and settled piles that scatter on contact. Broken
props and partial HP round-trip in snapshots; reconnect reconstructs settled litter.
Static 1080p prop/debris capture inspected. Placement is still the interim room-edge
pass; room-role pools and supply budgets belong to the generator milestone.

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

UI implementation: 0.72 scale for HUD and all modal/menu geometry. Inventory and
rewards render to a larger intermediate canvas so the smaller text keeps its strokes;
click regions use the inverse modal transform. Static 1080p HUD/inventory/reward
and a one-frame lobby capture inspected. Controller cursor switching is implemented;
physical controller feel and click feedback await user playtesting.

## Level generation and unique floors

- [ ] Replace the fixed five- or six-room corridor and three side rooms with a
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
- [ ] Design and build the haunted-house unique: a large outdoor approach with
  trees, grass, perhaps water, and a central mansion fortress. A switch inside
  seals the exit and starts classic Call of Duty Zombies-inspired survival
  waves; a defined completion condition releases the party. Author its layout
  and co-op entry/reconnect behavior deliberately.

## Biome content farm

Enemy implementation follows bespoke init/step functions and shared entity
storage. Reusable `counter_a`, `label_a`, timers, and related slots are explicitly
approved (Splonks style); document their meanings per enemy instead of growing
the shared struct with a separate state field for every species.

The minimum target is **20 genuinely distinct enemies, 20 loose-debris types,
and 50 biome-specific items per biome**. Across four biomes this is at least
80 enemies, 80 debris types, and 200 items, plus a small shared item pool.
Distinct behavior, encounter role, silhouette, sound, and sensible drops matter
more than different colors or HP values. Room, tile, prop, hazard, and container
variety belongs to every biome too.

- [ ] Design a full forest catalog to those quotas, including enemies with
  tells and drop tables, strange or weak-but-useful items, prop/debris families,
  room roles, tile types, chests/boxes, and new sound requirements.
- [ ] Implement, asset, sound, test, and playtest the forest catalog in coherent
  slices until it meets the quotas and works with the new generator.
- [ ] Design a full ice catalog to the same quotas and quality bar.
- [ ] Implement, asset, sound, test, and playtest the ice catalog.
- [ ] Design a full industrial / mine / lava catalog to the same quotas; settle
  that biome's blend as the catalog takes shape.
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
- [ ] Add gun-specific firing aftermath: ejected casings, impact fragments,
  impulses on nearby loose debris, distinct reload sounds, and clear dry-fire
  feedback when a magazine is empty. Casings and scraps are local cosmetics.
- [ ] Put tangible gold/currency finds into rooms, containers, and fitting
  enemy drop tables; tune occasional between-floor shops and biome-aware stock
  around what players can actually find.
- [ ] Playtest the core item and room interactions before settling perks,
  artifacts, and three-choice rewards. Decide how much of the current reward
  system remains; artifacts may become central or be cut if the game plays
  better without them.

## Suggested order

1. Tile rules and deterministic tests, then tile impact visuals.
2. Campfire state/contact rules and tests, then rendering, particles, and audio.
3. Forest ground simplification, reactive props and loose debris, then canopy
   lighting, cloud shadows, distance-fade removal, and footprint visibility.
4. Compact UI default, UI sizing, pointer device switching, and captures.
5. Route graph and authored forest room pool, then forest content slices and
   contextual loot, sound, and unique floors.
6. Extend each later biome through the same design, implementation, and
   playtest loop; decide the fourth biome and the reward/artifact direction
   from complete runs.

Use the Rust tile behavior as a parity reference and the current C++ lighting
as the visual baseline. Review each visible change in an SDL capture at normal
zoom before calling the task complete.
