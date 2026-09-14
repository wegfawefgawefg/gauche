# Next Gauche tasks

These are the agreed changes after the first C++ UI pass. The player will
handle hands-on playtesting. Implement each domain as a reviewable milestone.

## Item patterns

- [ ] Render a dim tile grid with `P` centered inside its own cell. Overlay
  damage, healing, utility, travel, and blast cells on that grid.
- [ ] Make long lines and distant impact areas legible within a card. Show the
  range numerically when a diagram cannot fit every tile at a useful size.
- [ ] Keep item cards and the directional world preview driven by the same
  pattern data, including obstruction and blast behavior.

## Inventory and comparison

- [ ] Keep inventory open when a focused item becomes the held item.
- [ ] Use smaller text and icons inside the existing cards and rows; retain
  their generous hit areas.
- [ ] Add an explicit compare control for post-floor rewards and ground items.
  Reuse the inventory's side-by-side cards and show live differences where
  meaningful. Keep the held item on the consistent right-hand side.
- [ ] Holding Alt hides the selected and ground item descriptions and the
  1–6 item names. Counts, cooldowns, and condition remain visible.

## Item state in the left-hand list

- [ ] Show a cooldown bar plus remaining and total cooldown on each occupied
  row. The detailed card should show the same live countdown.
- [ ] Show current and maximum condition for items that can take damage. A
  buckler should read, for example, `30/30` before blocking and display a
  shrinking condition bar as it wears out.
- [ ] Support items with a limited number of uses before breaking and show
  their uses remaining. Keep this separate from consumable stack count and
  from a gun's loaded and spare ammunition. Do not label ammunition as HP.
- [ ] Compare the actual condition, uses, or ammunition of two item instances,
  not just their type's base damage and cooldown.

Rust Gauche's `Item` had `max_count`, `count`, `consume_on_use`, usability,
droppability, and a use cooldown. It did not have durability or a reloadable
field. The C++ port already has `durability` for the buckler and per-gun
`loaded`/`spare` counts, but has no stored maximum durability. Model each
kind of capacity explicitly where needed instead of overloading `count`.

## Bear trap

- [ ] First use opens a closed trap with a distinct sound and visible state.
  A later use places that open trap on a valid tile.
- [ ] The placed trap stays visible until triggered. An actor stepping on it
  takes 100 damage; resolve its sprung state and aftermath explicitly.

## Splonks-style lighting

- [ ] Seed a subtle ambient field from tile openness, including diagonals,
  in addition to a readable minimum ambient level.
- [ ] Add colored, falloff-based light sources for stage fixtures, entities,
  carried or ground items where appropriate, and short-lived effects. Keep
  self-glow distinct from light cast onto the world.
- [ ] Sample lighting smoothly at all four tile corners. Use the field for
  terrain, entities, and any background layer Gauche renders.
- [ ] Render a coherent tile border outside the playable map. Extend the
  light field into its material so lights fade through edges and corners;
  border and interior tiles must share the same world-coordinate samples.
- [ ] Keep derived lighting in a local render cache, separate from lockstep
  gameplay state. Include only light inputs that must be reproducible across
  rollback or reconnect in synchronized state.

The reference for lighting is
`Splonks/splonks-cpp-gview-menu` on
`feature/gview-menu-integration`, especially `src/stage_lighting.cpp`,
`src/render/tile_lighting.cpp`, and `src/render/tiles_and_ents.cpp`.
