# Folding Barricade

Implemented on 2026-09-15. This adds practical combat equipment to Industry:
spend one carried kit to shape a lane, fire through it, and retreat before a
worker or welder opens a hole. It is not bulletproof cover.

## Rules and acquisition

- Unfold three sections across the adjacent row perpendicular to facing.
  Every cell must be clear, walkable, reachable from the user and free of live
  props/entities. Lava and rail cells are excluded. A failed placement leaves
  the whole kit untouched; there is no partial deployment.
- Each section is a compact tile prop with 20 HP. Durable kits give 40 HP per
  section. Existing kit condition becomes the sections' condition. Each breaks
  independently and leaves cosmetic steel washers/tin curls, without another
  collectible kit. No entity slots or links between sections are needed.
- Walkers stop. Arrows, rivets and other narrow shots using the shared
  shoot-through rule pass. Flat throws such as rocks/hooks stop. Lobbed objects
  pass overhead while in flight, but cannot finish inside an intact panel.
  Rockets still strike the cover.
- Arc Torches and Arc Welders cut the metal through one shared property;
  ordinary panel damage also works. Existing digging crews/Strikebreakers can
  smash an obstructing section. Both sides can shoot through it.
- Steel itself is not fire fuel. Existing floor liquids/hazards remain beneath
  the deployed sections. The folded kit responds to a Horseshoe Magnet and
  counts as steel when carried around a Magnet Crane.
- Native Industrial stock starts at regional stage 2, weight 3, price 23:
  rewards, shops, caches, secrets and workshops. It is not in the guaranteed
  useful-weapon pool and has no extra Forest spawn route. Shared rare-import
  rules remain unchanged. One kit per slot, no uses/ammo refill.
- Inventory details state section count and HP. Placement gives a short
  unfolding/hinge/latch sound; existing grate impacts and breaks supply damage
  feedback. The horizontal section rotates in the renderer for vertical lanes.

## State and validation

Prop variant bit 0 means vertical; bit 1 means Durable. Existing prop HP,
broken flag and snapshot layout suffice. Decode rejects unknown variant bits,
live zero-health sections and health above the appropriate maximum. Gameplay
compatibility is `0x2026091541`; snapshot version 50 and wire version 14 remain.

The strict release build passed. A temporary direct-function check covered all
four directions, all-or-nothing wall/actor/objective/prop failures, consumption,
20/40 HP, independent destruction/no kit duplication, arrow traversal,
projectile collision rules, both torch/welder metal cutting, nonflammability,
magnetic classification, native supply, supported attributes, snapshot/hash
round trips and malformed panel state. A single SDL-dummy static capture at
normal game scale showed both orientations. No live playtest or permanent new
test suite was added. Encounter balance is for player feedback.

## Assets and reproducibility

`tools/sound/barricade.py` synthesizes the 0.63-second unfolding sound offline;
peak is limited to 0.30. Source PNGs are preserved without bitmap edits from
built-in image generation in:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.

- `exec-5a1a5bad-7a03-44f4-86f9-c27b31ffe2c4.png` ->
  `assets/graphics/folding_barricade.png`.
- `exec-eaf87454-9aa4-4dc8-84e6-3a9b1f53cdfe.png` ->
  `assets/graphics/barricade_section.png`.

Kit prompt:

> One inventory icon for a minimalist top-down pixel roguelike: three folded grey steel lattice panels strapped into a compact flat rectangular bundle, an ochre carrying handle on top. Portable FOLDING BARRICADE kit. 16x16 logical chunky pixel blocks, broad simple silhouette, four flat colours charcoal/grey/pale grey/muted ochre. Centered square canvas, fills 90 percent width and 80 percent height. True transparent background and transparent gaps in lattice. No glow, shadows, texture, gradients, floor, text or extra objects. One sprite only.

Section prompt:

> One deployable low metal barricade SECTION sprite for a minimalist top-down pixel roguelike, viewed directly from ABOVE. A horizontal strip of grey steel lattice across the middle of the square, thin charcoal rectangular frame, THREE open square gaps with transparent interiors, a tiny ochre hinge at each end. Short flat stabilizing feet extend above and below at the two ends. No perspective: can be rotated ninety degrees in a top-down game. 16x16 logical chunky pixel blocks, four flat opaque colours charcoal/grey/pale grey/muted ochre. Fill 95 percent width and 55 percent height with the section, centered. True transparent alpha everywhere outside metal; no background, shadows, glow, floor, gradient or texture. One sprite only.
