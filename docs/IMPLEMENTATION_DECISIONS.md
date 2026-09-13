# Gauche implementation decisions

- Keep the old Rust commit/history intact. Do not transplant Rust history into
  the new C++ repository; link it as the source reference.
- Do not port dead placeholders merely to match enum names. Replace the old
  settings stubs with Gubsy settings; the new exit objective supplies a real
  win/transition rule in the first run slice.
- Preserve the old control semantics first (movement, use direction, mouse,
  inventory, pickup/drop, zoom). Gubsy remapping can expose those same actions.
- The first pass should use the original authored PNG/OGG assets. Load each
  PNG directly by its `Sprite` name. Keep asset metadata in code unless a
  concrete Gauche asset later needs richer data. New-content placeholder
  sprites may be generated with a small Python script at the source's usual
  16×16 scale.
- Pin the Gubsy dependency rather than relying on whichever of the two local
  Gubsy checkouts happens to be on disk. They currently differ.
- The Rust history remains in `gauche-rs`; the new C++ history is published as
  `gauche`. Both local checkouts track their matching GitHub repository. The
  `port-finish` and `direct-netcode-finish` tags mark the playable port and
  direct rollback milestones.
