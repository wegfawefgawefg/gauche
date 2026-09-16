# Ice circuit tools

Copper Wire and Grounding Spike bring Ice to 36 implemented regional items.
The remaining catalog, unique floors and other biomes remain unfinished.

## Copper wire

One use lays one adjacent, nonblocking 4-HP segment. A spool costs 11 gold and
has six segments; Durable has twelve. Placement rejects bodies, loot, fixtures,
active props and lava, and can replace broken clutter. Shallow water is valid.
Stepping on wire does not break it. Attacks do, leaving local copper fragments.
Metal wire does not burn or generate electricity by itself.

Wire and wet tiles form one cardinal circuit, stopped by solid cover. A source
retains its range and attenuation: Eel Battery reaches four steps (Big five),
and the eel's discharge reaches four. The traversal remains bounded to a
six-step diamond. Connections render as thin copper arms meeting adjacent wire,
stakes or water. Wires on dry or frozen ground can carry a pulse, but eels
still require actual water to swim. Grounded actors on a wired cell can be hit;
flying creatures are not grounded by a floor wire. Direct contact still hits.

## Grounding spikes

A spike costs 15 gold, has 8 HP and occupies a nonblocking prop cell. Full-condition
spikes stack to three; damaged recovered spikes do not merge into fresh stacks.
A ready spike on a pulse's circuit or one cardinal cell beside it sinks that
whole conducted pulse before damage. Ties use the nearest wave node and stable
cardinal order. Walls and blocking fixtures stop the connection.

The spike becomes hot and spent for 180 ticks. Its sprite glows and it gives off
brief real warmth; this can thaw its ice cell or light oil. Water, including a
direct pour, cools it immediately. Cooling leaves a visibly spent stake, not an
automatically rearmed shield. While spent it conducts subsequent shocks normally.
Pick up the cool spike at your feet or directly ahead, then plant it again to
rearm. Ordinary pickup/swap rules apply and preserve actual condition. Recovery
reserves an item entity before removing the prop. No room or item capacity means
the stake remains intact. Breaking it leaves local metal debris.

Prop state uses the existing eight-byte structure: variant 0 ready, 1 hot spent,
2 cool spent; growth_ticks is cooling time, hp is condition. Neither wire nor
spike spends an actor slot while placed. Snapshot layout remains 35; validation
accepts only valid cooling/variant ranges. Gameplay version is `0x2026091504`.

## Other electrical sources and presentation

Thunder Acorn retains its traveling projectile and four-body chain on dry ground.
At water or wire, its remaining energy switches to a six-step conducted pulse;
a ready spike can intercept this too. The remaining actor chain ends there,
avoiding a second hit through a circuit loop. Routes and generation-checked
victims are captured before damage, so deaths and cover destruction cannot add
new victims or extend a discharge. This intentionally updates the original
water-assisted actor jumps described in the older forest implementation notes.

The optional attack preview follows these circuits and shows a grounding point
instead of red damage cells for an intercepted pulse. Player item descriptions
explain placement, uses, condition, cooling and recovery. Spike stacks show their
count and HP in the quick inventory. Electrical branches use the existing local
arc particles; cooling adds a small vapor puff. Five original 16px PNGs and eight
offline synthesized OGG sounds cover the tools, hot/spent forms, unspooling,
cutting, planting, grounding, cooling, lifting and breakage.

Service passages alternate wire/spike supply within the equipment budget. Both
appear in Ice rewards and shop stock. Maintenance lockers now roll 20% coal,
20% sealant, 15% valve, 15% ammo, 10% wire, 10% spike, 10% empty.

## Validation

Strict game, static-render tool and existing snapshot target builds passed. The
existing codec fixture round-trips a worn wire, hot and cool-spent stakes, a
damaged recovered spike and a partly used Durable spool, including unchanged
truncation/input checks. New PNG dimensions and decoded audio finiteness/headroom
were checked. Static `circuits` and `circuit-items` captures were inspected for
connections, distinct stake states, readable descriptions and stack/condition
display. No live playtest or new test suite was run; combat balance remains for
the user's playtesting.
