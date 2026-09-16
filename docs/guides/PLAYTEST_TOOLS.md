# Playtest tools specification

Requested 2026-09-15. Status lives in MASTER_TASKS.md.

- ImGui Levels window lists all implemented stages and unique maps. Jump directly into a selection.
- Save a temporary starting-stage override across launches. It changes the starting point, not normal progression. Clearly show and disable it.
- Separate Repeat option returns to the selected stage after clearing it. Unique maps must be repeatable too. Haunted House exists; inspect whether Bear Den is a full map or only a room before listing it as a level.
- ImGui Loadout window edits all six inventory slots, supported attributes, counts and relevant condition/ammo/use state, plus artifacts. Apply immediately or save as the spawn loadout for new runs and respawns. Keep editing predictable; do not silently reset it after death.
- Include representative early Forest, later Forest, Ice and Industrial entry presets. These are useful test assumptions, not promised balance targets.
- Protect deterministic multiplayer from local debug mutations; make any scope limits explicit in the UI.
- In-game controller icon setting: Auto, Xbox, PlayStation, Nintendo. Set this user's preference to Xbox. This changes prompts, not physical bindings.
- On stage entry show a modest upper-right angled banner with biome/stage number and unique name when applicable. Slight drop/fade in, then fade away.

## Implemented controls

F1 opens/closes debug windows; F2 toggles the selector. Choose Levels or Loadout under Gameplay. Offline simulation pauses while F1 is open by default; uncheck its pause option to observe live behavior. Jump and Apply are disabled in network games. Override files are local preferences, outside simulation snapshots.

Levels lists Forest 1-1 through Ice 2-4 and Industrial 3-4 plus Haunted House and Freight Exchange (Industrial 3-2). Starting stage and repeat target are separately saved; selecting another list row does not silently change either. Rewards and shops resolve before repeating. Normal survivors carry their loot forward; spawn kits apply on new runs, debug jumps and returning from death.

Loadout presets: Forest entry/veteran, Ice entry, Industrial entry, Projectile lab. Six slots support the current single attribute system, permitted stack/HP/uses/ammo, bow arrows, lantern/candle fuel, kettle contents/heat, opened bear trap, lit stick and gun muffling. Entity references and pending attacks are never saved in a kit. Four artifacts are selectable; step interval is the final value, including any Fleet Feet adjustment.

Settings and Controller Options expose Auto/Xbox/PlayStation/Nintendo prompts. This machine's controller-icons.cfg is set to Xbox. Physical bindings stay as configured. All action legends and controller diagrams share the icon selection.

Local files: playtest.cfg and controller-icons.cfg under SDL's Gauche preference directory. Invalid item kinds/options fall back safely; saved item fields are rebuilt through make_item. Main menu announces enabled playtest overrides. Disable them in Levels to return to ordinary starts.

Strict game and capture builds passed. Static Levels, Loadout and stage-banner captures inspected; no live playtest. Stage announcement lasts 3.5 seconds with a small entrance and fade, and identifies Haunted House when present.

A temporary focused check also passed: settings roundtrip, Ice start override, survivor equipment carryover, entrance-respawn kit, Haunted House repeat, and construction of all 13 listed map entries. It was not a live playthrough or a new repository test suite.

Freight Exchange subsequently adds entry 14 without changing old indices. Its
saved start/repeat indices, jump, layout banner and four-player progression were
checked directly. Index bounds now derive from the list. Fresh starts on later
floors also retain the normal living player and starter equipment instead of
copying health from an absent previous player.

Saved playtest settings version 1 is migrated to version 2 on load/save: old Fire
indices now select Industrial, old Ice indices select Ice. The chosen row, start
override and repeat target all migrate; actual saved loadout items stay intact.

## World generation viewer

Main menu → Dev → World Gen inspects standalone Forest floors 1-1–1-4 with the ordinary automatic generator and default starting kit. It does not simulate. The completed floor is retained separately from bounded pass snapshots; Play always copies the completed map, even while inspecting an earlier pass. F6/controller Back returns from play; Play again restores the original terrain, entities, loot and random state.

| Action | Keyboard / mouse | Controller (physical Xbox labels) |
| --- | --- | --- |
| Change floor, same seed | Page Up / Page Down | LB / RB |
| Next seed and regenerate | R | X |
| Play finished map | Enter | A |
| Exit inspection | Escape | B |
| Fit map | F | Y |
| Pan | WASD | Left stick |
| Zoom | Wheel or minus / equals | D-pad up / down |
| Previous / next captured step | [ / ] | D-pad left / right |
| First / finished step | Home / End | — |
| Toggle fine capture, same map | T | — |
| Roof / crown visibility | O | Right stick click |
| Fullbright | L | Left stick click |
| Details | F1 | Start |
| Browse results / return to map controls | Tab | Back |
| Room bounds | G or ImGui checkbox | ImGui checkbox |
| Vignette | V | ImGui checkbox |
| Copy current seed | C | ImGui button |

