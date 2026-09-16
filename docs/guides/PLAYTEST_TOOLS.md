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
| Previous / next pass | [ / ] | D-pad left / right |
| Roof / crown visibility | O | Right stick click |
| Fullbright | L | Left stick click |
| Details | F1 | Start |
| Room bounds | ImGui checkbox | Back |
| Vignette | V | ImGui checkbox |
| Copy current seed | C | ImGui button |

ImGui Generation inspector edits the next seed, copies the completed map's seed/floor/build revision/settings, scrubs passes, highlights changed terrain/material/prop kinds, and shows recorded population outcomes. Room outlines reflect the selected pass; cyan is ordinary, gold is a reserved landmark. Current capture is coarse and capped at 24 complete Game snapshots (20 on ordinary Forest, about 19 MiB of base tile/entity storage in the checked sample). Generation retains no snapshots when the optional capture argument is absent.

Eligibility/unselected roll explanations, per-component annotations, finer checkpoints, live quickplay/co-op generation reports, and additional biome selectors remain pending. The current retained report is explicitly labeled when shown during play; it is not presented as another session's generation data. `GAUCHE_DEV_MODE` defaults ON; OFF hides developer entry/hotkeys and manual gameplay zoom controls.
