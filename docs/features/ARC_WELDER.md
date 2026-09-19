# Arc Welder and repair bays — 2026-09-15

Implemented Industrial catalog enemy 13 and its optional room composition.
This continues the request for useful equipment paired with meaningful places
and creatures to use it on. Industrial now has seven of its twenty catalog
behaviors and nine of fifty regional items; the catalog remains unfinished.

## Encounter

The 70-HP welder approaches at a 22-tick step interval. At two-cell range it
lowers its helmet, audibly hinges/clunks the mask and waits 42 ticks (0.7s).
It commits both its origin and facing, then sweeps left, center and right across
three lanes, each two cells deep. Pulses are nine ticks apart. Each lane deals
seven electrical damage at contact, then it recovers for 72 ticks (1.2s).
The visor stays down during the sweep and rises during recovery.

Moving behind it or out of its committed fan avoids the direct attack. A hit
of at least ten actual damage interrupts it; weak four-damage punches do not
permanently lock it down. Sleep, stun, airborne displacement, rooting and moving
it away from its committed cell also cancel the attack. No hidden armor rule.
The debug attack preview shows remaining lanes, behind the existing toggle.

Water and wire use shared bounded conduction with reach three. A lane's two
contact origins capture their victims before dealing damage and merge duplicate
victims, retaining the stronger distance. One actor cannot receive doubled
lane damage from overlapping paths. Later lanes are separate pulses. This can
shock the welder and other enemies too. A ready grounding spike sinks the whole
lane pulse and is then spent; it does not protect against all three lanes.

Walls, closed gates and solid corners stop contact lanes. Grates, scrap bins
and ore bins receive fourteen cutting damage. The far contact is not traced
through metal destroyed by that same pulse. Water branches obey existing
conduction cover rules. The behavior also works in an ordinary room without
its authored setup.

One death roll: 25% Arc Torch, next 15% Copper Wire, otherwise nothing. The
planned Welding Visor is not silently substituted with a fictional item.

## Room placement

Industrial may reserve one eligible non-objective room as a Repair Bay, with a
two-thirds selection chance. Existing workfront, blasting and assembly rooms
retain their roles. The bay is a broad dry clearing: an offset 3×3 shallow-water
work area, a welder standing on its dry bank, a ready grounding spike beside
the water and two grate benches on the opposite side.

The mirrored footprint is tried both ways. Every cell must be clear, inside
the room, away from spawn and outside protected routes before any mutation.
Insufficient entity capacity leaves the stage unchanged. If the authored setup
cannot fit, an ordinary room welder spends the same two-point encounter budget.
No extra free Arc Torch is planted beside it. Drain/toolbox dressing is pending.

## State, assets and checks

Shared entity slots hold phase (`label_a`), phase timer (`timer_a`), next lane
(`counter_a`), committed origin/facing (`point_a`/`point_b`). Hearing retains
c-slots. Snapshot decoding validates the phase state. Gameplay compatibility
is `0x2026091538`; snapshot layout 50 and wire format 14 are unchanged.

Two new bitmap poses use the standard held-tool renderer for the Arc Torch.
Two offline sound cues, mask hinge/clunks and death grunt, come from
`tools/sound/arc_welder.py`; existing Arc Torch pulse and metal-cut cues are
shared. They participate in ordinary hearing. Audio peaks are restrained.

Strict Release build passed. A temporary direct-function check covered warning,
lane cadence/recovery, committed aim, heavy-hit/displacement interruption,
water overlap and self/ally shock, grounding consumption, cover, cutting without
same-pulse penetration, and snapshot/hash continuation during the warning.
Repair-bay placement and no-mutation protected-footprint rejection passed.
Sixty-four generated Industrial floors retained reachable objectives and required
exit locks; they contained 48 welders. Static SDL-dummy repair-bay and discharge
renders were inspected. No live playthrough or new permanent test suite.

## Bitmap provenance

Made with the built-in imagegen tool; generated alpha is preserved. Idle source
`exec-73b88fc6-c54d-45b5-ad96-61476fa8583c.png`, copied to
`assets/graphics/arc_welder.png`. Generation prompt:

> One minimalist chunky pixel-art enemy sprite for Teeming, 2D top-down roguelike. A squat human industrial welder, seen from slightly above/front. Stocky muted ochre leather apron over dark charcoal overalls, heavy grey boots, short thick arms with copper-brown mittens hanging at sides. Head is a rectangular dark welding helmet with visor RAISED above a small pale grey face, two tiny dark eyes. No tool in hands, game draws that separately. Logical 16x16 pixel aesthetic, large flat simple blocks, sparse 5-colour palette, no outlines, no internal texture or gradients. Centered square composition, full body fills 85% height and 70% width. NO light, glow, halo, particles, text, props, floor, shadow or background. True transparent background with alpha. One sprite only.

Mask source `exec-b2dab393-6a93-4404-9513-6e4ce83210dd.png`, copied to
`assets/graphics/welder_mask.png`. Edit referenced the inspected idle source:

> Edit this one game sprite into its welding warning pose. Preserve exactly the body, proportions, palette, pixel-block style, canvas position and transparent background. Change ONLY the welding helmet: lower the raised visor down over the face, hiding the face and eyes completely behind a dark charcoal rectangular welding mask with one small pale grey horizontal viewing slot. The raised plate no longer sticks up above the head, it has hinged down covering the face. NO glow, lighting, sparks, particles, tool or background. One sprite, true transparent alpha. Keep the heavy mittens, ochre apron and boots unchanged.