The left results sidebar lists actual feature outcomes at the selected checkpoint. **Back/Tab** enters or leaves browsing; **D-pad / arrow Up–Down** selects, **Right** opens child rolls, **Left** returns to features, **A/Enter** focuses the recorded footprint/site, and **B/Escape** returns to map controls. While browsing, A does not start play and B does not exit the viewer. Bumpers still switch floors; X/R regenerates. Once back on map controls, A plays, B exits and the D-pad resumes zoom/timeline control. Fit and focus keep the selected area beside the sidebar. The Capture tab can hide the sidebar; Back/Tab restores it. F6/Back still returns from actual play.

ImGui Generation inspector edits the next seed, copies the completed map's seed/floor/build revision/settings, scrubs passes, highlights changed terrain/material/prop kinds, and shows recorded population outcomes. Room outlines reflect the selected pass; cyan is ordinary, gold is a reserved landmark. Coarse capture is capped at 36 complete Game snapshots (33 on ordinary Forest; memory varies with map dimensions). Generation retains no snapshots when the optional capture argument is absent.

The Rolls tab now covers the six large Forest landmarks, open sectors, cross-room rivers, ordinary Forest encounters, floor identities and ant colonies: the same registry drives their real selection chances and the 1-1–1-4 matrix. Recorded outcomes distinguish missed rolls, failed footprint searches, reservations, built geometry and suppression by whole-floor uniques. Select an entry to see candidate counts/variant, highlight its room footprints, focus them, or jump to its first appearance checkpoint. Geometry built is not a promise that all later props/loot survived.

F1 → Generation inspector also reads the current game's immutable generation report during ordinary play, rather than a previous preview. Enable **Show recorded map annotations**, then select a feature or child roll to see its original footprint, guide and cells over the gameplay camera. F1 can hide the panel while the overlay stays visible. Clear selection removes the marks; changing reports/floors clears stale selections automatically. Live and preview selections are independent. **Copy floor diagnostics** copies floor, run seed, planner RNG and build revision. Reports remain excluded from gameplay hashes and plain state snapshots. Developer hosts attach recorded decisions and their build revision to network join/resync snapshots; clients can use the same live inspector and overlays. Attachments are capped at 256 KiB: oversized geometry is explicitly omitted while decision text/counts remain, or the report is marked unavailable if metadata cannot fit. Invalid diagnostic contents do not prevent a valid game snapshot from loading. Hosts built without developer mode omit the attachment. Further child-component coverage, prop/actor removal attribution, additional fine-loop coverage and other biome selectors remain pending.

`GAUCHE_DEV_MODE` defaults ON; OFF hides developer entry/hotkeys and manual gameplay zoom controls.

Giant-tree, timber-grove and snake-tunnel entries now expand into child component rolls: interior pockets, supplies, props and rewards. Each shows its local choice weights, selected ticket, result and recorded cells. Focus a site to inspect it; the map marks placed cells or the anchor of an empty/failed slot. The observation cap is 512 child entries per generation and never changes generation behavior. Item membership/rarity stays in the master supply tables.

Terrain and fuel-area selections use a faint cyan tint and an outline around the footprint, including holes and separate islands. Individual enemy, item and prop sites keep cross markers; empty attempts mark their anchor. Orange shows skipped terrain/sites. Child selection mutes the gold feature bounds so they do not cover the scene. **Tiles** counts unique footprint cells; **Sites** counts recorded placements. The same display works during live play and from developer multiplayer reports.

Open-sector components show their proposed polygon, actual opened cells and nested fighter rolls. Failed proposals retain their outline and rejection reason; accepted polygons report fighter targets/placements and ground-prop totals after population. Fighters also count in the Population tab. Separate carving/population checkpoints expose both changes.

Spider-cave entries now include habitat reach, forked growth paths and nested population pockets. Open guides run through tile centers; closed polygon guides use tile edges. For a branch, the highlighted cells are its connected habitat footprint (including invaded existing floor), while the result reports newly opened wall count. The guide remains the proposed path, so clipped/rejected reaches stay visible. The Spider habitat growth checkpoint precedes inhabitants; Cross-room inhabitants and scenery shows the populated result.

Giant-tree entries include outward-root reach, shell attachments and tapered branches. Cyan areas show actual material conversions; orange areas mark skipped terrain or preserved walking paths. The Giant root growth checkpoint shows changes before population. These are normal Root walls with normal chopping/burning behavior, not a decorative overlay.

