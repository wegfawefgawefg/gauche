# Gauche working style

- Write direct C++20 in the C+ style of Adventures with Chickens and Splonks.
- Keep one clear responsibility per file, usually below 500 lines. Split by
  domain and ownership, not arbitrary line ranges or generic `utils` buckets.
- Prefer plain structs, enums, free functions, explicit switches, integer tile
  positions, and fixed-point gameplay fractions. Avoid ECS and event frameworks.
- Keep the main loop visible. Gubsy owns the SDL host, input/settings/menu
  shell, and lobby; Gauche owns its world, rules, rendering, and HUD.
- Preserve observable Rust behavior before changing a rule. Source parity and
  new dungeon-run content are separate review milestones.
- Comments explain a non-obvious "what if" case in a terse paragraph. Do not
  narrate obvious lines or leave commented-out code behind.
- Keep random gameplay choices in saved deterministic state. Local graphics,
  particles, and sound never affect gameplay hashes.
- Use the source PNG/OGG files for the port. New sprites may be rough generated
  16x16 PNGs until their content survives playtesting.
- Every smoke run must exit by itself. Use SDL dummy drivers for autonomous
  render checks, not an interactive desktop window.
- Commit cohesive milestones. Tag a major milestone only after its relevant
  build and gameplay checks pass.
