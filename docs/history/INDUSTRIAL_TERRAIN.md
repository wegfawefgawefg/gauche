# Native Industrial surfaces

Industrial now has its own seven 16px terrain assets: two charcoal floor tiles,
a neutral wall mass, broken floor, and three molten-surface tiles. Most floor
and lava cells use an uninterrupted base; sparse marks use restrained contrast.
Molten details alternate slowly rather than repeating a bright wave in every tile.
All neighboring molten variants share their boundary color.

The renderer selects these assets by biome and removes the old orange multiplier
on Forest artwork. Exposed wall contours retain the shared neighbor-aware rule,
with a neutral Industrial rim; wall interiors have no top ledge or repeated bevel.
Dig impacts sample the matching Industrial wall asset. Existing explicit tree,
timber and ice materials still take priority, including materials brought or
created by tools. Frozen water and other surface overlays retain their own art.

This is presentation only. It adds no heat resistance, cooling permission,
machine behavior, mining material or new content-count claims. Actual underworks
rooms, enemies, props and machinery follow the catalog in later slices.

Source generator: `tools/art/industrial_terrain.py`. Finished PNGs ship with the
game; Pillow is only needed to regenerate the art.

Validation: Release game and static renderer build passed with the published
dependencies. Inspected two generated Industrial entrance captures (seeds 1729
and 429): neutral floor/wall palette, continuous molten pools, sparse details
and visible hot/cold separation. No live game or new test suite was run.
