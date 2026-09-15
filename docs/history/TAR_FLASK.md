# Tar Flask and tar material — 2026-09-15

Industry now has twenty-four of fifty regional items. This is the shared material
foundation for the still-pending Tar Choir, plus a player tool with existing
world interactions and actual acquisition points.

## The flask

The eight-gold bottle comes in stacks/bundles of three. It travels up to five
cells at eight ticks per cell before shattering into a five-cell cross. Existing
flask collision, overhead travel and cover rules apply; it cannot paint through
walls or bridge water/gaps. Big expands the cross, Long extends the throw, and
invalid damage/condition modifiers remain unavailable. Cooldown: 0.75 seconds.

Native stage-one shop/reward/cache/secret/workshop membership lives in the master
supply table. Assembly belts now carry three Tar Flasks for one-third of their
small-item cargo rolls, otherwise the existing three-handful Bolt Pouch. The
other lane still carries its real Nail Board. No extra equipment allocation.
This explicit route was added after a 64-floor sample found tar terrain but no
bottles from generic loot; the item should have a place to find it now.

Kiln courts choose oil or tar for their offset three-cell fuel lane, with equal
chances. Clear dry margins and protected routes remain. Their coolant/foam
supplies wash or suppress the lane; nearby heat and the kiln make careless
ignition dangerous. These authored large patches last thirty seconds; flask
patches last ten.

## One shared surface

`Tar` and `ClottedTar` extend the existing liquid enum; there are no new per-tile
fields. Liquid tar adds twelve movement-recovery ticks to grounded walkers,
including players and enemies; flyers/tossed actors avoid ground contact. Sled
pulling also pays surface drag. Both tar states suppress slipping on bare ice.

Cold Flask's surface-freezing action changes tar to clotted tar without changing
terrain or resetting its remaining lifetime. Pouring tar onto native ice clots
it immediately. Crust is passable and no longer slows walking. Applied warmth or
fire softens/ignites it through the shared fuel rules. Fire consumes at most four
seconds of available tar, never extending a short remainder into free fuel.
Spread remains on the ordinary budgeted surface beat; tar has no special
whole-floor propagation loop.

Water/coolant/brine wash it away using the existing spill rules. Hot contact can
ignite freshly poured tar. Unburning liquid tar can be collected and discharged
by either pump, retaining actual partial quantity; hardened crust cannot be
pumped. Tank names, debug choices, pump gauge/jet color, surface colors and
snapshot validation recognize it. Gameplay compatibility: `0x2026091563`.

The common fuel check also rejects exhausted zero-lifetime liquid, and the
movement-delay helper rejects expired residue. The Tar Choir and its settling
tank encounter/drop table are deliberately still unimplemented; this milestone
does not count another enemy.

## Assets and evidence

`assets/graphics/tar_flask.png` is the original transparent built-in imagegen
sprite, copied without stripping alpha. Retained source:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-9b433089-87f5-4358-9946-ef258bbec9ae.png`.
Prompt: isolated squat thick-glass tar flask, cork and cloth tie, viscous dark
contents, muted amber/sage edges, a few chunky pixel clusters, transparent
background and no floor/shadow/text. Four original throwing, viscous splash,
cooling-crust and reheating sounds come from `tools/sound/tar_flask.py`, peaks
0.19–0.30. Local colored ripples indicate splash and clotting; ordinary shared
fire visuals remain. No music work.

The strict build initially caught an ambiguous boolean expression; that was
corrected and the release build passed. Temporary direct checks passed real
flight/consumption, midflight snapshot replay, walker/flyer movement, cold/native
ice crust, finite burning and normal spread, washing, exact partial pump
transfer, hardened-material refusal, cover, snapshot validation and static
presentation. The final 64-floor sample kept reachable routes and required locks,
with 23 three-flask bundles and nine tar-lane cells. Four sound files loaded with
SDL dummy audio; the flask/details and liquid/crust static render were inspected.
No autonomous playthrough or permanent test suite. Live balance remains user work.
