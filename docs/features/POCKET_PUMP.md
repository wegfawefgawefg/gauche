# Pocket Pump — 2026-09-15

Industry now has twenty of its fifty regional items. The Pocket Pump is reusable,
starts with three water pours, and costs 23 gold. Use collects an adjacent real
spill; secondary discharges one portion ahead. Both successful actions have a
0.5s cooldown. Half of cooling works supply it instead of a Coolant Can, and it
joins native stage-one stock, rewards, containers, treasure and workshop tables.

One tank holds one liquid: water, oil, sap, honey, rot, brine or coolant. Full
tanks, incompatible refills, blocked landings, frozen/burning sources and slag
refuse without consuming anything. It does not drain terrain lakes, springs or
lava. Partial spills transfer exactly, without rounding them into full puddles.
Same-liquid pours merge up to the existing surface cap, leaving excess in the
tank. Unlike non-water pours refuse; water can wash other spills away through
the ordinary surface rules.

Water quenches floor/actor/fixture fire and cools lava. Coolant also chills actors
and reduces boiler pressure, scaled down for tiny partial pours. The existing
can and pump share `splash_coolant`; the can retains its original full strength
and three-cell footprint, clipped by ordinary cover/sight rules. This makes
transport useful for clearing sticky ground, arranging fuel, interrupting hot
creatures/machines and carrying cooling liquid between encounters.

## State and presentation

Existing Item fields hold liquid kind (`loaded`) and remaining quantity (`spare`).
One full pour retains 600 surface ticks; the sealed tank holds 1800. Storage stops
evaporation, while discharged floor liquid ages normally. Dropping, trading,
stowing and snapshots retain this exact state. No new wire fields. Validation
rejects impossible liquids, out-of-range amounts and contradictory empty states.
Gameplay compatibility version: `0x2026091558`.

The quickbar shows a compact liquid initial/percentage and fill meter. Full
details name the liquid, state that the tool is reusable/non-stackable, and show
the current secondary-action binding. Debug loadouts can select liquid and fill
percentage; normalization preserves valid loads. Simultaneous use/secondary
chooses discharge once.

The original transparent sprite is `assets/graphics/pocket_pump.png`, generated
with the built-in image tool. Original retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-03991b71-b9d0-4d7c-9f8d-6fa49842338d.png`.
Prompt: isolated minimalist top-down pixel item, sage-green metal pressure tank,
brass piston handle, curled dark hose and brass nozzle, sparse angular clusters,
transparent alpha, no hands/scenery/text/glow/cast shadow. The sprite was inspected.
`tools/sound/pocket_pump.py` creates two original suction/discharge OGGs at peaks
0.22 and 0.23. No music changes.

## Verification and adjacent fix

Strict release build and temporary direct checks passed: finite/partial transfer,
capacity and mixed-liquid refusal, terrain/heat/frozen restrictions, dousing and
lava conversion, coolant actor/boiler behavior, existing-can parity, stowing,
debug normalization, snapshot continuation/rejection, combined input and full
cosmetic buffers. Sixty-four generated Industry floors kept required locks and
reachable routes; two pumps appeared. Both sounds loaded in an SDL dummy render.
The inventory render at `/tmp/teeming-pocket-pump.png` was inspected; quickbar
text was shortened after the first render exposed cooldown overlap.

The same material review fixed Hoist Wreck being mistaken for dry vegetation by
the fire-fuel predicate. Bare metal/stone wreckage no longer self-fuels a fire;
oil poured onto it still burns. A direct check covers both cases.
No autonomous playthrough. Live balance remains for user feedback.
