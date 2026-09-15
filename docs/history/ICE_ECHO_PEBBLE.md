# Echo Pebble

Ice item 38/50, implemented 2026-09-15.

A stack of up to three stones records the most recent audible weapon sound within six cardinal acoustic steps. Walls, blocking props and closed doors stop recording. Twenty defined weapon voices cover guns, bow/crossbow and common melee swings. Explosions, footsteps, creature calls, ambient audio and muffled attacks do not record.

Throw up to seven tiles, or eleven with Long. It flies six ticks per tile, lands before solid cover or on the first blocking actor, then waits half a second. Three quieter repetitions, a second apart, lure curious creatures and wake sleepers through the ordinary hearing flood. Blank stones knock. No damage, bullets, attack effects or inventory use are replayed. The memory is frozen at launch; echoes never record into other stones. After a final second it crumbles into local stone chips.

Carried and loose stones record. Items with different recordings do not merge; identical partial stacks do. The detail panel names the recording. The debug loadout editor can choose any supported voice or blank memory. Sparse Echo Tunnel supplies, a 10% hound drop, regional rewards and shops provide stones at 16 gold.

Item loaded stores sound ID+1 (zero blank), spare stores acoustic radius. The projectile uses shared counters/timers for flight and the three echoes. Physical timers run independently of enemy AI. Saved recordings and playback phase are validated; gameplay compatibility advances to 0x2026091506, snapshot field layout remains 36.

Two original 16px sprites, four offline synthesized sounds, a brief blue pulse and fine expanding ring. New source scripts are under tools/art and tools/sound. Audio decoding/headroom and asset dimensions checked. Strict game/capture/codec builds passed. The existing snapshot fixture passed with a recorded bow voice and two completed echoes. Static inventory/world captures were inspected; no live playtest.

Also corrected the debug muffling editor to use the real six-charge limit and eligible weapon check, matching snapshot validation.
