# Sled — Ice item 50/50

Implemented September 15, 2026. Gameplay version `0x2026091519`; snapshot format
remains 38. Peers need matching gameplay builds.

Deploy one cell ahead on clear dry walkable ground, optionally under one loose
item. Step aboard to coast in its facing direction. Snow and ice take four ticks
per cell; ordinary ground takes eight, with sticky surfaces adding their usual
delay. Steer with movement only while stopped. Secondary brakes and dismounts;
it consumes that input instead of reloading a held weapon. Aim, attacks, use and
pickup remain available during the ride. The HUD shows the riding controls.

The vehicle is a plain entity because it moves and owns gameplay references.
It reuses `entity_a` for rider, `entity_b` for cargo, `point_a` for expected cell,
`label_a` for stopped/coasting and `timer_b` for its beat. A loaded GroundItem
uses `label_a=2` and a reciprocal generation handle; its original Item instance
remains the payload. No new snapshot fields or cosmetic networking.

Movement commits vehicle, rider and cargo cells before landing events. Rider
contact still triggers fire, traps and portals. Boarding precedes normal ice
slip; riding does not emit walking footprints. Camera smoothing stays separate.
Walls, blocking props, closed gates, bodies, other sleds, and a second cargo
stack stop travel without crushing anything. Shallow water brakes at the bank;
deep water/lava block entry. Lost support detaches occupants and strands the
sled rather than treating it as a boat. Stillwater stops it; a rooted rider stops
it. A sleeping/stunned rider coasts but cannot steer until able to act again.

Picking up/swapping cargo uses the normal inventory rules. Hooks, shoves, death
and stale handles detach displaced cargo/riders without snapping them back.
Loose cargo can be taken by the existing thieves. Braking retains cargo until
picked up. A stopped, empty, unoccupied, unlit sled can be recovered with its
actual remaining health; 45 HP normally, 90 with Durable. Melee, blasts and
shared damage effects can break it. Low runners do not block actor-height
projectiles. Fire burns the wood; water quenches it. Breaking leaves the real
cargo behind and scatters local wood/rope debris. Sleds cannot sleep or stun.

Added a top-down wood/iron sprite and six offline synthesized sounds. Found in
Ice cliff travel supplies, 10% of fishing creels (from their empty roll), and Ice
shop/reward pools. Available in the existing debug loadout editor.

Validation: release game and static renderer builds; focused direct-function
checks outside the repo for boarding before ice slip, real rider/cargo cells,
coasting/stopped steering, walls/bodies, pickup/recovery wear, external shoves,
root/fire/water/Stillwater, Durable health, destruction cleanup and snapshot/hash
continuation. Static `sled`, `sled-hud` and `sled-items` scenes inspected with SDL
dummy drivers. No live playtesting or permanent new test suite.

Ice now has implementations for all 50 catalog items and all 20 enemy behaviors.
That does not finish the biome: room composition, route integration and balance
remain tracked by MASTER_TASKS.md.
