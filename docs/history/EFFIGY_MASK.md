# Effigy Mask

Implemented Ice item 47/50. Hold Use and stand still to watch behind you as well
as in front. Snow effigies already freeze under ordinary player observation; the
mask extends that same rule into the opposite ninety-degree cone. It does not
stun other enemies, remove an effigy shell or grant sight through cover. Both
cones use the existing Manhattan-seven sight limit, walls, smoke and whiteout.
Side blind spots remain. This lets one stationary player cover two approaches
while a teammate moves, with ordinary front observation costing nothing.

Twelve seconds of total wear, twenty-four for Durable; no other attributes.
Value 28. Partial seconds survive stowing, dropping, trading and snapshots;
tapping Use does not refill wear. Movement intent, displacement, release, cancel,
interaction, sleep, stun and death lower the mask. Valid held Use can raise it
again after interruption. The final worn tick destroys the mask. Direct item use
has no instant effect: the player continuous-action phase owns it.

Item spare stores 0..59 worn ticks within the current second; uses counts the
remaining seconds including that partly worn one. opened is the active worn flag.
Player label_b uses quick_slots+1, outside the bow/brick slot range; point_b is
the stationary stance. Existing action cancellation prevents a traded active mask
from passively granting rear gaze. Fractional wear and these slots are already
saved and hashed. The decoder rejects spare >59 or a loaded mask. Snapshot format
remains 38; gameplay compatibility is now 0x2026091516.

Snow effigies drop a mask 10% of the time, candles 20%, nothing otherwise.
Alternating memorial courts include a mask; Ice shops and rewards include it.
A small awake face appears behind the wearer. HUD and inventory show remaining
wear, with a timed MASK WATCHING indicator. The inspection diagram distinguishes
solid rear gaze from outlined ordinary front gaze. Four new sounds cover raising,
lowering, quiet wear and breaking; raising is audible to existing listeners.

Validation: strict game/render builds, direct checks of cones, cover, interruptions,
fractional wear, snapshot/hash round trips, malformed charge state, attributes
and depletion. Static world/inventory captures inspected with dummy SDL drivers.
No autonomous playthrough or new permanent test suite.

Assets: tools/art/effigy_mask.py and tools/sound/effigy_mask.py.
Static render modes: mask and mask-items.
