# Nozzle Elbow — 2026-09-15

Industry now has twenty-one of fifty regional items. The Nozzle Elbow costs
14 gold, appears in native stage-one stock/reward/cache/secret/workshop tables,
and fills the Emergency Pump's formerly reserved 15% drop slot. Cooling works
now offer equal chances of an elbow, water-filled Pocket Pump or Coolant Can.
The two machine outlets and neighboring hot/cold/fuel surfaces make these
supplies useful in that encounter.

Use attaches the actual fitting to an adjacent boiler or Emergency Pump.
Secondary changes its clockwise/counterclockwise bend before attachment. Future
jets turn ninety degrees from the machine's ordinary aim; an already warned jet
retains its original committed direction. Removing or breaking the fitting also
leaves an existing warning alone. A boiler porter's ordinary aiming remains
active beneath the elbow; the existing Pressure Valve still locks its straight
outlet away from the user.

The fitting has eighteen HP, thirty-six with Durable. Real health hits also
wear it, and breakage removes it. Damage does not become free armor for the
machine. Ordinary body damage/control interruption rules still apply. It is a
steel item for magnet/Crane attraction while loose or held.

## Shared fitting lifecycle

`items/machine_fittings.*` owns the common recoverable socket for Pressure Valves
and elbows. A filled socket refuses replacements rather than deleting hardware.
Boilers allow recovery while idle and below twenty-five pressure. Pumps permit
recovery while roaming or recovering, once the short visible jet has ended;
filling and warned jets remain busy.

Normal pickup/swap controls and prompts recover the actual item. Full inventory
without a droppable selection and exhausted entity pools leave it attached.
Machine death salvages a surviving fitting with its exact remaining condition
and handedness. A killing hit that breaks the fitting cannot also drop it intact.
Existing Pressure Valve pickup behavior is retained in the shared path.

Item `loaded` stores the bend (0 clockwise, 1 counterclockwise); `durability`
stores fitting HP. The machine's existing `ground_item` stores the actual fitting.
No new entity or wire fields. Validation rejects impossible bends, condition,
stacking and socket contents. Gameplay compatibility: `0x2026091560`.

## Presentation and validation

A small attached elbow sprite and the ordinary nozzle show modified outlets;
current warnings continue to display the direction they will actually fire.
The quickbar names L/R and HP; details show recoverability, condition and the
current secondary binding. The debug loadout editor selects handedness and wear.

Original transparent sprite `assets/graphics/nozzle_elbow.png`, generated with
the built-in image tool. Source retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-ca6e6a92-ed35-4bb5-b869-49aafa910c63.png`.
Prompt: isolated chunky L-shaped steel pipe elbow with brass threaded collars,
small ivory turn-arrow, sparse top-down pixel clusters, muted charcoal/brass/sage,
transparent background, no floor/hands/glow/shadows/scenery/text. Four original
threading/latch/fracture sounds come from `tools/sound/nozzle_elbow.py`, peaks
0.20–0.30. Breakage produces local chain-link scraps. No music work.

Strict release build and temporary direct checks passed: both bends across four
cardinal directions, unchanged existing warnings, redirected real pump jets,
boiler arming, exact recovery/salvage, fitting damage/breakage, Valve socket
compatibility, capacity failures, snapshot continuation/rejection and debug
normalization. Sixty-four Industry floors kept reachable routes and required
locks, with three elbows placed. Four sounds loaded in an SDL dummy render.
The static item/attachment render was inspected; its first description was too
long, so it was shortened to fit the details panel and condition was made visible.
No autonomous playthrough or permanent test suite. Live balance remains for
player feedback.
