# Rabbit Charm and Blink Seed — 2026-09-15

Two existing Forest toys now have distinct jobs, following the user's revised
item direction. These changes do not complete the separate gap/tar/pit generation,
jumping, flying or reactive-dodge proposals.

## Rabbit Charm

Holding the charm in the selected hand doubles movement recovery, without using
charges or pressing Use. For the ordinary seven-tick player movement interval,
this means a four-tick beat. Stowing it removes its benefit; carrying duplicates
adds nothing. Existing haste shares the same recovery ceiling rather than
multiplying it. Ice/grip slowing, roots and normal ground contact still apply.
Attack speed is unchanged, and fighting requires switching to another item.

The charm no longer retreats or teleports on Use. Its description and detail
panel explain held speed and no charges; it remains one nonstackable item with
its existing rabbit silhouette. No new actor state or visual-position smoothing
was added. Footsteps continue to originate at the real movement destination.

## Blink Seed

The former Swap Seed is now **Blink Seed** in the item catalog, display names,
item-use dispatch, attributes, supply references and debug/static scenes. Its
stable enum position is retained; the projectile behavior/file is named Blink.
The existing seed sprite and fold/fizzle recordings are reused. Successful travel
has matching purple departure/arrival flashes, without the old spray of little
seed sprites that obscured the effect.

A seed flies one cell every six simulation ticks, up to five cells (Long extends
that reach), with a rendered throwing arc. It consumes one from its stack at
launch. Native supply gives two seeds; the stack limit remains three. It does not
need a target creature and never swaps or pushes another actor.

- At maximum range, blink to the landing cell. Intermediate water can be crossed;
  a nonwalkable final cell searches back along the traveled line for a landing.
- Walls, blocking props, closed doors, sealed gates and solid actors end flight
  on the near side. Occupancy is checked again at impact; if that near-side cell
  has become occupied, search backward along the traveled path.
- The caster may move during flight. Death or a stale generation handle prevents
  travel. Roots prevent the blink, and the existing sealed-encounter boundary
  rule also applies if the arena changed while the seed was airborne.
- No legal destination, or landing on the caster's current cell, produces a
  fizzle; the thrown seed stays consumed. No refund or arbitrary nearby search.
- At arrival, ordinary fire/water/prop/trap/spring/linked-door contact runs.
  Teleportation does not silently grant hazard immunity. Facing is preserved,
  movement recovery is imposed and stale melee/bow action is canceled.

All flight state uses the existing saved entity slots. Gameplay version advances
to **0x2026091533**; wire stays **14**, snapshot layout **49**. Rebuild all peers
together; old gameplay builds must not join this one.

## Validation

Release game and static renderer builds pass. Temporary direct-function checks
cover six-tick travel, range landing, one-seed consumption, both flashes, walls,
locked doors, occupied targets, moving/stale/rooted casters, fire contact, sealed
arena boundaries, occupied near-side fallback and crossing water to a bank.
A snapshot taken during flight restores to the same final hash. Rabbit checks
cover held/stowed behavior, four-tick movement, shared haste ceiling, grip/roots,
no retreat and snapshot validity. No autonomous playthrough or permanent test
suite was added. Static item cards and the flight/flash scene are inspected
separately; live balance and feel remain for human feedback.
