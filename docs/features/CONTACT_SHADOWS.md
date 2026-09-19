# Contact shadows prototype — 2026-09-15

Small black clipped-corner ovals now ground the world sprites. They are independent
of light direction, strength and color, as requested. Their job is contact/depth
and showing separation from the ground, not physically correct cast lighting.

The shared ground pass covers creatures, pickups, most fixtures, raised/solid
props and most loose debris. Tiny insect and scrap shadows are lighter and smaller.
Footprints, stains, seed-bed/leaf-layer props, roots, wire, floor traps and other
flat markers do not acquire shadows. Loose leaves, twigs, bark, casings, gears,
stones and similar scraps do; spores, grass blades, pine needles, straw, flare
cinders and flat kelp scraps are excluded. This is deliberately generous coverage
for visual comparison, not a permanent classification of all future art.

Actor shadows stay at the real cell while falling survivors and existing flight
poses rise above them. The Zombie Stack has one contact at its base. Projectile
sprites and shadows use the same extracted travel/arc pose: the shadow stays on
the ground while a thrown bomb/seed rises, widens slightly and fades with height.
A net uses its active lanes and the same wall clipping as the cloth. No actor
position interpolation or gameplay movement was introduced.

The pass runs after ground surfaces and before scenery, props, debris, footprints
and bodies. Shadows darken the existing scene without adding light or revealing
bright silhouettes in unlit areas. The owl's existing landing warning is a separate
gameplay tell and remains when contact shadows are disabled. Submerged Bell Divers
are excluded. Future tall/falling constructions and additional airborne cosmetic
particles still need their own height/ground anchors when those systems are added.

## Controls and reproducible captures

Open **F1 → selector → Presentation**. Contact shadows is the master switch;
Creature/item, Prop and Loose debris switches compare each category independently.
Defaults are enabled. These are session-local presentation choices, like the
existing combat overlays; no network state or gameplay/snapshot version changed.

After building `teeming_render_scene`, create matching static views with:

```sh
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./build-release/teeming_render_scene /tmp/teeming-shadows.png shadows
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./build-release/teeming_render_scene /tmp/teeming-shadows-off.png shadows-off
```

The comparison has a chick, falling zombie, owl, stack, wasp, pickup, airborne bomb,
solid/flat props, footprints and the entire current loose-debris catalog. Both
captures were inspected. The shadows add modest depth; live visual preference is
still for user feedback.

## Cost and checks

All visible blobs are batched into one geometry call: eight vertices and six
triangles per blob, with reused local scratch vectors and screen culling before
emitting geometry. The helper restores the renderer's previous blend mode.

A temporary dense-pass measurement, SDL software renderer at 640×360, five warm
runs with all scraps visible, measured approximately:

| Scraps | Shadow pass including software draw completion |
| --- | ---: |
| 1,000 | 0.500 ms |
| 10,000 | 4.739 ms |
| 100,000 | 47.987 ms |

These are CPU-renderer references, not GPU/game frame-rate claims. The runtime
loose-debris population cap remains 4,096; this prototype did not alter population
or simulate a larger world. Profiling the full scene on the real graphics backend
is separate from measuring this isolated draw pass.

Release game/static renderer builds pass. Focused checks confirm independent
switches, unchanged simulation hash, excluded footprint shadows, visible twig
shadows, blend-mode restoration and arc/body separation returning to ground at
landing. No autonomous playthrough or permanent test suite was added.