The river entry separates the steering guide/banks from each actual source-to-outlet centerline/channel. Outlet attempts retain failure reasons; built channels show bank fighter and fish-shoal rolls. River channels and banks is a separate checkpoint. Rivers remain shallow with standable banks; required crossings remain wadeable. Current strength rolls gentle or fast flow; deep variants remain pending. Circulating rides are described below.

The Capture tab lists all registered instrumented features: giant trees, timber groves, spider caves, snake habitats, sectors, rivers, outlying encounters and late finds. Select one scope or all features, and sample every N matching attempts. Recapture same map preserves the original seed/floor and camera even if Next seed was edited. T toggles fine capture and recaptures immediately. Fine snapshots have a separate 32-entry / approximately 48 MiB limit, so reaching it does not consume coarse pass slots. The UI reports snapshot counts, estimated storage and truncation. Fine-step selection can follow its recorded component automatically.

Changed actors / loot marks additions green, removals or former positions red, and moves/changed occupants yellow. The comparison uses the preceding captured snapshot, so sampling compares across skipped attempts. Fine snapshots are resolved attempts, including rolled-back failures; they do not expose half-applied tile mutations.

Ordinary Forest encounters now record size, family, loose/close/two-pocket spacing and each placed fighter, with independent specialist and wildlife rolls. Stage 1 packs use simple attackers; later stages add hunting animals/mixed packs and specialists. The Population tab summarizes ordinary pack counts separately from landmarks. These are generation-time placements, not live threat counts; geometry may reduce a requested group and the child result states that explicitly.

Floor identity / modifiers records one major identity and one compatible minor modifier, including None. Theme weights / compatibility shows stage weights and exclusions; the feature matrix remains base chances, while a selected feature row shows its actual adjusted chance. Base room-role rolls, irregular patches and the Floor identity terrain/scenery checkpoints show where effects were applied. Nonblocking silk/vegetation can overlap landmark reservations; terrain changes preserve structural wood, roofs, hazards, hidden contents and objectives. Verified viewer examples at this revision: 1-2 seed 2 (ant trails), 1-2 seed 6 (mushroom woods), 1-4 seed 2 (mushroom woods with old ruins). Automatic whole-floor selection consumes its own rolls, so forced ordinary-layout audit seeds can differ from viewer seeds.

Ant colonies / food routes records nest/source pairs, source quantities, worker/porter choices and optional whistle captains. Food-route guides connect real finite sugar piles to real nests; trails stay clear of later blocking scenery. Colonies use remaining capacity after ordinary and cross-room fighters. They defend themselves when attacked; they are not counted as a substitute for ordinary hostile packs. The Ant colonies and food routes checkpoint shows initial placements. Heavy hauling child rolls now place two rope pullers and a load when a sufficiently wide route fits. The current forced ordinary-generation sample places crews on 1-2 seed 2 and 1-3 seed 8; use the recorded Heavy hauling roll to locate successful placement. Marked pulls warn for one second, then hit and shove anyone who stays in the center cell; trapping that victim can crush them. Disabling a puller stops the job. The cargo shares the finite sugar source and returns loads to its nest; a linked captain quickens recovery after pulls. Exposure and traffic behavior still need playtest tuning.

Mushroom woods / households records irregular district footprints, wild/settled branches, household sizes, individual defender roles and native cupboard rolls. Mushroom woods and households is a separate checkpoint after ordinary fighters and ant colonies. Homes have a front door; residents flee inside, emerge if their home burns, and defend their own settlement if attacked. Large mushroom stems are breakable cover and release sleeping spores. Examples above use the actual viewer generation path.

Forest 1-4 registers Old Growth boss clearing and Snowbound Forest border; 1-1–1-3 show them as ineligible. Try 1-4 seed 1, 2 or 3, select each feature to focus its region, then compare Forest boss clearing and Biome border scenery checkpoints. The bear has a committed maul and seven-cell rush, with directional warnings in normal play. Wood cover breaks, stone causes a long recovery, and control effects can interrupt. Killing it releases its actual native weapon and 40 gold. The normal exit is independent of the fight. Snow follows connected ground near the exit; required walking routes stay free of newly added slippery ice, and established currents remain flowing.

Giant tree now records Hollow structure, individual root branches, wet/rotten pockets and separately rolled combat groups. On Forest 1-2, try seed 1 for root galleries, 20 for a wet hollow, and 32 for a rotten floor. Select Giant tree and hide roofs to compare geometry; restore roofs/lighting or Play to judge the actual view. Fine capture filtered to Giant tree exposes each branch/pool after it resolves. These use the normal viewer path; later generator revisions may change placements.

