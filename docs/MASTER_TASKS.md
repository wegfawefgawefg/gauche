# Gauche master task list

This is the **single live checklist**. [Content brief](design/CONTENT_FARM_BRIEF.md): scale/constraints; [forest ideas](design/FOREST_CONTENT_IDEAS.md): candidates; [NEXT_TASKS.md](archive/NEXT_TASKS.md): history. Preserve dark, readable lighting.

Execution started 2026-09-14. The user owns playtesting and balance feedback. Use builds and focused render/asset inspection, without large new test suites or autonomous playthroughs. Unchecked work stays visible; note uninspected visuals.

## Immediate priority: ordinary biome content and Forest variety (2026-09-16)

**Current focus override:** User paused the full-game goal and requested Forest 1-1–1-4 plus the generation/playtest inspector only. [Saved broad goal](archive/FULL_GAME_GOAL_2026-09-16.md); [replacement goal text](design/FOREST_GENERATION_GOAL.md). Baseline pinned as `forest-gen-baseline-2026-09-16` (`9d1559e`). Defer unrelated Ice/Industry/catalog work; retain the rest of this checklist for resumption.

Latest playtesting finds Forest empty/samey too; restore its layered scenes while continuing ordinary Ice and Industry content. Execute these concrete milestones before further isolated catalog expansion. Existing counts describe implementation, not encounter frequency or biome completeness. Each gameplay milestone includes normal-generator placement, meaningful interactions and generation coverage/static captures. Forced debug showcases alone do not complete it. Keep notes brief; no new history documents.

Latest playtest priorities (2026-09-16):

Generation direction agreed after the Spelunky/Splonks review: recognizable places remain, but their interiors must compose smaller alternatives and contextual population. Gauche's larger top-down maps also need growth/carving beyond room boundaries. This work takes priority over more isolated presets; prior landmark checkmarks certify their initial implementations, not replay variety. Source: [verbatim user direction](user_messages/2026-09-16-generation-feedback.txt). Start with the inspection tool, then migrate ordinary content incrementally.

- [ ] **FOREST-COMBAT-DENSITY:** Preserve the recent combat-heavy pass while recomposing generation. Make all four floors lively with things to fight, break and use; tune difficulty through enemy mix/telegraphs/terrain as well as numbers. Inspect enemy and breakable density per traversable sector, including newly carved open ground, so landmark reservations and growth do not silently hollow out ordinary encounters. Count ordinary hostiles separately from passive life and conditional threats; decorative clutter does not satisfy combat density. Keep 1-1 busy with readable simpler enemies, and build denser mixed packs and overlapping threats on later floors. Compare against the pinned baseline and user playtesting; Gauntlet/Qud is the direction.
  - Ordinary Forest encounters now compose independently weighted size, family, loose/clustered/two-pocket spacing, individual members, specialists and wildlife. Footprint-limited local groups replace the shared threat allowance; first-floor packs use bats/mosquitoes/zombies, later packs add hunters/mixed groups. Scavengers no longer spend sector/river fighter slots. Inspector exposes actual child choices/placements and group totals. Same 32-floor sample: ordinary rooms without direct fighter actors fell 106→12 of 247 (worm segments count as actors); 795 ordinary pack members were recorded. Builds, report/fine-capture neutrality, safe arrival, species windows, route/lock/codec, exhausted-capacity cases and static previews passed. Live balance, sector coverage and pinned-baseline comparison remain open.
- [ ] **GEN-INSPECT:** Dev-only generation workspace: fit and center the whole map, pan/zoom, hide branch/vignette overlays and overhead roofs/crowns independently, optional full-bright view, pause simulation, pick biome/floor/seed, regenerate repeatedly and reproduce the same seed. Keep ordinary gameplay camera/balance separate; do not reroll a live co-op session from a local inspection control.
- [x] **GEN-CONTROLLER-PLAY:** Main-menu Dev option opens World Gen. Controller bumpers switch Forest floors, a labeled action regenerates, sticks/pad pan and zoom, and fit/annotation controls remain accessible without a mouse. Always display seed; expose copyable seed/revision/settings for feedback. Play enters the exact inspected generation, not a new roll; return/replay retains its original state separately from play mutations. Inspecting must not advance enemies or hazards. Implemented main-menu viewer, physical controller/keyboard controls, exact completed-floor Play and original-state return/replay. Strict build, four-floor capture-on/off hashes and route access, direct controller actions, exact copy/replay checks and dummy-driver static menu/viewer captures passed; user live playtesting remains.
- [ ] **GEN-OPEN-SECTORS:** Initial shared polygon/line carving delivered: irregular cross-room clearings with stage-weighted shapes, structural/hazard protection, transactional exit-lock validation, their own fighters/scenery, and recorded polygon/changed-cell guides. A 32-floor check accepted 29 sectors (5,099 opened cells, 94 fighters), preserving routes/locks/borders/determinism. Broader theme-driven opening and playtest tuning remain. Allow growth/carving to erase substantial room boundaries and create broad connected sectors. Preserve useful cover, landmarks, terrain contrast and required objectives rather than preserving every planned room silhouette. Include both enclosed and open layouts in ordinary Forest, with stage/theme-dependent frequency and visible pass annotations.
- [ ] **GEN-THEME-MIX:** Add weighted major floor identities, compatible minor modifiers and occasional combinations, including ordinary Forest with no dominant event. Major themes influence terrain, resources, populations and repeated physical clues; ants must be recognizable beyond one isolated ant room. Tune per-stage/per-run exposure explicitly, explain eligibility and roll outcomes in inspector, and measure actual placements/rejections. Initial proposal: usually zero/one dominant theme plus compatible minor features, with deliberate richer overlaps; this is tunable, not a universal hard cap. A 10% per-run independent chance gives ~65% exposure across ten runs, not certainty; distinguish per-floor rolls.
  - Initial theme layer implemented: stage-weighted None/Wet woods/Timber/Spider major identities and compatible None/Mushrooms/Ruins/Undergrowth modifiers. Shared registry exposes weights/compatibility; real room-role, landmark/river/sector chances, polygon terrain/scenery patches, tree opportunities and fighter families follow the selection. Nonblocking patches can overlap landmark ground; structural edits preserve objectives, roofs, hazards and hidden contents. Separate terrain/scenery checkpoints and recorded child patches expose effects/failures. 32-floor sample: 15 no major, 9 wet, 3 timber, 5 spider; all minor outcomes, 241 patch attempts, 6,795 recorded changed cells and 767 recorded ordinary fighters. Route/lock/codec/capture/report-cap checks and static previews passed. Ant/gnome identities, richer mixed ecology, exposure balancing and user acceptance remain open.
