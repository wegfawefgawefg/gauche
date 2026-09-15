# Industrial cover, containers and debris

Three compact props now appear in generated Industrial floors: 60-HP grates,
18-HP scrap bins and 30-HP ore bins. Workfronts and blasting alcoves attempt one
three-cell partition with open ends. Other non-entry/exit/objective rooms get up
to two separated bins. Placement follows actor/loot population, skips protected
cells and leaves space around fixtures, players and supplies. No scatter of
Forest plants is added to this biome.

Grates separate collision from sight and shot passage. Bodies, ordinary melee
reach and larger thrown objects stop. Bullets, arrows/bolts, ice needles, coal/frost
pellets and beams pass without damaging the bars. Piercing shots still respect
other existing cover rules. Bombs and rockets collide rather than slipping through
because their damage happens to be ranged. The debug weapon-range preview follows
this distinction. Sight and directional light pass; melee/area-effect sight keeps
its existing solid-cover behavior. No diagonal corner leaks are introduced.

The metal props do not act as ignition fuel. External oil/sap remains real fuel
through the existing surface rules. Grates break through ordinary prop damage;
Press Hammers get their existing double blocking-cover damage. Mining crews and
Strikebreakers already score breakable props as excavation paths, so they can
cut through a blocked passage instead of getting stuck forever.

Scrap bins roll once: 40–59 gives one Copper Wire; other ranges are currently
empty. The catalog's magnets, glow slag and chain hooks remain pending. Ore bins
roll once: 0–24 two Coal Lumps; 25–39 two to four gold; otherwise empty. Broken
props cannot reroll. Ore flakes and metallic scraps are cosmetic, not collectible
currency or ammunition. These are bonus supplies, separate from floor healing
and ammunition budgets.

Four new native debris materials have distinct drag: basalt chips stop quickly,
ore flakes skip farther, steel washers roll, and tin curls settle between them.
Industrial wall impacts produce basalt; bins/grates use the matching materials,
with existing copper curls mixed into scrap bins. Broken-prop state reconstructs
settled local litter on rejoin. All pieces use the existing bounded local pool,
contact movement, raking and sleep rules; they do not touch gameplay state or
interact with each other. This adds four Industrial catalog materials, not the
complete twenty-material set.

Eight PNG assets include horizontal/vertical mesh, both bins and four debris
silhouettes. Six offline-generated metal/rubble sounds cover hit and break cues.
Static `grates` render mode shows both orientations, containers and the debris.

Release builds and temporary direct-call checks pass: body/sight separation,
bullets/arrows/beams passing intact bars, stopped bomb landing, melee and hammer
breakage, Stoker aiming, a worker cutting the only route, non-fuel metal, one-shot
container loot, local debris containment/hash independence, and sixteen generated
Industrial floor/seed pairs with required objectives still reachable and locked.
Snapshots roundtrip. Art was inspected with a static SDL-dummy capture; no live
playtest or new permanent test suite. Feel and composition await player feedback.

Snapshot 47 and gameplay `0x2026091528` require matching rebuilt peers.
