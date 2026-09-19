# Bolt Pouch — 2026-09-15

A practical Industrial throwable: five small physical bolts, each dealing six
blockable damage, thrown in a widening fan. All start at the player's hand.
First cell is concentrated (up to 30 damage); the second separates into three
lanes, and the third into five. Range five, four ticks per cell, 42-tick recovery.
This replaces the initial two-damage catalog sketch with a useful limited combat
supply. Damage still respects enemy defenses and lobby friendly-fire rules.

Each handful consumes one stack unit; maximum stack six, ordinary pickup bundle
three, native stage one, common weight eight, price nine. Strong/Heavy change
per-bolt damage, Agile recovery, Long reach; unsupported modifiers stay excluded.
All Piercing uses the existing artifact convention. No reload or ammunition
refill, and no recovered bolts. Folded pouches count as steel for magnets/cranes.

Five entity slots must be available before any projectile is allocated; refusal
spends nothing. Bolts pass grates and low mesh, hit solid cover, and cannot cut a
diagonal corner through walls. A cover-breaking bolt is itself absorbed. Bolts
can be parried, then travel straight back with the new owner; the original hard
lifetime is never extended. Generation-checked owner handles prevent stale-slot
immunity. Shared snapshot validation checks the fan lanes, item, damage, range
and clocks. Gameplay version is `0x2026091551`; snapshot layout remains 50.

Assembly lines now carry a three-handful pouch in place of one of their two coal
pickups, alongside existing crank/brake supplies and shoot-through grate fights.
Scrap-bin roll 60–79 gives the same bundle; the master supply table includes
rewards, shops, caches, secrets and workshops. This is not a new room family.

Two original offline sounds (`tools/sound/bolt_pouch.py`) provide a cloth/steel
throw and quiet bouncing clatter. Each impact emits one local steel washer scrap;
there is no new collectible, obstruction or net-synced cosmetic object. Sound
semantics enter the existing hearing rules. The existing Rivet sprite is reused
for visible hardware in flight.

New bitmap: `assets/graphics/bolt_pouch.png`, generated with the built-in imagegen
tool; original retained as `exec-2560862e-0e9a-4c6f-90a2-820dbf1e2a93.png` under
the thread's generated-image directory. Final prompt:

> Create one tiny pixel art inventory sprite for Teeming, a minimalist top-down dungeon game. Subject: open squat dark rust-brown canvas bolt pouch, five pale dull steel hexagonal bolt heads peeking from its mouth, short folded cloth lip. One isolated object, centered, actual transparent alpha background. Crisp hand-placed chunky pixel shapes, limited 6-color palette charcoal outline #262a26, muted rusty brown #765038, cloth tan #a37c4e, dull gray steel #88958b, sparse cream #c9cbb1. Readable silhouette and negative space; no texture noise, no text, no glow, no scenery, no border, no ground shadow. Looks like a 16x16 pixel-art item enlarged with nearest-neighbor blocks, not smooth illustration. Top-down/three-quarter item icon matching old minimalist roguelike equipment sprites.

Validation: strict release build; temporary direct checks of delayed close-up
hits, five-lane spread, grate passage, cover absorption, corner blocking, full
entity-pool refusal, mid-flight snapshot/replay equality, invalid-lane rejection,
parry ownership/lifetime, stack merging and authored assembly supplies. Sixty-four
generated Industry floors retained reachable routes and required locks, with 64
pouch supplies present. SDL dummy loaded the PNG and both OGGs; inspected a static
fan/cover render. No interactive playtest or permanent test suite added.