- [ ] **GEN-ELIGIBILITY-PANEL:** Eligibility/rule details may live in ImGui; make actual session reports available during ordinary play/co-op/quickplay, not only in the viewer. Six large Forest landmark rules now drive both rolls and the inspector: floor-chance matrix, pending/ineligible/missed/failed/reserved/built/unique-suppressed outcomes, candidate counts, variants, footprint focus and reservation-pass links. Reports persist with locally generated games through ordinary play; received network snapshots explicitly lack diagnostics. Ordinary room/component rules, later removal attribution and network report transfer remain.  Controller-accessible sidebar lists registered themes/features/components for the selected biome-stage and their actual generation results. Show placed (count/variant), eligible but not selected, ineligible (floor/profile/prerequisite reason), suppressed by conflict/budget, selected but placement failed, and pending at the selected timeline checkpoint. Distinguish rolled selection from successful placement and later removal/overwriting. Show authored weights/chances with their pool/roll scope, never label a weight as an unconditional spawn percentage. Floor switching exposes how eligibility changes across 1-1–1-4; filters for present/eligible/all and a compact floor-eligibility matrix help identify missing coverage. Expand an entry for dependencies, exclusion/placement reasons and child rolls; selecting a placed entry highlights its footprint or focuses its timeline event. Use the same registration/rule evaluation and captured decisions as generation, not a separately maintained UI list or new random draws. Unimplemented ideas are not presented as rollable content.
- [ ] **FOREST-ANTS:** Small/medium/large ants with readable roles and trails from a finite sugar pile to a colony destination. Workers actually gather/carry/deliver; large ants carry conspicuous cargo, medium teams pull heavy cargo with visible ropes. Moving loads can crush occupants with a readable warning and escape opportunity. Whistle-led teams coordinate, respond to blocked paths/interference and use generation-qualified leaders/cargo. Include colony-heavy floor themes with scattered evidence, route/resource variation and mixed-theme opportunities; use appropriately simple body/collision ownership rather than requiring generic multi-tile actors.
- [ ] **FOREST-MUSHROOM-GNOMES:** Mushroom forests grow as variable terrain/vegetation regions with small houses and tiny gnomes. Gnomes flee/scream at an approaching player, enter real houses and shut doors; attacks on their homes can provoke a coordinated armed response. Add readable stick/bow/crossbow roles and gnomes riding ants, with clear affiliation, response scope, population bounds and persistent ownership. Settlement layouts, occupied/empty houses, resources and adjacent ant activity compose stochastically, not one fixed army scene. Houses and terrain support normal damage/fire/route interactions.
- [ ] **GEN-ANNOTATIONS:** Adapt Splonks's transient debug rectangles/labels (`src/state.hpp`, `state.cpp`, `render/debug.cpp`) and persistent stage-generation annotations; add world-space lines, polylines, polygons and point/cross markers. Generation records survive until regeneration and are redrawn per frame; runtime annotations clear/rebuild per simulation frame. Filter by pass/component/type; show room and child variants, footprints, chosen rolls/weights, attempted spawn sites and an X for empty/rejected outcomes with reason. Capture existing decisions without additional gameplay RNG calls or debug-dependent hashes.
- [ ] **GEN-PASS-VIEW:** Inspect base terrain, room components, cross-room growth, infrastructure, population and final scatter separately; show river/root guide paths and rasterized footprints, parent/child component identity, affected materials, reserved routes and rejection causes. Support holding layout fixed while rerolling child components/population, with explicit reproducible seed controls. Record only when inspection is enabled; bound trace memory and avoid annotation clutter by default.
- [x] **GEN-TIMELINE:** Forward/backward stepping, slider/list, first/final controls and camera-preserving same-map recapture are implemented. Optional fine capture resolves sector attempts, river attempts and spider/root branches, including rejection/rollback, with feature filtering and every-N sampling. Fine steps follow their recorded component and retain matching reports/map state. Terrain/prop/current changes and actor/loot additions/removals/moves can be highlighted. Separate caps retain up to 32 coarse and 32 fine snapshots, with approximately 48 MiB additional fine storage; limits are visible and Play always uses the retained completed map. 32-floor fine-on/off checks matched every coarse checkpoint and final hash (128 fine snapshots, 35 empty/rejected attempts), plus filtering/sampling/cap, camera, same-map recipe, controller stepping and static UI checks. Other loops can opt into the shared capture mechanism; no mutation undo or simulation is used.
- [ ] **GEN-COMPOSE:** First nested component layer delivered for giant-tree/grove/snake supplies: independent interior pockets, child supply/prop rolls, native master-table rewards, optional island prizes with exact placement, and recorded empty/failure outcomes. Removes repeated entrance bundles and the guaranteed axe beside the snake crossing. Enemy group budgets remain intact. Geometry/population composition and cross-room growth still require substantial work.  Extend GEN-LAYERS with room/landmark alternatives containing smaller weighted component slots, conditional children, probabilistic cells and geometry-aware encounters/loot. Migrate giant tree, timber grove and snake pit away from unconditional entrance kits and repeated reward triplets. Holding the landmark footprint fixed must still yield different tactical arrangements. Retain intentional signature rewards; protect required progression, while optional loot may require equipment found elsewhere.
- [ ] **GEN-GROWTH:** Shared bounded integer polygon/thick-line raster helpers, forked path proposals and planned-route validation are in use by open sectors and spider spread; biome/material policy stays with the caller. Giant-root material conversion is also integrated; broader growth families remain. Add deterministic passes that originate at a scene and extend beyond its reservation using branching paths and rasterized lines/polygons. Define material conversion/carving/merging rules, termination, overlap priorities and bounded work. Replace blanket landmark exclusion with specific protected entrances/objectives and compatible later layers; preserve outer boundary and required route/lock rules without fencing every scene into its original room.
- [x] **GEN-SPIDER-SPREAD:** Spider caves now roll contained/outlying/branching reach, variable-width forked passages and uneven terminal chambers. Connected footprints cross original room boundaries; nested empty/web/hunting-spider/nursery/prey pockets populate the surviving space. Water, pits, structural roots, hidden contents and objective areas constrain growth; invalid route edits roll back. Inspector records proposed paths, surviving habitat cells, parent rejection reasons and child weights, with a separate growth checkpoint. Build and 32-floor audit passed: 51 branches, 4,656 footprint cells outside base cave rooms and 130 pockets; routes/locks/hashes/codecs intact. Focused containment/barrier/rollback/report-cap checks and static renders passed. Exposure and human combat tuning continue under floor pacing/density.
- [x] **GEN-ROOT-SPREAD:** Giant trees roll compact/wandering/sprawling roots, weighted shell attachments and tapered forked branches using the shared growth/raster machinery. Eligible mineral walls become actual damageable/burnable Root; roots also rise through open ground. Thin walking paths preserve reachable room anchors, objective/exit access and tree mouths/cache, while hazards, outer walls, contents and overhead passages constrain growth. Inspector shows proposed branches, conversions and skipped cells/reasons at a separate checkpoint. Strict build, 32-floor generation audit, real ignition/chopping, static render and focused route/hazard/contents/rollback/report-cap checks passed. Sample: 3 outward-growing trees, 13 branches, 267 wall conversions and 103 raised floor cells. Exposure and human playtesting remain under floor pacing.
- [ ] **GEN-ROUTES:** Extend existing cross-room infrastructure into plausible whole-level road/rail routes with curves, intersections/crossings, variable stations/roadside components and explicit terrain replacement rules. Routes may pass through or reshape multiple rooms; props, traffic and inhabitants populate the resulting route rather than a fixed room kit. Gate families by biome/floor and allow compatible growth/infrastructure to coexist.
- [ ] **GEN-RIVERS:** Source-to-outlet shallow rivers now use seeded steering guides and shared weighted routing, variable channel widths, carved banks, preserved wall-backed springs, animated boundary drains/chasm spills and acyclic currents. Riverbanks add fighters/plants and optional catchable fish shoals; inspector records styles, outlet attempts, steering/centerlines and population. Strict build and 32-floor audit: 18 rivers, 4,799 water cells, 15 drains/3 spills, 128 bank fighters; routes/locks/currents/later-pass preservation/determinism/codecs passed. Focused routing/protected-terrain/cargo-drift/report-cap checks and static renders passed. Fast-flow strength, deep variants and richer crossings remain. Generate a source-to-sink centerline with seeded directional steering/waypoints, then rasterize varied channel widths and banks through multiple rooms. Support fast straighter rivers and wandering channels; terminate at a chasm with a visible pour or an edge drain. Preserve the outer boundary, ensure each channel has a coherent outlet, and place crossings/access according to required versus optional routes. Inspect centerlines, banks, widths and flow directions.
- [ ] **GEN-RIVER-RIDES:** Add an explicitly authored circulating-current variant with a generated loop and actual rideable lily pads/logs; treat it separately from source-to-sink rivers. Boarding, carrying, dismounting, blockage and deterministic co-op ownership need real behavior. Deep-channel variants have standable banks and clearly readable lethal deep water for unprotected actors; integrate flotation/escape rules and floor suitability rather than silently applying a new rule to all existing water.
- [ ] **GEN-ORPHAN-LOOT:** Final sparse scatter of loose items and embedded wall valuables, separate from scene rewards. Use biome/floor/source rarity and actual material/extraction rules; allow empty rolls, avoid duplicating authored caches, respect occupied/critical cells and annotate attempts. Cross-biome powerful gear stays exceptionally rare.
- [ ] **GEN-FLOOR-PACING:** Explicit Forest stage profiles: 1-1 simpler/friendlier but lively and combat-rich, using readable simple enemies (moths, occasional zombie, bats, mosquitoes, chickens), streams and lower complexity; 1-2 introduces more chaotic combinations; 1-3 opens the full Forest interaction/encounter mix; 1-4 includes a boss and distinctive Ice-border transition content. Tune components, growth, hazards, density, rewards and enemies together, not just HP/count multipliers. Coordinate existing boss and biome-transition tasks; expose profile weights/windows in the inspector and verify ordinary generation coverage.

