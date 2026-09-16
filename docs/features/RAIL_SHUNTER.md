# Rail shunters and freight carts

Implemented the twentieth Industry catalog enemy behavior together with a real
cart and optional freight siding. This reaches the initial behavior count;
encounter balance and the remaining regional items are still work to do.
The subsequent [rail-points slice](RAIL_POINTS.md) implements keyed T junctions. No interactive playtesting was run.

## Shunting encounter

A 120-HP worker walks slowly to the rear of its assigned cart, rings a hand bell
for 0.8 seconds, then pushes in that committed direction. A moved, destroyed or
braked cart invalidates the commitment. Damage, cold, sleep, stun, rooting and
displacement interrupt the warning. After the cart stops at a rail end, the
worker walks around it and shunts back. A missing cart does not leave the worker
permanently inert: it can seek an unclaimed replacement or approach a nearby
player. A generation-tagged handle cannot silently become a replacement actor.

An adjacent opponent gets a separate 0.6-second raised-bar warning before a
12-damage fixed-cell swing. The worker then recovers for a second. It never
hits immediately on sight or arrival.

One death roll: 20% Brake Shoe, next 20% Rail Switch Key, next 20% 4–8 gold, otherwise nothing. Cargo belongs to the cart and
is independent of worker loot.

## Cart and existing tools

A cart is one 64-HP blocking entity, carrying one complete Item in its existing
`ground_item` field. It rolls one tile every eight ticks on existing rails.
The first occupied cell takes 18 damage; the cart stops before it, without
repeated damage or shoving a victim into an instant wall crush. Missing track,
solid cover and track ends also stop it. It cannot dig a route itself.

Empty carts collect one actual loose item along their path. Stopped carts expose
that item to the ordinary adjacent pickup/swap interaction. Ammo, modifiers,
stack, wear and remaining item state survive transport. Destruction turns the
cart's own entity slot into the carried item with a new generation, so a full
entity pool cannot discard the cargo. In-flight/anchored items and another
sled's cargo cannot be taken.

Brake Shoes now stop carts for six seconds as well as jamming belts. Already
braked targets refuse another shoe without spending it. A health hit of at
least 16 snaps the brake; expiration frees the cart but does not silently
restart it. The old full-size train ignores shoes.

Chain Hooks catch, stop and haul carts along remaining track; they pull the
cart rather than treating it as an immovable anchor or taking only its cargo.
A set brake refuses hauling. Ordinary shove tools can start a cart rolling,
with the pusher retained for collision/friendly-fire attribution.

Authored freight rails have 24 HP and require dig power one. Picks, vacant-cell
Pocket Drill contacts and digging blasts can break them into ordinary floor.
Blocking cover/bodies take drill contact first. The old train's zero-HP rails
retain their previous behavior. No new rail direction fields or arbitrary
track construction were introduced.

## Placement and presentation

Freight sidings are one of twelve optional Industry maintenance roles. A
thirteen-cell horizontal track, worker, loaded cart and Brake Shoe supply fit
in a dry room with open margins. The cart starts with either three Bolt Pouch
handfuls or a Press Hammer at 12 remaining durability. The room spends three
threat and two equipment units; native starting-weapon allocation still runs
first.

Placement checks the complete footprint before mutation. Rails can cross the
reserved walking path because they remain walkable; actors and supplies start
outside it. The optional role is excluded from objective/exit assignment and
secret-room replacement. The later [rail-points slice](RAIL_POINTS.md) adds branching junctions with
working keyed points and supplies the appropriate key beside them.

Two original transparent sprites provide worker/cart silhouettes. A raised-bell
shake and raised strike pose communicate warnings; cargo is drawn inside the
tub and a brass brake strip shows braking. Eleven offline OGG cues cover wheel
clatter, stops, impacts, cargo handling, breakage, bell, swing, death and rail
cutting. Generator: `tools/sound/freight.py`.

## State and verification

Existing shared Entity slots hold phases, timers and generation-safe handles.
Snapshot validators check cart health, phase, clocks, facing and safe cargo
references. Gameplay version is `0x2026091565`; co-op peers need matching builds.

Strict release build passed. Temporary focused checks covered the warning and
travel clock, worker turnaround, exact cargo snapshot replay, single collision,
actual Brake Shoe use, chain hauling and pickup, collection, full-pool salvage,
cuttable rails, real drill contacts and cover precedence, six interrupts,
stale handles and malformed snapshots.

All 128 sampled Industry floors retained exit reachability and required locks,
decoded successfully and produced matching hashes. Eight contained complete
freight rooms. This wider check also exposed and fixed an older
[scrap-yard snapshot bug](../engineering/SCRAP_YARD_SNAPSHOT.md). All eleven sounds loaded under
SDL dummy audio, and a static software-render capture was inspected. Player
feedback is still needed for difficulty, pacing and readability in motion.
