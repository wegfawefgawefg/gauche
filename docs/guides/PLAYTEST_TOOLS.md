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
| Room bounds | ImGui checkbox | Back |
| Vignette | V | ImGui checkbox |
| Copy current seed | C | ImGui button |

ImGui Generation inspector edits the next seed, copies the completed map's seed/floor/build revision/settings, scrubs passes, highlights changed terrain/material/prop kinds, and shows recorded population outcomes. Room outlines reflect the selected pass; cyan is ordinary, gold is a reserved landmark. Coarse capture is capped at 32 complete Game snapshots (25 on ordinary Forest; memory varies with map dimensions). Generation retains no snapshots when the optional capture argument is absent.

The Rolls tab now covers the six large Forest landmarks, open sectors and cross-room rivers: the same registry drives their real selection chances and the 1-1–1-4 matrix. Recorded outcomes distinguish missed rolls, failed footprint searches, reservations, built geometry and suppression by whole-floor uniques. Select an entry to see candidate counts/variant, highlight its room footprints, focus them, or jump to its first appearance checkpoint. Geometry built is not a promise that all later props/loot survived.

F1 → Generation inspector also reads the current locally generated game's immutable report during ordinary play, rather than a previous preview. Reports are excluded from gameplay hashes and snapshot serialization; received network snapshots explicitly show unavailable diagnostics. Ordinary room/child-component rolls, later removal attribution, additional fine-loop coverage, network report transfer and other biome selectors remain pending.

`GAUCHE_DEV_MODE` defaults ON; OFF hides developer entry/hotkeys and manual gameplay zoom controls.

Giant-tree, timber-grove and snake-tunnel entries now expand into child component rolls: interior pockets, supplies, props and rewards. Each shows its local choice weights, selected ticket, result and recorded cells. Focus a site to inspect it; the map marks placed cells or the anchor of an empty/failed slot. The observation cap is 256 child entries per generation and never changes generation behavior. Item membership/rarity stays in the master supply tables.

Open-sector components show their proposed polygon, actual opened cells and nested fighter rolls. Failed proposals retain their outline and rejection reason; accepted polygons report fighter targets/placements and ground-prop totals after population. Fighters also count in the Population tab. Separate carving/population checkpoints expose both changes.

Spider-cave entries now include habitat reach, forked growth paths and nested population pockets. Open guides run through tile centers; closed polygon guides use tile edges. For a branch, the highlighted cells are its connected habitat footprint (including invaded existing floor), while the result reports newly opened wall count. The guide remains the proposed path, so clipped/rejected reaches stay visible. The Spider habitat growth checkpoint precedes inhabitants; Cross-room inhabitants and scenery shows the populated result.

Giant-tree entries include outward-root reach, shell attachments and tapered branches. Cyan cells are actual material conversions; orange crosses mark skipped terrain or preserved walking paths. The Giant root growth checkpoint shows changes before population. These are normal Root walls with normal chopping/burning behavior, not a decorative overlay.

The river entry separates the steering guide/banks from each actual source-to-outlet centerline/channel. Outlet attempts retain failure reasons; built channels show bank fighter and fish-shoal rolls. River channels and banks is a separate checkpoint. Initial rivers are shallow, with normal currents and standable banks; required crossings remain wadeable. Deep/fast variants and circulating rideable routes remain pending.

The Capture tab enables individual sector/river attempts and spider/root branches, filters to one feature, and samples every N matching attempts. Recapture same map preserves the original seed/floor and camera even if Next seed was edited. T toggles fine capture and recaptures immediately. Fine snapshots have a separate 32-entry / approximately 48 MiB limit, so reaching it does not consume coarse pass slots. The UI reports snapshot counts, estimated storage and truncation. Fine-step selection can follow its recorded component automatically.

Changed actors / loot marks additions green, removals or former positions red, and moves/changed occupants yellow. The comparison uses the preceding captured snapshot, so sampling compares across skipped attempts. Fine snapshots are resolved attempts, including rolled-back failures; they do not expose half-applied tile mutations.