- [ ] **FOREST-DENSITY:** Forest now feels empty: audit room-level inhabitants, props and layers across all four floors against prior generation. Fix shared-budget exhaustion/placement rejection and restore ordinary life, interactive clutter and occasional crowded areas; do not substitute loose twigs or blanket enemy inflation. Preserve safe arrival, required routes and exit locks; report before/after coverage and inspect ordinary seeds. First correction scales/shuffles Forest encounter allocation and increases role-specific interactive clutter: 64-floor inhabitant-free room counts fell from 206/733 to 39/733; props rose from 3,712 to 6,778. Routes/locks/snapshots passed. Scene variety and human combat tuning remain open.
- [x] **ARROW-CONTACT:** Arrows now resolve actor arrivals and current-cell overlaps before advancing, including shoves, portals and airborne toss steps. Piercing uses the existing saved, generation-qualified flight contacts once per victim per flight leg; parries reset the leg and projectile removal clears its contacts. Build and 16 head-on/perpendicular order cases passed, plus dwell/reentry, reused slots, no phantom hits behind a departed arrow, current/stale owners, moving parries/returned shots, shield/fire handling, airborne crossing, expiry, real boomerang return/catch and snapshot replay. Network gameplay version updated.
- [x] **MIMIC-TELL:** Added code-generated lid-opening/gaping, jaw snap and recovery poses, directional lean and a closing-jaw effect on the committed bite cell. The 24-tick windup now cancels on displacement, sleep, stun, toss or a 12+ damage hit; an 18-tick stationary recovery precedes retreat. Crate disguise/return-home behavior retained. Build, focused reveal/dodge/interrupt/cover/rehide checks, four-direction snapshot replay, particle/hash checks and static renders passed; gameplay version updated.
- [x] **GIANT-CANOPY-VISIBILITY:** Giant-tree relief now covers the raised crown/trunk projection and a one-cell player margin, restoring opacity over a three-cell approach band instead of concealing the player at the interior edge. The code-generated crown has an uneven lobed outline with 19.5% less opaque coverage. Build and focused checks passed across all widths/heights and intact/half/near-destroyed roofs; inside/north-overhang coverage, monotonic opacity, unchanged narrow-log reveal, per-viewer pixels and gameplay hashes verified. Static approaches/departures on all four sides, actual cut exits, distant opaque crown and damaged canopy inspected; art regenerates identically. Original feedback retained in the 2026-09-16 user notes.
- [ ] **TREE-VARIETY:** Replace uniformly three-tile oak scenery with varied heights/silhouettes and ordinary singles, small groups, groves and rings. Add conifers, rare thick 2x2 redwoods and 3x3 giant-tree constructions, plus mushroom fairy rings. Fixed multi-tile scenery is allowed without multi-cell moving actors; heights, fall reaches and collision must agree. Use layered seeded passes with varying density rather than a tiny global tree cap.
- [x] **ICE-VARIETY:** Ordinary Ice now rolls per-room pillar groves (2/3/4/5-cell heights, split/broad silhouettes) and nested clusters of three brittle-spike and three snow-pile shapes on eligible terrain. Spikes break for 1 damage plus a one-second bleed; traction/insulation protects, bandages cure, heat melts, and snow piles can be scooped without erasing underlying ice. Protected routes and objective approaches remain clear. Build, direct contact/cure/scoop/melt/fall/snapshot checks and 64 ordinary floor generation checks passed: 379 pillars, 1,219 spikes, 5,547 snow piles, all heights, repeatable generation and reachable locked objectives. Static views and reproducible code art inspected. Frozen logs remain Ice 1 only; real arches and broader scene composition remain ICE-ARCH/ICE-SETS.
- [ ] **ORCHARD-LIFE:** Rows of actual fruit trees drop apples when struck; compose a tender, tools/equipment and scarecrow. Requested living-scarecrow chance is 30%, with a readable delayed swipe and persistent generated identity. Support ordinary orchard singles/groups as well as a fuller scene; account for food supply and repeat harvesting.

- [x] **DEPTH-ORDER:** Roof rows, tall props, pickups and actors now share ground-anchor Y ordering; same-row roof pieces cover their occupants and still reveal only for the local viewer inside. Tree/pillar/lamp/tower falling poses retain their base anchor. Crane and counterweight attachments draw with their owner, including visible heads whose base is off-screen. Flat fixtures/shadows remain below bodies. Build, six pixel-exact front/behind/falling composites in both orientations, stable row ordering, inside-only opacity and unchanged gameplay hash checks passed.
- [x] **GOLD-HOOK:** Fishing Line now reels real coin entities alongside items. Exact value and normal player collection stay on the original pile; occupied landings, moved/collected cargo and interrupted casts release it. Build and direct actual-pile, other-player pickup, cancellation, occupied-bank and snapshot/replay checks passed.
- [ ] **CROWD-COMBAT:** Use haunted house as the successful combat reference: deliberately crowded pockets/gauntlets with many readable enemies, clutter/cover, crowd-control tools and longer-lasting weapons. Prototype roughly 10x weapon durability as a tuning candidate, separately from ammo/consumable quantities; audit utility budgets too. Keep quieter approaches between intense areas rather than making every room identical.
- [ ] **FEEDBACK-AUDIT:** Cross-check retained large user notes against this checklist and real code/generator integration. Add missing actionable entries, distinguish delivered/partial/not-started, and fix vague umbrella tasks. Keep this a compact coverage audit, not another history document. Archived 1–13 and balance messages have checklist coverage; Forest landmark variants needed the explicit tasks below. Remaining older conversation/conditions bundles still need the same audit. From 2026-09-16, preserve each new content-idea/feedback message verbatim with its date before interpretation.

Generation review and bear variations — [verbatim feedback, 2026-09-16](user_messages/2026-09-16-generation-feedback.txt):

- [ ] **GEN-LAYERS:** Audit each ordinary biome's actual generation order, fixed-coordinate sets, repeated shapes, density losses and orientation support. Refactor incrementally around room terrain, optional nested component groups, inhabitants/props/item sockets, cross-room passes and whole-floor overlays. Seeded weighted alternatives, quantities, footprints, conditional child components and empty outcomes must compose; keep coherent authored exceptions, protected routes and independent scene budgets. Measure coverage/repetition in ordinary seeds rather than counting catalog entries. Initial code audit: route_graph chooses roles/shapes; room_shapes forces a central cross; terrain passes apply role-specific coordinate formulas; authored populations and scatter follow; cross-room Ice/Industry features exist, but there is no consistent nested-component/orientation contract. Fixed Ice-hole scan confirmed in ice_terrain; Forest wall springs depend on a Brook room and one eligible north-facing wall search.
- [ ] **GEN-SOCKETS:** Replace the unconditional three-cell central cross in every ordinary room with protected paths to its actual incoming/outgoing sockets and objectives. Leave interior areas for shaped terrain and component placement; validate connectivity after overlays rather than erasing every shape with the same plus sign. Forest and natural Reservoir/Quarry/Echo/Cliff rooms now protect only actual graph corridors and the party arrival pad. Authored shelves, bear hollows and machinery retain their explicit routes; broader migration stays open. Build, eight rotation/reflection fixtures, 128 Forest plus 128 Ice/Industry route/lock/repeatability/snapshot checks and static room views passed. With scatter budgets unchanged, 64-floor Forest props increased from 6,778 to 9,915 as unused protected arms stopped excluding them.
- [ ] **GEN-ORIENT:** Give component groups explicit quarter-turn/mirror transforms, applying them consistently to terrain, facing, fixtures, connections, sockets and art. Audit rectangular footprints/entrances under all legal orientations; per-sprite UV rotation alone is insufficient. First terrain migration adds quarter-turns to ordinary Forest and natural Ice rooms with world-aligned bounds; Forest floor materials mirror/rotate with the outline. Authored component families still need migration.
- [x] **BEAR-ROAM:** Ordinary roaming bears and lone oversized-bear clearings now roll independently of fishing banks and connected dens. The scarred old bear has native 28px art, 320 HP, a 60-tick tell and three-cell swipe; its clearing contains a native weapon/coins and variable remains. Collision remains single-cell. 19 elder and 12 roaming clearings in 128 ordinary Forest floors; build, generation/snapshot checks and direct delayed-attack checks passed.
- [ ] **BEAR-FISH:** Restore visible Forest wall springs/streams and a bear fishing/eating beside them. Use real finite fish, recoverable/interceptable food, interrupted work and ordinary territorial combat; optional skeletons, gnawed bones, rotten fish and bank clutter in varied arrangements. Fishing component is integrated in ordinary Brook rooms: rolled existing banks, 3–6 finite River Fish, watch/paw/swat/eat poses, interrupted catches, territorial combat, optional bone/fern/log sockets and Fishing Line. Fish are retrievable, edible, cookable and usable as dry-land animal bait. 88 fishers on 66/128 ordinary Forest floors, all four bank facings; build, route/lock/determinism/snapshot checks and direct catch/interruption/hook/cooking checks passed. Skeleton/rotten-fish variation and source visibility remain open.
- [x] **BEAR-FAMILY:** Sleeping families now compose into ordinary single-room terrain (3–6 bears) and connected hollows (3–8 sleepers, with family/adult-group rolls). Distinct cub/mother/father art, sizes and stats; real beds, optional remains/food/cache sockets. Cubs flee and follow their living parent; harm or a frightened cub wakes nearby audible relatives, while walls/distance and generation-qualified identity exclude unrelated bears. 34 single-room families plus 32 connected families in 128 ordinary floors; four single-room group sizes, maximum eight sleepers, route/lock/repeatability/snapshots, local family response, stale parent identity, fleeing and static captures passed. Broader 2x2 landmarks remain separate work.
- [ ] **FOREST-SPRINGS:** Audit why entire Forest runs can lack the previous wall-source streams. Record planned Brook rooms, suitable source cells, successful placements and failures; restore ordinary varied source/pool/channel placements without putting every source in the same corner. Initial 128-floor audit: all 148 selected Brook rooms retained a spring, but 20 floors had no spring at all even including the new bear habitats. Sources have not been removed. Ordinary Brook springs now roll suitable wall edges, 3–5-cell reach and pool widths; their current and source art face out from that bank. 45 layouts/48 fixture seeds cover all four directions without wetting protected trails. All 148 Brook rooms still get sources after this change; role frequency and branch visibility remain open.
- [ ] **ICE-SETS:** Replace the repeated two-hole placement scan with seeded suitable-site selection, varying counts/spacing/bank side. Hole placement corrected: seeded eligible-site shuffle, 2–3 holes, minimum three-cell separation, protected routes excluded. 32/32 distinct fixture arrangements and 64 ordinary Ice route/lock/repeatability/snapshot checks passed; ordinary static captures inspected. Related fishing/reservoir terrain and empty interiors still need nested prop/population/loot components and whole-set orientations; task stays open.

