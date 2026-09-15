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
