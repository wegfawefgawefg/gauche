# Forest presentation and tile rules

Planning only. No gameplay, renderer, input, or UI change is part of this document.
The new lighting contrast is the starting point; preserve its dark, readable rooms.

## Tile rules and damage feedback

- [ ] Give tiles one explicit break rule: **unbreakable**, **damageable**, or
  **dig-required**. A dig-required tile also has a minimum dig power; a
  damageable tile needs only ordinary damage. Keep current/max HP for anything
  that can break. This avoids contradictory `diggable` and `unbreakable` flags.
- [ ] Give items an optional numeric dig power (zero when absent). Damage still
  determines how quickly an eligible tile loses HP; dig power determines
  whether the hit can start digging. A fist and ordinary gun have no dig power;
  a pickaxe does. Make generated forest walls dig-required and map boundaries
  unbreakable. Reserve damageable for boxes, weak barriers, and suitable built
  barricades. Define bombs, rockets, and the train's wall-cutting exception
  explicitly so their behavior does not depend on incidental attack damage.
- [ ] Route melee, shots, blasts, and train impacts through the same tile-rule
  decision. A rejected hit must leave tile HP unchanged and give clear hard-hit
  feedback. Keep objective rooms and exits reachable with available tools.
- [ ] Replace the C++ wall's flat red damage rectangle with the Rust feel:
  impact shake, material debris, hit/break sound, a small HP bar while damaged,
  and a final ruin tile. Add a shared crack or chip overlay that grows with lost
  HP and is tinted per material, if the bar alone is too subtle. Do not require
  separate half-broken art for every tile. Check visibility under colored light.
- [ ] Cover all three break rules, dig thresholds, explosions/train exceptions,
  tile HP, and tile state through rollback, snapshot, and reconnect checks.

Rust Gauche did not have a sequence of cracked wall sprites: damaged breakable
tiles had a health bar and shake, hits threw debris, and a destroyed wall became
Ruin. The C++ port currently lets any damage lower wall HP and draws a dark red
rectangle over damaged walls.

## Campfires and movement effects

- [ ] Add distinct small flame and smoke particles above an active campfire.
  C++ already emits smoke every 12 ticks, but it is faint; tune its opacity and
  scale. Let flame particles read brightly in a dark room without turning the
  entire room into a uniform glow.
- [ ] Resolve campfire contact **once when an actor enters its cell**, not each
  stationary simulation tick. A susceptible actor catches fire for about five
  seconds and takes about 20 damage over that duration. Keep this deterministic
  and separate its strength from existing Ember and lava burns so their balance
  does not change by accident. Decide refresh behavior when entering another
  fire; repeated contact should not multiply damage unpredictably.
- [ ] On ignition, play an appropriately panicked scream in short, controlled
  bursts and attach flame/smoke particles to the burning actor as it moves.
  Audition the existing `ape_scream.ogg` before using it; provide a fitting
  source sound if it does not work. Particle timing remains local cosmetics;
  burn duration, damage, and fire state belong in deterministic game state.
- [ ] Count one trample per entry into a lit campfire. Each trample plays a
  distinct sound and briefly lowers the fire's own light strength; it recovers
  unless the fifth trample extinguishes it. Show an extinguished state, stop
  flame/smoke and light emission, and stop campfire cooking once it is out.
- [ ] Draw the campfire base below actors and its flame effects above them, so
  a player on the same tile stays visible. Use intentional world layers rather
  than entity-slot order. Check items, traps, fire, actors, and held items when
  they overlap.
- [ ] Restore visible left/right footstep sprites behind walking actors.
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
- [ ] Remove the separate 12-tile distance fade from non-player entities.
  Lighting should determine whether an actor is visible. Preserve deliberate
  invisibility or fog effects only where a game rule asks for them.
- [ ] Compare dark and lit rooms in the same capture: an unlit room stays dark,
  a canopy opening or campfire has a clear local pool, shafts align with their
  ground patches, and actors/particles use the same light field as the tiles.

