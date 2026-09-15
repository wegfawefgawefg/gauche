# Stillwater Bell and shallow currents

Implemented Ice item 46/50. Ring to calm a sound-reachable area within four steps
for three seconds. Walls, blocking props and closed doors constrain the area.
The fixed patch stops ice/oil slips, clears existing slide momentum and pauses
floating cargo without spending its remaining travel. Voluntary movement, attacks,
ordinary shoves and water hazards continue. Leaving the patch loses its protection;
expiry restores normal slipping and drift. New arrivals receive the ground effect.

Four uses, value 30, one-second cooldown. Durable has eight uses; Big extends the
radius to five. Other attributes are unsupported. A second ring refreshes time
without stacking it; a fully fresh identical area spends nothing. The area has a
quiet broken outline, still water animation and a STILL GROUND timer on occupants'
HUDs. New bell/rundown/drift sounds are generated offline, with one new bell sprite.
The ring can wake and attract existing listeners; it does not stun enemies.

Forest brooks and Ice reservoirs now supply gentle southward shallow currents,
placed outside reserved routes. Once per second, each eligible walker or loose
item drifts at most one real cell. A blocked landing stops drift; currents never
crush against banks or pull into impassable deep water/lava. Flying creatures,
eels, seals, divers, hard blockers and gripping/rooted actors resist the drift.
Movement retains the actor's aim and voluntary movement recovery.

An air-bladder float keeps its existing twelve-tick beat and finite sixteen-cell
travel allowance. Currents steer it when present; calm pauses it. It is excluded
from the separate once-per-second drift phase, so there is no double movement.
Hooks or other displacement retain the existing float cancellation rules.

Freezing a stream suspends its current; thawing restores the saved direction.
A Folded Bridge similarly covers the current, and a broken plank restores it.
The calm field does not melt ice, clear oil, prevent burns or grant cold resistance.
Existing tools therefore interact with this system without new per-enemy rules.

Bell Divers have a 10% bell drop in addition to their existing 20% bladder and
25% coin pools. Ice rewards and shops also include it. Required objectives are
never placed in a current by the generator's protected-path rules.

Tile current direction (0 none, 1 east, 2 south, 3 west, 4 north) and surface
still_ticks are deterministic saved/hashed fields. The decoder rejects invalid
directions and calm durations over 180 ticks. Snapshot format is now 38; gameplay
compatibility is `0x2026091515`. Cosmetic flow marks, rings and sound playback are
local and do not change the simulation.

Validation: strict game/render builds; focused direct checks for drift speed,
blocked/deep banks, swimmer/flyer resistance, calm ice/oil, retained walking/shoves,
float suspension/resume, freeze/thaw memory, wall occlusion, variants, use count,
snapshot/hash coverage and malformed fields. Static seed-1701 generation across
four Forest and four Ice floors produced 91 and 46 current cells respectively.
Flow, calm and inventory captures inspected with dummy SDL drivers; no autonomous
playthrough or new permanent test suite.

Asset scripts: `tools/art/stillwater_bell.py`, `tools/sound/stillwater_bell.py`.
Static render modes: `stillwater-flow`, `stillwater`, `stillwater-items`.
