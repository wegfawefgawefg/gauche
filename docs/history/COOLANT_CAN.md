# Coolant Can and cooling works

Implemented 2026-09-15. This is a practical Industrial tool with shared world
consequences: extinguish a lane, stall a machine, turn a small lava crossing to
stone, or leave a slippery conductive patch that may help either side.

## Can and residue

Four uses, eight with Durable; one can per slot, price 12. A pour affects the
three adjacent cells across the facing direction, clipped by existing cover and
corner visibility. No exposed cells means no use spent. Pouring onto ordinary
dry ground is allowed because the residue itself has uses. The entire footprint
is captured before any terrain change. There is a 0.75s cooldown; the fourth
pour removes the empty can with its own sound. Ammo does not refill it.

A pour applies 10 seconds of coolant through the common liquid system. Like
water it extinguishes surface/actor/fixture fire, washes out competing liquids,
deflates pressure rats, cools grounding spikes, and turns lava cells to ruin.
It then applies three seconds of chill to susceptible actors, including crane
motors. Direct boiler cooling removes 80 pressure even with a plugged outlet,
without changing fuel or attached fittings. Below 25 pressure cancels a warned
vent. The existing mallet and the can now share pressure/state/pose reduction;
the mallet retains its 40-pressure reduction and refusal at sealed outlets.

Coolant is a distinct muted-green water-based liquid. It conducts existing
water shocks and supports normal wet contact/float/ripple rules. It makes
walking entrants slip one extra real cell, like oil; grip, roots, stillness and
flying retain their normal exceptions. Ice-only traction does not remove that
fluid slip. Coolant evaporates after ten seconds or can be replaced by a water
pour. It prevents new water freezing while present, like brine, and cannot be
used as clean kettle water. It grants no fire or lava immunity to the carrier.
The metal can is included in the shared magnetic-item property.

Native Industrial supply: stage 1, weight 6, rewards/shops/caches/secrets/
workshops. Ore bins gain a 15% coolant range after their existing coal/coin
ranges. It is not in the guaranteed weapon pool. The emergency-pump enemy's
planned can drop remains future work because that enemy is not implemented.

## Cooling works

The existing optional maintenance-room selection (two-thirds chance) now picks
a repair bay or cooling works equally. It does not add another guaranteed enemy
budget on top. Cooling works consumes two threat points and one equipment slot:
a finite-fuel boiler, a pressure rat, a Coolant Can, an ore bin, a short lava
trench and a small initial coolant spill. The boiler starts at 45 pressure and
1800 fuel ticks, facing a four-cell service lane. The whole lane and trench sit
outside the protected central route, with dry ways around both ends.

All mutable footprint cells, room bounds, spawn clearance, existing occupants
and three free entity slots are checked before placement. Failed placement uses
an ordinary rat fallback. Cooling works is not converted into the secret room.
The proposed emergency pump, nozzle choices and larger reservoir are still
future variants, not claimed as completed here.

## Sorting-yard generation repair

The generated-floor check initially found **zero** cooling works. Inspection
showed its layout included offsets inside the generator's reserved central
cross (both axes from -1 through 1). The older sorting-yard layout had the same
mistake, so it silently fell back to ordinary cranes despite its static fixture
capture looking correct.

Both layouts now keep their complete footprints to the sides. Sorting-yard
cranes are at offset (6,-3), worn pickaxes at (2,-3), hot cells at (4,-3)/(4,-2),
and the second bin at (-2,2), horizontally mirrored where appropriate. Every
old protection check remains. The new check counts actual authored components,
not just the presence of a crane.

## Verification and assets

Strict release build passed. Temporary direct checks covered four/eight pours,
blocked use, three-cell lava cooling, extinguishing, pressure-rat/crane chill,
sealed-boiler cooling with fuel/fittings preserved, unchanged mallet behavior,
slip/grip/flying rules, shock conduction, antifreeze versus clean-water washing,
liquid expiry, native membership, and snapshot/hash round trips. Across 64
Industrial generated floors, required reachability and locks passed, with
26 actual cooling works and 34 complete hot sorting yards. Two SDL-dummy static
captures checked the can, colored spill and scene. No autonomous live playtest
or permanent new test suite; tuning remains for user feedback.

Gameplay compatibility `0x2026091543`; snapshot 50 and wire 14 stay unchanged.
Coolant appends one liquid enum value and uses existing saved surface clocks;
no entity or surface struct fields were added.

`tools/sound/coolant.py` generates pour (0.65s), empty-can knock (0.34s), and
slip (0.19s), peaks 0.20–0.26. Sprite source preserved without bitmap edits:
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-2fbd3985-7369-4920-8ba3-cd7b16bd18c5.png`
-> `assets/graphics/coolant_can.png`.

Built-in image prompt:

> One inventory icon for a minimalist top-down pixel roguelike: a squat industrial COOLANT CAN with a short pouring spout pointing upper right, rectangular handle with a transparent hole at top left. Flat muted mint-green can body, charcoal edge, pale grey cap and handle. One simple cream snowflake mark on front, no letters. Three-quarter view from above. 16x16 logical chunky pixel blocks, five flat colours, broad readable silhouette, no scratches, metallic gradients, shadows, glow or texture. Centered square canvas, fill 90 percent width and height. Genuine transparent alpha background. One sprite only, no floor or other objects.
