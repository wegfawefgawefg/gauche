# Playtest tools specification

Requested 2026-09-15. Status lives in MASTER_TASKS.md.

- ImGui Levels window lists all implemented stages and unique maps. Jump directly into a selection.
- Save a temporary starting-stage override across launches. It changes the starting point, not normal progression. Clearly show and disable it.
- Separate Repeat option returns to the selected stage after clearing it. Unique maps must be repeatable too. Haunted House exists; inspect whether Bear Den is a full map or only a room before listing it as a level.
- ImGui Loadout window edits all six inventory slots, supported attributes, counts and relevant condition/ammo/use state, plus artifacts. Apply immediately or save as the spawn loadout for new runs and respawns. Keep editing predictable; do not silently reset it after death.
- Include representative early Forest, later Forest, Fire and Ice entry presets. These are useful test assumptions, not promised balance targets.
- Protect deterministic multiplayer from local debug mutations; make any scope limits explicit in the UI.
- In-game controller icon setting: Auto, Xbox, PlayStation, Nintendo. Set this user's preference to Xbox. This changes prompts, not physical bindings.
- On stage entry show a modest upper-right angled banner with biome/stage number and unique name when applicable. Slight drop/fade in, then fade away.

## Implemented controls

F1 opens/closes debug windows; F2 toggles the selector. Choose Levels or Loadout under Gameplay. Offline simulation pauses while F1 is open by default; uncheck its pause option to observe live behavior. Jump and Apply are disabled in network games. Override files are local preferences, outside simulation snapshots.

Levels lists Forest 1-1 through Ice 3-4 plus Haunted House. Starting stage and repeat target are separately saved; selecting another list row does not silently change either. Rewards and shops resolve before repeating. Normal survivors carry their loot forward; spawn kits apply on new runs, debug jumps and returning from death.

Loadout presets: Forest entry/veteran, Fire entry, Ice entry, Projectile lab. Six slots support the current single attribute system, permitted stack/HP/uses/ammo, bow arrows, lantern/candle fuel, kettle contents/heat, opened bear trap, lit stick and gun muffling. Entity references and pending attacks are never saved in a kit. Four artifacts are selectable; step interval is the final value, including any Fleet Feet adjustment.

Settings and Controller Options expose Auto/Xbox/PlayStation/Nintendo prompts. This machine's controller-icons.cfg is set to Xbox. Physical bindings stay as configured. All action legends and controller diagrams share the icon selection.

Local files: playtest.cfg and controller-icons.cfg under SDL's Gauche preference directory. Invalid item kinds/options fall back safely; saved item fields are rebuilt through make_item. Main menu announces enabled playtest overrides. Disable them in Levels to return to ordinary starts.

Strict game and capture builds passed. Static Levels, Loadout and stage-banner captures inspected; no live playtest. Stage announcement lasts 3.5 seconds with a small entrance and fade, and identifies Haunted House when present.

A temporary focused check also passed: settings roundtrip, Ice start override, survivor equipment carryover, entrance-respawn kit, Haunted House repeat, and construction of all 13 listed map entries. It was not a live playthrough or a new repository test suite.