Concrete Forest landmarks (normal generation, not debug-only catalog entries):

- [x] **FOREST-DEN:** Reserve a connected pair of rooms for a bear sleeping hollow and stream approach: broad shared geometry, dry through-route, clustered beds/sleepers, food/remains and a guarded native-tool cache. Keep independent scene population, prevent ordinary scatter overwriting it, and measure normal frequency. First connected hollow variant now generates with rolled banks, 3–8 sleeping bears, nest sites, remains/vegetation clusters and a native tool cache. 45 placements/128 ordinary floors; repeatability, dry bed/cache access, routes/locks, noise waking, snapshots and both-orientation static captures passed. BEAR-ROAM/FAMILY now add separate populations and single-room variants; BEAR-FISH and larger 2x2 landmarks retain their own remaining work.
- [x] **FOREST-SPIDER:** Ordinary Forest now reserves two or three connected cave footprints, excluding objectives and bear habitats. Uneven connecting necks, rolled web banks/broods/remains, optional mother cache chamber, native equipment/coins and an approach torch compose independently of the ordinary encounter budget. New Forest adults/hatchlings/mothers have distinct native art, stats, committed 36/60-tick bites and closing-fang effects at the locked target; they ignore their webs, while other grounded creatures are briefly rooted. Webs are punchable, burnable and traversable, with no unique-tool requirement. Build and 128-floor audit passed: 36 caves (five three-room), 21 mothers, 343 adults, 186 hatchlings, 27 different web counts, both axes, routes/locks/cache access/repeatability/snapshots. Direct windup/dodge/stun/web/replay checks and static cave capture passed; later clutter cannot occupy brood anchors.
- [x] **FOREST-SNAKE:** Ordinary Forest rolls two/three connected footprints with varied banks, winding dry ribbons, real void, optional ledges, snake groups and low remains/vegetation. Optional cache moats roll outline, bank and 1–3-cell width after gate approaches; a native Hatchet/tree can form a returnable bridge. Horn/net supplies use real displacement/rooting. New code-drawn snakes telegraph a fixed-target bite for 45 ticks, recover and can be shoved into pits. Build and 128-floor audit passed: 31 landmarks (seven three-room), 261 snakes, 17 isolated caches with actual tree-bridge round trips, all four directions/three widths, intact routes/locks and deterministic snapshots. Direct combat/fall/presentation checks, spider regression and static captures passed. Snake/spider territory no longer shares expiring noise memory.
- [x] **FOREST-ROOTS:** Ordinary Forest overlays branching roots across two existing room footprints and their separating wall, retaining base shapes/sockets. Prefer adjacent rooms without a route edge; roll side, branches/forks, hollow lengths, native inhabitants and supplies independently. Real 60-HP wooden roots use Hatchet/fire damage, bark/split feedback and connected code art; hollow sections reveal only to the occupant and lose their roof when burned/unsupported. Native tools and a cache accompany the crossing. Build and 128-floor audit passed: 31 overlays, 21 new route links/shortcuts needing at most two roots cut, 62 passages, 181 inhabitants, all directions and 23 root counts. Required routes/locks survive the overlay and total root removal; material/snapshot replay, cargo preservation, snake regression, both-axis grain/reveal and five static views passed. Seventeen new PNGs regenerate identically.
- [x] **FOREST-GIANT:** Rare ordinary-Forest tree reserves four connected planning cells, rolls canopy dimensions/height, 2–3 hollow root mouths, branching interior roots, goblin or spider groups and clustered vegetation/webs around a native cache. Code-drawn crown/bark use ground-row depth and local inside-only group reveal; fire/support loss retires the canopy without deleting occupants or loot. Build and 128-floor generation audit passed: 16 trees, 42 entrances, 322 inhabitants (13–29/tree), all three widths/lengths/heights and both populations; routes/locks/cache access, intact support, deterministic snapshots, malformed roof rejection and burning replay passed. Four static views, root-maze/depth regressions and identical art regeneration checked. Gameplay protocol/snapshot updated for saved canopy width.
- [x] **FOREST-TIMBER:** Ordinary Forest rolls a connected four-room timber grove: independently sized clearings, dense low litter with nested plant patches, 18–30 falling trees, a winding mineral firebreak, optional spring refuge, native tools/cache and its own 12–20 inhabitants. Shared four-room reservation excludes objectives/earlier landmarks. Build and 128-floor audit passed: 28 groves, all orientations, 709 trees, 462 inhabitants and 26 springs; routes/locks/access/repeatability/snapshots intact. A single ignition reached 228 cells with synchronized material replay; a complete grove burn toppled every planted tree without blocking bare routes. One 1,041-fire damage/spread/growth beat took ~0.11 ms locally (material systems only). Static approach/interior/cache/fire views and giant-tree regression checked. Whole-floor Timber Forest and broader tree silhouettes/heights remain separate tasks.
- [ ] **FOREST-GROUND:** Common mud/vegetation patches, less common entangling vines and small readable holes/pitfalls; give each actual movement/tool/fire interactions. Keep early required routes safe and preserve useful optional traversal.
- [x] **ICE-ARCH:** Added a separate open ice-rib family with two tapering, breakable/meltable feet, a faceted crown and hanging icicles. Ordinary placement rolls 3–6-cell spans, 2–4-cell heights and both axes before clutter; center terrain/occupants remain intact, pillars avoid the span, crown reveal requires standing underneath, and rows retain ground-anchor depth ordering. Heat drips temporary water and support loss retires the arch without erasing cargo or replacement props. 96 ordinary Ice floors produced 9/53/63/61 arches across stages 1/2/3/4 (24 floors each); all spans/heights/axes, route/lock/repeatability/snapshot checks, direct material/support/reveal cases and static native-art views passed. Frozen logs remain a separate Ice-1 transition feature.
- [x] **FROZEN-LOG:** Rename existing icy log assets/code and restrict ordinary placement to Ice 1 (2-1); 64 ordinary Ice floors checked: 43 logs on Ice 1, none on Ice 2–4, routes/locks/snapshots intact. Existing melting/reveal behavior retained.

Art cleanup, then overdue walk-under/falling scenery, then ordinary-biome integration:

- [x] **ART-1:** Replace all 66 exact imagegen-source PNG matches with code-drawn sprites/poses at native pixel resolution; keep filenames/gameplay, fix obsolete rendering crops/scales, inspect contact sheets and in-game static captures. Rebuild with `python3 tools/art/underworks.py`; build, deterministic regeneration and static render inspection passed. No imagegen sprites going forward.
- [x] **GEN-1:** Added `--audit-generation`: 64 ordinary floors per Ice/Industry, planned rooms, encounter/installation attempts, budget rejections, placement failures/fallbacks and actual entity/ground-item counts. Industry now selects 2–3 distinct installations with a separate bounded budget: sampled machinery scenes rose from 67 to 160; missing planned crews/assembly guards fell from 11/22 to 0/0. Ice/Industry incidental encounters use seeded shuffled room order. Build, report-neutral repeatability, routes/locks/snapshots and ordinary-seed static captures passed. Broader density/loot balance remains COMBAT-3/LOOT-2; no autonomous playthrough.
- [x] **OVER-1:** Implement connected overhead groups on one gameplay plane: code-made roof art, ground anchors, actor sorting and whole-group fade/dither when a local player walks beneath. Support split-screen/co-op visibility locally; keep collision independent of opacity. Roof spans are saved separately from ground contents; each viewport's viewer alone reveals the group.
- [x] **OVER-2:** Place hollow fallen-log/root tunnels in ordinary Forest generation, including a rare wall-crossing shortcut. Show both entrances and fade the connected roof over occupants; wood/fire/tool interactions must not strand required objectives. Hollow timber tunnels now burn and lose support when their sides are chopped; failed placement restores the ground. Further root/tree variations remain in the larger candidate roster. Log feedback: wood-bottomed passages, grain aligned on floors/supports, subtler cut ends and fully opaque roofs until inside; terrain UV rotation preserves world lighting. Both orientations inspected; fire interactions preserved.
- [x] **OVER-3 (partial design superseded):** Added frozen-log roofs and overhead maintenance gantries in Industry. The Ice art was mislabeled as arches; actual arches remain ICE-ARCH below. Include an open passage beneath a visible bridge-like span; no walkable upper deck or stacked navigation layer is required. Ice roofs melt/drip; steel spans have cuttable grate supports. Destroyed roofs leave ground contents and occupants intact.
- [x] **OVER-4 (playtest):** Hollow-log roofs now align with their support tiles. Upright logs have a closed far dome and down-facing entrance; horizontal logs retain left/right openings. Roof groups reveal only while the viewport player occupies the actual passage, not beside supports or outside entrances. Build and eight static views passed, with exhaustive passage-boundary checks in both orientations and unchanged gameplay hashes.

- [x] **OVER-5 (playtest):** Replace flat per-tile roof lighting with shared corner lighting, removing rectangular brightness patches mistaken for transparency. Timber remains fully opaque outside and fades only inside. Build, both-orientation opacity/background checks and eight static light/reveal views passed.