## Forest ground art

- [ ] Simplify the three forest floor textures and grass. Use broad, calm
  color areas with sparse, purposeful marks; remove the repeated high-frequency
  speckles that make every tile equally busy. Preserve negative space around
  players, items, footprints, and attack previews.
- [ ] Put visual variety at room scale with occasional tufts, roots, paths, and
  larger patches rather than random detail in every 16-pixel tile. Check tiled
  repetition, cave/outdoor distinction, and readability under both deep shade
  and canopy light. Keep the source art native to Gauche's simple pixel style.

## Reactive forest props

- [ ] Add a light **prop** layer for small world objects over the floor. Props
  are not entities: use compact per-instance state such as cell, kind, HP, and
  broken state, rather than one of the 512 full actor slots. A prop can be
  passable or blocking, with an explicit response to hit, step, or both. Keep
  ordinary visual scatter separate from props that change collision, drop
  loot, or have persistent state.
- [ ] Start with a few distinct interactions: dry leaves crunch and scatter
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

- [ ] Add a small floor-bound **loose debris** collection, distinct from the
  existing short-lived particles and from props/entities. A piece has a
  material/sprite, sub-tile position, small velocity, and settled state. It
  can rest on the floor for the level rather than disappearing on a timer.
- [ ] Breaking a prop releases material-specific pieces: leaves, twigs, wood
  chips, mushroom bits, or similar scraps. Apply local impulses from actor
  steps, blasts, trains, and gentle outdoor wind. Pieces slide or tumble a
  short distance, slow with friction, avoid passing through solid walls, and
  settle again. Draw them above floor tiles but beneath actors, using the same
  lighting as the ground.
- [ ] Let debris collect naturally without unlimited sprite growth. Cap loose
  pieces per room or cell and merge older settled pieces into small litter
  piles; a later step can scatter a pile again. Leave clear space around
  objectives, loot, attack previews, and important actor silhouettes.
- [ ] Keep motion and pile presentation local while scraps have no collision,
  damage, loot, or AI effect. Deduplicate break events across rollback, clear
  the collection on floor change, and reconstruct a basic settled scatter from
  saved broken-prop state on reconnect. If a future scrap becomes an actual
  pickup or obstacle, synchronize that specific gameplay object.

## UI and pointer

- [ ] Start each session with selected and ground item detail cards collapsed.
  Preserve the current control that expands them; inventory and comparison
  should still open full cards when requested.
- [ ] Reduce the overall visual footprint of UI by roughly 25–30%: quick
  slots, HP, floor/zoom labels, item cards, reward/shop/inventory text and
  icons, main menu/lobby/settings, and related shadows/offsets. Check actual
  1080p captures for room to see the world and for legibility. Retain generous
  mouse/controller targets even if their artwork shrinks; update pointer hit
  rectangles with the layout.
- [ ] Track the most recently used pointing device locally. Any meaningful
  gamepad button, stick, trigger, or D-pad input hides both the OS cursor and
  Gauche's drawn pointer, including in menus and inventory. Mouse movement or
  click brings the pointer back. Prevent stick noise and synthetic mouse motion
  from making it flicker; this state must not enter lockstep input or hashes.
- [ ] Exercise the compact default, expanded detail toggle, inventory/reward
  comparison, and controller-to-mouse switching in menu and gameplay captures.

## Suggested order

1. Tile rules and deterministic tests, then tile impact visuals.
2. Campfire state/contact rules and tests, then rendering, particles, and audio.
3. Forest ground simplification, reactive props and loose debris, then canopy
   lighting, cloud shadows, distance-fade removal, and footprint visibility.
4. Compact UI default, UI sizing, pointer device switching, and captures.

Use the Rust tile behavior as a parity reference and the current C++ lighting
as the visual baseline. Review each visible change in an SDL capture at normal
zoom before calling the task complete.
