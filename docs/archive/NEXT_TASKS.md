# Gauche item UI and lighting milestone

Completed milestone archive. Current open work lives in the
[Gauche master task list](../MASTER_TASKS.md).

These agreed changes are implemented. The player will handle hands-on
playtesting; the automated gameplay, rollback, snapshot, network, presentation,
lighting, menu, UI, and border smoke checks run through CTest.

## Item patterns

- [x] Render a dim tile grid with `P` centered inside its own cell. Overlay
  damage, healing, utility, travel, and blast cells on that grid.
- [x] Make long lines and distant impact areas legible within a card. Show the
  range numerically when a diagram cannot fit every tile at a useful size.
- [x] Keep item cards and the directional world preview driven by the same
  pattern data, including obstruction and blast behavior.

## Inventory and comparison

- [x] Keep inventory open when a focused item becomes the held item.
- [x] Use smaller text and icons inside the existing cards and rows; retain
  their generous hit areas.
- [x] Add an explicit compare control for post-floor rewards and ground items.
  Reuse the inventory's side-by-side cards and show live differences where
  meaningful. Keep the held item on the consistent right-hand side.
- [x] Holding Alt hides the selected and ground item descriptions and the
  1–6 item names. Counts, cooldowns, and condition remain visible.

## Item state in the left-hand list

- [x] Show a cooldown bar plus remaining and total cooldown on each occupied
  row. The detailed card should show the same live countdown.
- [x] Show current and maximum condition for items that can take damage. A
  buckler should read, for example, `30/30` before blocking and display a
  shrinking condition bar as it wears out.
- [x] Support items with a limited number of uses before breaking and show
  their uses remaining. Keep this separate from consumable stack count and
  from a gun's loaded and spare ammunition. Do not label ammunition as HP.
- [x] Compare the actual condition, uses, or ammunition of two item instances,
  not just their type's base damage and cooldown.

Rust Gauche's `Item` had `max_count`, `count`, `consume_on_use`, usability,
droppability, and a use cooldown. It did not have durability or a reloadable
field. This milestone adds explicit stack capacity and consume-on-use state
alongside maximum condition, remaining uses, and each gun's loaded and spare
ammunition.

## Rare item attributes

- [x] Bring back the spirit of Rust's `Strong`, `Agile`, `Durable`, `Fragile`,
  `Heavy`, and `Big` attributes as explicit, uncommon item-instance modifiers.
  They should be recognizable in names, cards, comparison, and the left list.
- [x] Make attributes change actual item rules and pattern data. For example,
  Heavy can hit harder with a longer cooldown; Big can enlarge a blast or
  strike shape; Durable and Fragile can alter condition or uses. Define their
  effects per item family so nonsensical combinations are excluded.
- [x] Let a small number of post-floor item rewards roll one interesting
  modifier. Show the modified stats and pattern before choosing. Keep the
  modifier in deterministic item state, snapshots, and reconnects.
- [x] Keep Rust's clear distinction between stack count, consume-on-use, and
  cooldown while adding the newer C++ durability and magazine rules.

## Bear trap

- [x] First use opens a closed trap with a distinct sound and visible state.
  A later use places that open trap on a valid tile.
- [x] The placed trap stays visible until triggered. An actor stepping on it
  takes 100 damage; resolve its sprung state and aftermath explicitly.

## Splonks-style lighting

- [x] Seed ambient light from tile openness, with equal cardinal and diagonal
  weights as in the active Splonks workspace settings.
- [x] Add colored, falloff-based light sources for stage fixtures, entities,
  carried or ground items where appropriate, and short-lived effects. Keep
  self-glow distinct from light cast onto the world.
- [x] Sample lighting smoothly at all four tile corners. Use the field for
  terrain, entities, and any background layer Gauche renders.
- [x] Render a coherent tile border outside the playable map. Extend the
  light field into its material so lights fade through edges and corners;
  border and interior tiles must share the same world-coordinate samples.
- [x] Keep derived lighting in a local render cache, separate from lockstep
  gameplay state. Include only light inputs that must be reproducible across
  rollback or reconnect in synchronized state.
- [x] Stop light propagation at solid tiles and remove the extra output
  brightness floor. Give Gauche a stronger contrast curve than the active
  Splonks profile so distant rooms remain dark.

Splonks' active `data/settings.cfg` uses full terrain exposure and output
levels 0–1. Its code defaults use only 12% exposure and a much brighter
foreground; the active profile is the relevant visual reference here.

The reference for lighting is
`Splonks/splonks-cpp-gview-menu` on
`feature/gview-menu-integration`, especially `src/stage_lighting.cpp`,
`src/render/tile_lighting.cpp`, and `src/render/tiles_and_ents.cpp`.

The local lighting cache lives in `src/lighting/`; the live game and network
snapshots contain the fixture, actor, and item state that feeds it. Explosion
and muzzle flashes are presentation-only. Snapshot format 7 and wire protocol
6 include explicit stack capacity and consume-on-use state.