Roof milestone evidence: 96 ordinary generated floors retained reachable objectives and required exit locks; 54/81/76 spans across 32 Forest/Ice/Industry floors (3/0/0 empty). Rare timber shortcuts placed in 3/16 eligible fixtures. Snapshot/material replay, local reveal, wetting/burning/melting and support loss checked directly; code-made art and three new sounds inspected/decoded through static captures. No autonomous playthrough. GEN-1's broader content-frequency audit remains open.
- [x] **FALL-1:** Ordinary Ice pillars use a planted base, crack/shake tell, committed away-from-blow direction, marked lane and 42-tick warning plus 24-tick tip before crushing. Fallen chunks are breakable/meltable; survivors, loot and quest fixtures are not buried. Sustained heat shrinks the pillar, shortens its real fall reach and releases temporary water. Code art and three offline sounds added. Build, static standing/warning/falling/melt captures, four-direction impact/occupant/quest checks and snapshot replay passed; 231 pillars across 64 ordinary Ice floors (2 with no eligible site), required routes/locks intact. No autonomous playthrough.
- [x] **FALL-2:** Ordinary Forest trees take two native Hatchet blows, burn upward, and commit away from the cutting blow (fire follows their planted lean). A 54-tick warning plus 30-tick tip ends in 48-damage crush and cuttable timber; occupants/loot/quest fixtures are not buried. Supported 1–3-cell wet/void spans become real walkable log bridges; breakage restores the underlay and unsupported sections fall. Five code-made sprites and three offline sounds. Build, direct impact/fire/quench/crossing/support/hatchet/snapshot checks, original bridge regression and six ordinary-scene static captures passed. Across 64 Forest floors: 173 trees, 46 crossings including six deep/void, no empty floors; routes/locks retained. The 128-floor Ice/Industry generation audit also passed.
- [x] **FALL-3:** Ordinary Industrial workrooms now have loaded floodlight towers: saved fixed lean, buckled frame, 60-tick marked warning and 30-tick tip, flickering lamps that break/go dark, 60-damage crush and persistent cuttable metal cover. Real pipes/belts in the lane break and remain repairable; loot, survivors and quest fixtures are not buried. Five code sprites and two offline sounds. Build, direct four-direction/hammer/Arc Torch/lamp/occupant/fixture/belt-power/snapshot checks, identical art regeneration and ordinary static captures passed. Across 64 Industry floors: 183 towers, none empty; full 128-floor route/lock/repeatability audit passed. Streetlights retain their prior behavior; the eight-family-per-biome target remains open.

Distinct ordinary floors and living machinery:

- [x] **ICE-1:** Ordinary Ice can select broad snow/ice shelves with six bent/branched fissure variants, widened neighboring connections and an optional native-weapon island with a fitting permanent bridge kit. Authored interiors and required routes remain intact. Build and 128-floor repeatability/route/lock/snapshot audit passed; 24/64 sampled Ice floors selected the profile (117 shelves, 57 connections, 24 islands). Direct checks verified isolated rewards, dry approaches, three-cell bridge fit and return routes; ordinary-seed terrain and before/after bridge captures inspected. Other traversal widths/tools remain GAP-3.
- [x] **ICE-2:** Ordinary non-shelf Ice can join two broad caverns with a winding deep/shallow meltwater channel, dry bank detours, frozen diving pocket and isolated salvage plinth. Fishing Line and Folded Bridge fit the actual crossing; eels/divers and later skaters inhabit suitable terrain, with bank creels/caches. Gate approaches determine channel side. Build and 128-floor route/lock/repeatability/snapshot audit passed; 32/64 sampled Ice floors had channels (smallest connected watercourse 131 cells). Direct checks covered dry objective/exit access, real fishing retrieval, bridge fit/return/support and snapshots; ordinary-seed terrain, stream, frozen pocket and crossing captures inspected.
- [x] **ICE-3:** Eight code-made material sprites distinguish frosted rock, clear 60-HP seams, cloudy 100-HP ice and packed snow; actual shore/material boundaries get sparse edge treatments. Buildings/gate surrounds retain rock; natural walls form coarse strata. Two visible sealed supplies per sampled floor open once through digging, sustained flame, thaw charge or rail; rock/boundaries resist melting. Build, identical art regeneration, 128-floor route/lock/repeatability/snapshot audit and direct material/release/invalid-content checks passed. Inspected ordinary-floor enclosed/cracked/open captures; 128 pockets across 64 Ice floors. Snapshot 61 stores their real contents.
- [x] **IND-1:** Ordinary Industry selects broad work halls with support columns/service paths around a central core, or stepped excavation halls with a shared pit and cuttable rock rib. Shared floor/belts and a work-hall heat trough cross the old room boundary; authored installations remain intact. Sampled 28 work-hall/36 excavation floors, all with a cross-room connection. Build and 128-floor route/lock/repeatability/snapshot audit passed; direct checks verified both service paths and appropriate dig powers. Ordinary-seed terrain/scene captures inspected; crews and assembly guards retained (one guard fallback).
- [x] **IND-2:** Each sampled ordinary Industry floor has a winding conveyor between two work areas, with dry loading/unloading access, finite coal/bolt cargo, a brake shoe and endpoint fixtures where space permits. Elbows draw the actual incoming/outgoing lanes. Across 64 floors: 2,277 belt sections, 334 turns and 192 actual cargo stacks delivered in focused transport checks; unique directed paths, braking, synchronized state and snapshots passed. Static endpoints/corners inspected. Mining, fuel consumption and power remain IND-3/4/5.
- [x] **IND-3:** Ordinary belt loading banks now have a stationary cutter and exposed finite coal seam (16 real lumps). Visible cutting strokes chip actual wall HP; output jams/brakes/full pools stop extraction, clearing restarts a full stroke, and exhaustion retracts the head. Player digging shares the remaining coal; boundaries and excessive dig requirements stop work. Three code-made sprites and three offline sounds. Build, 128-floor route/lock/repeatability/snapshot audit, direct resource/jam/full-pool/boundary checks and static ordinary-floor captures passed; 64/64 Industry placements and 64 coal delivered across four sampled belts with synchronized state. Boiler/power connections remain IND-4/5.
- [x] **IND-4:** Ordinary unloading ends now feed real coal into existing boilers (+20s per accepted lump), with a spring, two breakable/repairable pipes, stored water and visible flow/leaks/gauge. Steam consumes water; a dry vessel stops pressure and warns for three seconds before losing its fuel to fire. Intake repair or cooling can prevent failure; Sealant repairs pipe HP as well as tanks. Bolts stay beside the line. Snapshot 62 saves bounded plumbing and exact tank handles. Build, 128-floor route/lock/repeatability/snapshot audit and focused fuel/cap/cargo/leak/repair/dry-fire/cooling/stale-handle checks passed: 64 connected installations, 44 actual coal consumed through four ordinary belts. Working/leaking/warning/fire captures inspected. Power consumers remain IND-5.
- [x] **IND-5:** Each ordinary boiler now powers a code-drawn steam drive, its saved conveyor path and the exact coal-cutter handle. Fuel/water loss, sealed vents or a broken drive stop motion/roller ambience/cutting; broken belt sections disconnect upstream transmission. Sealant repairs drives/belts, and hand cranks move stopped straight runs. Two of the existing eight starting coal sit at the hopper for a real cold start; normal pressure stabilizes while driving. Snapshot 63 bounds/validates these explicit links. Build, 128-floor route/lock/repeatability/snapshot audit, cutoff/repair/crank/brake/stale-link checks and powered/broken/stopped captures passed. All 64 sampled Industry floors have drives (longest 39 sections); four isolated mechanism checks cold-started and consumed 44 real coal with matching state.
- [x] **IND-6:** Staff ordinary scrap-yard and freight-exchange cranes with a visible worker/control stand. Operators move real loose metal, wait for a blocked receiving tile, flee briefly when they or their crane are hurt, then return to the controls. Absence, stun, freezing or death stops the staffed crane; stale handles never attach a replacement worker. Existing magnet priority, fixed-cell windup and automatic-crane behavior remain. Native code sprites, cable/status lamp and lunch salvage; build, focused interference/counterplay/snapshot checks, 128-floor generation audit and static captures passed (eight staffed yards in 64 ordinary Industry floors, plus freight exchange).
- [x] **LIFE-1:** Ordinary meltwater banks now have a Fishing Widow hauling finite drifting food/needles/wool with the actual fishing-hook system, nearby creels and a seal that can steal landed food. Watch from outside her two-cell territory, intercept salvage, or disturb her into the existing warned hook fight. Full banks pause work; displacement/incapacitation cuts the line, empty casts get a patient wait, and death drops the carried rod with remaining charges. Added two code-made work poses. Build, 128-floor route/lock/repeatability/snapshot audit, direct cargo/interrupt/stance/seal/full-bank checks and ordinary static captures passed: 30 staffed banks across 32 channels in 64 Ice floors (two ice-hole conflicts skipped).
- [x] **LIFE-2:** Compose an ordinary Industry shift scene using existing excavation workers, cargo handling and connected machines. Workers do jobs before aggro; territorial danger and moving loads make passage a decision. Support participants leaving their original scene. Ordinary connected halls now include a foreman, three finite-seam miners and a real-cargo hauler feeding the powered belt. Existing retaliation/whistle orders override work; carried coal survives interruptions and drops on death. Verified 64 placements, four isolated extraction/transport runs, cargo conservation, blocked intake, retaliation, stale handles, snapshot/hash parity and static renders; 128-floor generation audit passed.
- [ ] **COMBAT-1:** Add/rework straightforward Ice combat enemies alongside the specialist encounters: readable pursuit, a short visible attack tell, recovery and useful drops. Choose native silhouettes/roles (e.g. ice hound, cave brute) after checking existing overlap. Reserve ordinary spawn budget so players actually fight these; a catalog entry or another conditional puzzle enemy does not satisfy this pass.
- [ ] **COMBAT-2:** Give Industry a similar ordinary combat baseline (e.g. pipe-wielding worker and clearly telegraphed ranged guard), alongside quirky machines and crews. Audit current enemy availability first; tune mixed groups, approach space and early-biome damage without instant attacks on sight.
  - Ordinary Industry now composes one spaced Rivet Gunner/Pipe Guard group from its existing threat budget, preferring an existing gunner and requiring a dry firing approach plus sidesteps. Normal-play amber aim marks, stronger code-made brace/reload poses and real-launch air puffs improve readability. Build, direct timing/interruption/placement/cosmetic checks, five ordinary-scene captures and 128-floor audit passed. All 64 sampled Industry floors had a group (56 reused gunners, eight new); broader role variety and live balance remain open.
