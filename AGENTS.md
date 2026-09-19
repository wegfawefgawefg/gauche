# Teeming working style

- Write direct C++20 in the C+ style of Adventures with Chickens and Splonks.
- Keep one clear responsibility per file, usually below 500 lines. Split by
  domain and ownership, not arbitrary line ranges or generic `utils` buckets.
- Prefer plain structs, enums, free functions, explicit switches, integer tile
  positions, and fixed-point gameplay fractions. Avoid ECS and event frameworks.
- Keep the main loop visible. Gubsy owns the SDL host, input/settings/menu
  shell, and lobby; Teeming owns its world, rules, rendering, and HUD.
- Preserve observable Rust behavior before changing a rule. Source parity and
  new dungeon-run content are separate review milestones.
- Label important code paragraphs by domain or phase. Comments explain a
  non-obvious "what if" case tersely; do not narrate obvious lines or leave
  commented-out code behind.
- Enemy behavior may reuse shared `counter_a`, `label_a`, timer, and similar
  fields in the entity struct, as in Splonks. Document each enemy's slot meanings
  beside its init/step functions; prefer those slots over per-enemy struct fields.
- Shared `entity_a`/`entity_b` handles and `point_a`/`point_b` cells may hold
  leaders, threats, protected allies, patrol points, or territory. References
  must include the entity generation; stale handles must fail safely.
- Keep random gameplay choices in saved deterministic state. Local graphics,
  particles, and sound never affect gameplay hashes.
- Preserve source PNG/OGG files for the port. Generate new sprites and animation
  poses with code, extending the Python/Pillow scripts in `tools/art/` at the
  established pixel scale. Do not use imagegen for sprites; prioritize readable
  silhouettes and consistency with the existing game art.
- Every smoke run must exit by itself. Use SDL dummy drivers for autonomous
  render checks, not an interactive desktop window.
- Commit cohesive milestones. Tag a major milestone only after its relevant
  build and gameplay checks pass.