Timber grove records its outline, glade polygons, mineral/fuel trail guides, optional wall spring, tree stands, understory density and separate fighter/visitor rolls. Forest 1-2 seeds 17, 5 and 18 illustrate overlapping woodland, linked glades and wooded ridges through the normal viewer path. Filter fine capture to Timber grove to step through glades/trails/fuel beds; use every 2 attempts if the separate 32-snapshot limit clips the final beds. Component reports are bounded at 512 entries and explicitly flag truncation.

Snake habitats record overall identity, straight/turning extent, independent link modes, dry route guides, cross-links, bank depressions, colonies and optional island opportunities. Forest 1-2 seeds 16, 4 and 21 illustrate rift banks, braided crossings and sinkhole beds; seeds 31 and 82 have turning three-room habitats. Fine capture filtered to Snake tunnel shows resolved links, bank depressions and the island attempt. Detached prizes remain optional, and a tree beside the gap is a separate roll.

`./build-release/gauche --audit-forest > /tmp/forest.csv` measures 16 fixed seeds per Forest stage without starting graphics or simulation. It checks required routes/locks and separates mobile fighters, specialists, reactive defenders, passive life and enemy sources; it also records walking-distance coverage, quiet areas, props and landmark presence. These are forced ordinary layouts, so seeds can differ from the viewer's Automatic path. See [baseline comparison and measurement limits](../engineering/FOREST_GENERATION_COMPARISON.md).

River shape can now select a circulating river on Forest 1-2–1-4. Select its Circulating route child to see the actual closed centerline and feeding water cells. Independent Rideable supports / Support type rolls place lily pads and drift logs on loops and ordinary rivers. Walk onto one to ride and walk off to dismount; it carries one player and one loose item, follows the current, and pauses at blockers or stopped/frozen water. The **River depth** child now selects wadeable channels, scattered pools (1-2 onward), or deep stretches with fords (1-3 onward). Try **1-2 seed 1** for pools and **1-4 seed 6** for a long deep channel. Dark water is lethal without support; step onto a drifting support to ride across and onto shallow water/banks to dismount. Cold Flask temporarily freezes deep river, while bridges span it. Loose cargo sinks unless carried or inflated with an Air Bladder. Ordinary currents stop at a shallow-to-deep edge; deliberate movement and shoves can enter it. The River depth and fords fine checkpoint shows the result before later population. Personal flotation gear remains pending.

The **Current strength** child records gentle (one push per second) or fast (two) flow; supports and float-equipped cargo also move twice as fast in fast water. Forest 1-1 stays gentle. Try **1-2 seed 2** or **1-4 seed 4** for fast rivers in the normal viewer. Water streaks follow current direction and strength. Freezing or stilling stops flow without losing its stored direction/strength; anchors, grip, rooted passengers and blocked landings retain their normal behavior. Seeds refer to this implementation and can change with later generator revisions.

**Outlying Forest encounters** inspects reachable quiet ground after room/cross-room populations. Its own checkpoint and feature-filtered fine captures expose quiet outcomes, species/size/member rolls and capacity failures. Entrances, objectives, reserved habitats and nearby bears are excluded; only connected quiet areas of at least 96 cells qualify. Try ordinary viewer **1-2 seed 2** (hunting pocket) and **1-4 seed 3** (wet-ground toad). Ordinary room specialists also have broader habitat choices on later floors; select Ordinary Forest encounters to inspect their actual pools.

**Late Forest finds** runs after scenery, with a coarse checkpoint and optional per-socket fine steps. In **1-2 seed 1**, select Ground search or Wall search children to inspect loose supplies, buried items and empty sockets. Expand Supply role → Supply item for actual eligible native/shared item weights from the master registry. Empty sockets show a cross; buried finds stay hidden during ordinary play until their wall opens. This pass avoids authored rewards and does not replace combat populations.

**Room roles / reservations** records each base Forest role and subsequent required-objective, secret-leaf, landmark, and socket-orientation assignments. Added landmark footprints and geometry-validation reversals are recorded too. Expand a base role to see its ordered changes and source; assignments explicitly have no additional dice roll. Select one to highlight its footprint. **1-2 seed 1** includes a thicket repurposed for a bear den. The same history is available through F1 during play and in developer network reports.

**Connected bear den** exposes nested terrain, resident, stream, approach-pack, supply and floor-patch choices. Terrain examples: **1-1 seed 1** (open banks), **1-3 seed 3** (root-separated bays), **1-2 seed 6** (branching shelters). Enable fine capture for this feature and regenerate to step through polygon pockets, connecting banks, root ribs, water/dry crossings and bed placement. Root ribs roll back if they isolate surviving ground; placement reductions state why. Actual populations and supplies roll independently of the terrain family.