- [ ] **COMBAT-3:** Audit encounter density, not just total bodies: separate ordinary hostiles, conditional threats and passive life; measure reachable floor area, combat-room occupancy, groups and quiet stretches. The first pass's 3–6 new fighters per floor is not a final target. Try small ordinary groups and occasional crowded scenes alongside breathing room; review the one-fighter-per-room restriction and tune through user playtests. Reference: NetHack 3.6.7 uses sparse [ordinary-room rolls](https://github.com/NetHack/NetHack/blob/NetHack-3.6.7_Released/src/mklev.c), dense special rooms and later wandering spawns; do not copy turn-based spawn rates into real-time play.
- [ ] **COMBAT-4:** Make ordinary combat meaningfully dangerous alongside the sandbox toys. Audit native weapon damage/attack timing against enemy HP, attack/recovery windows, stagger, pursuit, group pressure, ammo and healing across progression. Establish useful weapon upgrades and dangerous enemies worth avoiding; preserve readable counterplay rather than instant attacks or universal HP inflation. Compare reference games for lessons, not target counts: Gauche has its own room sizes, floor structure and real-time pace. Use focused calculations and user playtests.
- [x] First simple-combat pass: Frost Goblin (28 HP, 24-tick windup) and Pipe Guard (44 HP, 30-tick windup), with chase/single-cell swing/recovery, raised/swing code art, six offline voice/swing sounds, useful native drops, decoy/hearing response and strong-hit/stun interrupts. Reserve existing threat budget and place after installations. Build, static captures, direct combat and snapshot checks passed; 64 ordinary floors per biome each placed 288 fighters (3–6 per floor), retaining 419/435 other mobile bodies. Required routes/locks remained intact. More simple roles and mixed-group balance remain under COMBAT-1/2; no autonomous playthrough.
- [x] Ordinary group placement: increase the basic allowance to 6/8/10/12 through each biome, retaining the specialist allowance. Place after specialist scenes/supplies; allow spaced pairs and later trios, cap existing room occupants, avoid refuges and the entrance's seven-tile sight radius. Same 128-floor sample: 575/576 basic fighters, 420/441 other mobile bodies; rooms with multiple bodies average 5.3/4.8 (previously 2.9/2.0), with about seven empty rooms per floor. One cramped floor placed 11 of 12. Build, seed reproducibility, route/lock/snapshot checks and static group captures passed. Broader hostile/conditional-life classification and user balance remain open under COMBAT-3.
- [ ] **FALL-4:** Specify fall-direction policy for each tall scenery family: away from the blow, toward it, fixed structural facing/load, or one saved random choice. Lean/art and warning lane must communicate the committed result; repeated hits cannot reroll it. Streetlights, Ice pillars and cut Forest trees use away-from-the-breaking-blow; burning trees follow their saved planted lean. Floodlight towers follow their fixed structural lean regardless of impact source. Apply an explicit policy to each future family.

Environmental response and loot quality:

- [x] **LAVA-1:** Native eight-frame flow spans an eight-tile patch, with pool-edge crust and staggered bubbling. Grounded arrivals take 5 damage immediately with a positional sizzle and visible scorch; remaining in lava adds 5 per half-second alongside the existing short afterburn. Flight/toss transit and heat-native actors are exempt; landings make contact. Existing budgeted positional bubbling silences when water/coolant turns its source to stone. Build, direct contact/landing/quench/immunity/snapshot checks, sound decode, five static ordinary-pool views and 128-floor generation audit passed. Eight code-made art frames regenerate identically; no autonomous playthrough.
- [x] **LAVA-2:** Ordinary Industry shore vents swell for 72 ticks, lob to one committed marked tile over 36 ticks, then rest 360–720 ticks. Impact deals 14 and ignites actual fuel; at most one extra neighbour is tested. Maximum 12 vents, three active tells/flights, one new tell per 30 ticks. Cooling cancels buildup; wet/foam landings quench globs; leeches/stillwater pause sources. Saved/hashed with validated snapshot state. Build, five static views, audio decode, focused timing/interaction/budget/replay checks and 128-floor audit passed; 377 vents across 64 ordinary Industry floors, none empty.
- [x] **LAVA-3:** Ordinary Industry has three-cell steam/lava seams with idle, 66-tick pressure tell, 90-tick release and cooling phases; maximum eight per floor. Water/coolant/foam suppress them, and real Heat Siphon transfers drain their finite pressure heat. Sealed salvage recesses place a useful native tool behind a seam and a full Pocket Pump on its reachable approach. Seven code-made sprites and four positional sounds. Build, eight static views, interaction/snapshot checks, existing lava regression and 128-floor audit passed. The 64-floor Industry sample placed 416 seams (222 steam/194 lava) and 64 reachable recesses whose seam cuts off the walking approach to the reward; required routes/locks stayed intact.
- [x] **LOOT-1:** Every item has one primary role in the existing supply registry; `--audit-supply` prints actual membership/counts. Native dependable totals: Forest 38, Ice 27, Industry 26. Minimum per biome: 18 combat, 6 survival, 6 mobility, 10 contextual, ten flexible slots within at least fifty. Shared items stay separate. Gaps and selection policy: [loot roles](features/LOOT_ROLES.md). Role-target content remains LOOT-3.
- [ ] **LOOT-2:** Role-aware selection now protects item rewards, favors useful cache/shop gear, reserves a further native combat/mobility pickup within the floor budget, and adds equipment branches to crates/snow caches. Build, focused offer/placement/snapshot checks, 192-floor availability sample and 128-floor generation audit passed. Fishing Line/fittings have actual matching scenes; bundle/ammo limits checked. Still review repeated scene supplies (notably Cranks/Brake Shoes), full container/enemy acquisition frequency and human balance; see [loot roles](features/LOOT_ROLES.md).
- [ ] **LOOT-3:** Fill the role targets as catalogs grow: Forest +1 survival/+1 mobility; Ice +4 combat; Industry +1 combat/+1 survival/+2 mobility/+1 contextual within fourteen further native items; fourth biome full fifty with the same role minimums. Prioritize readable fighting tools and practical consumable stacks. Recount with `--audit-supply` after additions; shared items do not fill native quotas.
  - Ice Axe and Tusk Pike are integrated into ordinary Ice supply, quarries and fishing supplies, with code-made sprites, action strokes and five sounds. Cleave/ice digging and tip-only shoves use repairable condition. Build, four-direction combat/wear/repair/snapshot checks, six static views and the 128-floor generation audit passed; 64 ordinary Ice floors placed 30 axes and 21 pikes. Ice now has 14 combat items; four more remain.
- [x] **FEEDBACK-1:** Crates recoil locally for 12 ticks, shed wood chips, show two damage stages and leave broken boards. Three seeded hollow-knock/splinter sounds replace the generic crack. Original HP, collision and loot rules preserved. Build, real fist-hit/loot/snapshot/cosmetic-bound checks, four static ordinary-Forest views and audio decode passed; three native PNGs regenerate identically.
- [x] **FEEDBACK-2:** Fallen logs/log bridges emit partial-hit events, directional recoil, wood chips and a fitting chop sound; code-made cut/split/broken poses show condition and remnants. Exposed timber/tree walls get bright grain-aligned scars and actual wood/bark fragments. Original HP/collision/bridge underlays remain. Build, six real punches, both orientations, event dedup/snapshots, bridge collapse and shared crate regression passed; hit/settled captures inspected and five new PNGs regenerate identically.
- [x] **FIRE-ARROW:** Wooden arrows/bolts catch exposed flames during actual flight, show an upright flame and light, and ignite struck actors/materials or fuel at landing. Metal rivets remain cold. Direct water/cold splashes quench; flight over water retains fire, wet impacts resist ignition. Build, campfire/surface/dead-fire, shaft/rivet, actor/timber/oil impact, parry and snapshot replay checks passed; four-direction static captures inspected.


## Scene dressing and fourth-biome candidates (2026-09-16)

New user ideas extend the concrete priorities above; candidates are not implementation claims. Ordinary scenes should have furnishings, materials, breakables and activity that explain their purpose, not just their central enemy/mechanic.

- [ ] **POWER-1:** Place an ordinary Industrial distribution yard with small electrical enemies, generator/battery banks, resistor or load assemblies, large bar switches, poles and connected lights. Tie visible working/fault/off states to actual power and the boiler-consumer work in IND-5.
- [ ] **POWER-2:** Render cheap suspended cables between endpoint fixtures, with sag, attachment points and local spark showers on endpoint/connection failure. Wires need not occupy grid cells or have separate hitboxes; saved endpoint relationships own any power effects. More ordinary Industry lighting should come from visible poles/fixtures.
- [ ] **DRESS-1:** Add code-made asphalt/road and concrete-wall treatments, half-built structures, gates and barbed-wire fencing. Give fence cutting/breaking/hazard rules and put these materials into ordinary work areas rather than only a showcase.
- [x] **DRESS-2:** Native-code pallets/stacks, wire-bound rocks and walk-through shipping containers in ordinary Industry. Cuttable steel supports, burnable wood, low cover, persistent decks and broken remnants; optional existing equipment moved onto cargo pallets without extra loot rolls. Passage/entrances reserved against later clutter. 64 floors: 128 containers, three parallel pairs, 38 cargo stacks; all routes/locks valid. Build, materials/support collapse/snapshots, static inside/outside/destroyed views and 128-floor audit passed.
- [ ] **GEAR-1:** Add a useful angle grinder with readable cutting sparks, resource cost and several native targets (grates, fences, bindings, gates). Distinguish its role from the pickaxe and existing Arc Torch.
- [ ] **GEAR-2:** Explore a gas-welding/flame tool with a dragged fuel cylinder: tether/corner handling, finite fuel, ignition and a vulnerable explosive cylinder. Distinguish it from electrical welding; provide targets and counterplay wherever it drops.
- [x] **CHAPEL-1:** Nearby exposed flames relight fueled dry candles; torch use lights rather than smashes them. Existing surface fire no longer suppresses wick ignition. Wet/spent/broken guards remain, and real weapon blows still break candles. Build and disposable direct checks passed for flame reach, torch/heat use, light/sound, guards, destruction and snapshot replay; no autonomous playthrough.
- [x] **CHAPEL-2:** Ordinary Ice chapels have their own nave shape, pew rows, book/cloth altar, floor runners, offering urns, wax drips and intact/ruined arrangements around usable aisles and candles. Six code-made prop sprites use material debris; wood/cloth burn, ceramic urns do not, and altar/urn contents provide contextual supplies. Keeper flame lights candles without smashing them. Build, static captures, material/loot and snapshot checks passed: 96 ordinary Ice floors contained 67 chapels, all with altars, 735 pew sections and 134 urns; objectives/required locks retained. No autonomous playthrough.
- [ ] **CITY-1:** Develop civilization/city plus military as the fourth-biome candidate: inhabited neighborhoods, houses/apartments, streets and parks, police/army escalation, dogs, traffic and trains. Keep it a candidate until the biome decision, then give its ordinary life and hostile response concrete generation/population budgets.
- [ ] **CITY-2:** Expand candidate civic scenes: zoo, baseball field, playground with slide, children playing/kicking a ball, waterpark and wave pool. Give civilians flee/panic reactions including burning people, distinct from combat AI; make scenes observable before interference.
- [ ] **CITY-3:** Design vehicle/response candidates (cars, tanks, helicopters, missile-firing jets) with route/airspace handling, warnings, bounded arrivals, destruction and civilian/terrain interactions rather than unbounded ambient attacks.
- [ ] **WATER-1:** Review the proposed immediately lethal deep water alongside current cold-water/rescue rules. Specify active inflatable-tube flotation, activation/release/failure timing, shoves, currents, co-op deaths and enemy exceptions before changing shared water behavior. Connect the decision to pools, docks and waterpark scenes; shallow water remains distinct.

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

Layering follow-up adds **six proposed cross-room feature families per biome**, separate from rooms/landmarks: rare post-geometry formations crossing existing room boundaries, such as burnable buried logs opening shortcuts. Frequency is now governed by biome-stage profiles and compatible overlap rules (GEN-GROWTH/GEN-FLOOR-PACING), rather than a universal one-feature cap. Rare feature families still retain run-level rarity; ordinary growth and infrastructure can coexist.

[Level-feature distinctions](design/LEVEL_FEATURES.md) clarify floor variants, triggered disasters and infrastructure. Reactor exit meltdown is an objective-triggered event; Timber Forest is a variant that enables wildfire; a station is a landmark attached to map-crossing railway infrastructure. Categories compose and do not imply new arbitrary quotas or separate engine frameworks.

- [ ] Rework haunted-house pressure: weaker/simpler ordinary zombies in larger crowds, readable delayed swipes, spawn/crowd budgets and solo/co-op pacing coordinated with the Fist nerf. Preserve the mode's promising foundation.
- [ ] Give haunted houses crowd-control tools: build/repair barriers, blockable routes, zombie breach behavior and marked lever fire/spike lanes with costs/cooldowns, friendly-fire rules and no stranded-wave softlocks.
- [ ] Add distinct haunted-house layouts, meaningful internal rooms, outside hut/outbuildings and compound candidates; define encounter boundaries, placement/supply budgets and debug variant/repeat access. See the [house follow-up](design/WORLD_INTERACTION_REVIEW.md#haunted-house-follow-up-2026-09-15).
- [x] Repair shop/three-choice transactional replacement: NO ROOM slot picker, explicit exchange confirmation, cancellation and atomic co-op stock/cost validation; South/Enter confirm, East/Escape cancel, West gameplay pickup/interact including keys, LT default reload. No attack-trigger purchases; corrected Pack/Continue and Equip/Drop mouse regions. Angled titles and static captures are in place; see [offer exchanges](features/OFFER_EXCHANGES.md).
- [x] Fix controller shop Continue navigation: Down from any card, Up restores the previous card, horizontal cycling retained; focused angled Continue button and South/Enter prompt. Ignore inactive-pointer hover. Build, direct modal/input/ready-command checks and static controller render passed.
- [ ] Finish shop/reward arrival presentation: merchant/cart, distinct arrival cues/music and fitting animation. Continue the separate offer valuation/artifact balance pass; the transactional repair does not complete this presentation work.
- [ ] Audit every enemy's notice/windup/strike/recovery, attack effects and first-hit counterplay; continue with snail and legacy circle/gun enemies after the Wolf pass below. User confirms the revised zombie windup plus slash is an excellent readability benchmark. Give each attack a readable species-appropriate tell, visible strike at actual impact, fitting sound and recovery; a timer or subtle lean alone is insufficient. Rework inherited art/behavior; compare current Stoker to the reported older build.
- [x] Zombie/stack swipe pass: replace instant adjacent hits with 24/30-tick raised-arm windups, a committed cell and 36/55-tick recovery. Restore scratch presentation at impact rather than windup; stun, displacement and 12+ damage interrupt, while toppled bodies retain their fall state. Reuse original zombie art/sounds. Build, direct hit/whiff/recovery/decoy/topple/cosmetic timing and snapshot checks passed; static warning/impact captures inspected. Broader enemy and damage/HP balance remain open.
- [x] Wolf readability pass: new code-drawn crouch/lunge and closing-jaw impact sprites, short snarl and airy snap. Preserve 18-tick windup, 11 damage and 32-tick recovery; displacement, stun/sleep/root/toss or 12+ damage cancel commitment. A dodge leaves the bite at its original target cell; canceled bites produce no jaw effect. Pack/food/whistle behavior retained. Build, direct combat/decoy/whistle/snapshot and cosmetic timing checks passed; four-direction static captures inspected and sounds decoded. Live feel remains user playtesting.
- [x] Throwing Net feedback: extend the hold from 2s to 30s, update both item descriptions and snapshot limits. Rope applications cannot shorten an existing net. Build and direct catch/countdown/release/snapshot checks passed; trapped actors retain their existing attack rules.
- [ ] **TEXT-1:** Sweep all item descriptions: short, understated, occasionally odd; remove coaching, advertising and exhaustive mechanics prose ("real weapons hit harder", "always available", "bring a friend"). Let use reveal most interactions; give a brief hint only where obscure behavior needs it. Keep explicit stat/cooldown/pattern displays. Fist direction: "It's your fist." Fungal Bread direction: "Bread... probably."
- [x] **SLEEP-1:** Fungal Bread sleeps the eater for 10s rather than 2s; preserve the existing prevention/cure interactions and validate saved status limits.
- [x] **SLEEP-2:** Sleeping creatures emit small drifting Zs; local bounded cosmetics only, stop on waking/death, readable without obscuring the creature.
- [x] **SLEEP-3:** Occasional snoring from sleeping creatures; space and limit nearby voices, stop on waking/death. Offline-generated audio, cosmetic variation only.
- [x] **EAT-AUDIO:** Shared gulp for food/drink consumption, plus a 15% chance of a short burp after eating. Trigger once on actual consumption, not a failed use or repeated held input; cover player and creature eating without multiplying sounds for one event. Keep audio variation outside deterministic gameplay state. Build and focused checks passed: player/crow consumption, exact 600-tick wake, failed-use/cure/damage paths, snapshot, cosmetic ownership, voice cleanup and sound-event deduplication. Static Z render inspected; generated sprite and decoded audio reproduce exactly.

- [x] Dog readability: full-body native sprites, 24-tick crouch/growl, committed adjacent bite with closing-jaw effect, 30-tick recovery. Retain 28 HP/6 damage, food/decoy responses and stationary attacks while netted; strong hits, stun, sleep, toss and displacement interrupt. Build, four-direction timing/whiff/interrupt/cosmetic/snapshot checks and eight static captures passed; wolf shared-effect regression passed. Three PNGs and decoded audio regenerate identically.
- [ ] Identify the second obscured silhouette from the dog playtest screenshot before changing its behavior; continue the all-enemy readability pass.
- [x] Replace Rabbit Charm retreat with persistent held movement speed; no charges, no teleport, no haste multiplication, ordinary roots/slowing/hazards preserved. See [movement toys](features/MOVEMENT_TOYS.md).
- [ ] Design separately themed escape, hop, leap, moving-flight and optional reactive dodge alongside useful gaps/tar/pits and solvable required routes; the Rabbit change does not implement these terrain/generation candidates.
- [x] **GAP-1:** Bottomless chasm terrain with dark void, upper rim and recessed face; explicit flying species, grounded actor/item loss, steps/shoves/slips/belts, airborne landing, physical throws and grounded fuses, bridge underlays/collapse, and AI route avoidance. Falling sprites shrink on the actual tile with an offline descending whistle; ordinary deaths over void do not spill loot or stack survivors ashore. Build, direct mechanics, snapshot round-trips, static render and sound decode checks passed. Creature-specific falling cries remain GAP-AUDIO.
- [x] First ordinary fissure placement: protected-route-aware cuts in eligible rooms, none on Forest 1–2 and rare narrow cuts later; wider cuts in Ice/Industry. A 192-floor deterministic generation check preserved route/lock reachability and safe spawns; 63/64 sampled Ice floors and 63/64 Industry floors contained gaps. Static ordinary-floor captures inspected. Broad void geometry, branching cuts and traversal reward pockets remain GAP-2/3; no autonomous playthrough or balance claim.
- [x] Separate chasm footing from airspace in sight and melee traces: enemies can acquire targets across pits, light/heat traces cross them, and weapons can reach airborne targets or the far bank within range. Walls/solid props retain cover. Focused sight, spear, bullet, flyer, navigation and snapshot checks passed.
- [ ] **GAP-2:** Use fissures/chasm cuts to divide ordinary spaces as well as walls, linked to ICE-1 and Industrial excavation geometry. Forest floors 1–2 get little or none; Ice/Industry can have substantial gaps. Vary narrow crossings, branching cuts and broad voids. Preserve required routes and exit locks, safe arrival cells and readable footing; optional edges may demand traversal gear.
- [ ] **GAP-3:** Generate optional graph pockets/reward islands reachable only with suitable hops, leaps, grapples, ropes, flight or constructed crossings. Vary crossing widths and provide worthwhile rewards. Check actual item ranges, usable anchors, landing space and a way back; never place the required key/switch/exit in a gear-gated pocket without a guaranteed solution.
- [ ] **GAP-AUDIO:** Add creature-specific falling cries/growls with descending pitch, filtered echo and receding volume, alongside the common fall whistle and shrinking sprite at the actual hole. Keep this presentation local/cosmetic; choose suitable voices rather than playing a creature scream for every loose object. Pits should make shoves and other displacement tools valuable.
- [ ] **ROPE-1:** Add a distinct Swing Rope: cast upward to establish a readable overhead tether, swing forward several cells, let directional input pump/reverse the swing, and release to dismount. Define attachment, range, collision, landing and release-over-void behavior, plus cancellation on death/disconnect. Keep gameplay position and collision aligned throughout; smooth rope/height animation can be cosmetic. Current Rope Hook is a short reel which stops at gaps and does not implement this.
- [ ] **GAP-4:** Integrate traversal gear with the same support/crossing rules: short hop, leap attack, temporary moving flight, existing launch spring, bridges and anchor hooks. Show landing reach and failure clearly; gap width and knockback should matter in combat. Review native availability so new terrain gives existing toys real applications without mandatory lucky drops.
- [ ] **EXIT-ALT-1:** Add occasional alternate exits with explicit destination/progression rules. Candidate: an Industrial exit on a plinth inside a roughly 6x6 chasm field, reached with a bridge or other real traversal solution. Keep a normal exit available for optional branches; if this is the required exit, guarantee sufficient tools, anchors, span length and landing space. Show the destination choice and resolve it coherently for the co-op party.
- [ ] **EXIT-ALT-2:** Add rare excavatable alternate entrances hidden about two tiles into walls, with a subtle glint/sparkle clue. Verify their reachable excavation approach, wall material/tool requirements and destination; preserve boundary walls and required locks. Discovery should reward attention without every wall sparkling.
- [ ] **TREASURE-1:** Embed real recoverable items and valuables inside eligible walls. Excavation exposes/releases contents; translucent ice can reveal them, opaque rock/wood usually conceals them. Define what fire, melting, explosions and collapse do to contents; never duplicate loot. Add a detection item/augment that reveals hidden contents locally without changing authoritative placement or obscuring ordinary play.
- [ ] **TREASURE-2:** Expand gold presentation and loot value into distinct pickups: coins, small/large gold pieces, bars and gems such as ruby/sapphire/diamond, with clear silhouettes, values and sounds. Native mining deposits make Industrial excavation worthwhile; future civilization furniture can contain jewelry and household valuables. Balance sources and sale/currency conversion with the economy; retain coherent co-op pickup ownership and serialization.
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
  The Last Shift is implemented: rare Industry 3-4 floor, explicit shutdown, opened exit, 60s meltdown, 1.5s tile warnings, bounded wall/floor spread, cooling counters, native supplies, countdown/art/sounds and dev selection/repeat. Snapshot/replay and timed-route checks pass; full simulation averaged 0.094ms/tick with 2,525 active cells. Solo pause follows simulation; online menus keep time running. Team-pause policy remains pending, so this combined task stays open.
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
  whole-floor unique; wider landmark/unique quotas remain open.
- [x] Add The Last Shift reactor unique at Industry 3-4 (seeded 20% at that
  single stage): two service routes, cooling stores, guarded side loot and
  irreversible shutdown escape. ImGui selection/start/repeat supported.
  The broader reactor task above retains the outstanding team-pause policy.

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
  Steel Toe Caps add twelve limited kicks: two-cell shoves, short bomb punts preserving fuses, cart starts and modest wall-impact damage; native supplies, freight scenes and shunter drops.
  Lunch Tins hold two interruptible meals, then remain as recoverable thrown noise lures; portion HUD, Hearth sharing, native healing supplies and foreman drops are integrated.
  All twenty initial catalog enemy behaviors and twenty-nine of fifty regional items implemented;
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

- [ ] **TOOL-LIFE:** Audit reusable utility and melee-tool lifetimes by actual useful work: walls opened, fights completed and repeated interactions, not just a small nominal charge count. Dig Claws' old 18 scratches barely opened a wall. Consider roughly 200–300 actions for repeat-use dual-purpose tools; review short-lived 3–4-use tools individually against their effect and availability. Compare per-hit wear with per-tile excavation costs before changing the digging model; explosive/one-shot consumables are a separate balance case.
- [x] **REPAIR-WEAR:** Physical melee/digging wear uses repairable condition: Stick, Pickaxe, Hatchet, Spear, Maul, Rake, Knife, Torch and Digging Claws now match existing condition-based tools. Kept all action costs/budgets/modifiers (claws 200); no second wear meter. Resin names its target on the HUD and inspect panel, restores condition only, and consumes nothing with no target. Debug v1/v2 loadouts migrate remaining wear into v3. Build, focused budget/repair/consumable/drop/snapshot/loadout checks and static HUD captures passed.
- [x] Raise newly created Digging Claws from 18 to 200 uses and update inspect text. Each scratch still costs one use and deals 8 damage: a 60-HP timber wall needs 8 scratches, a 90-HP tree needs 12. The new budget supports multiple excavations plus fighting; attack speed, damage and dig power stay as before. This is the first tool-lifetime adjustment, not the full audit or a new digging model.
- [x] **COOK-HEAT:** Held and loose meat/eggs cook one portion per 45 ticks over actual flames (campfires, surfaces, burning props, candles and exposed fire sources). Held batching stops safely on release/extinction; full packs show NO ROOM without losing ingredients. Finished food survives ordinary fire; loose food in lava is destroyed. Build, focused input/portion/capacity/heat/snapshot checks and static cooking captures passed; no new recipes.

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
