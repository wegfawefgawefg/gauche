# Frozen Pilgrim

The eleventh implemented Ice enemy is an abandoned pilgrim in a crust of ice.
It appears in shelters; later bathhouse floors can pair it with eels or leeches.
Room campfires give it a real reason to move. The chapel remains planned content.

## Behavior and counterplay

- 48 HP in every state; transforming never heals it. Frozen ordinary hits are
  halved, minimum one. Damage which bypasses blocking, including burn ticks,
  bypasses the crust. Once thawed it takes ordinary damage in full.
- Frozen pilgrims do not attack. They take one step every 36 ticks toward
  reachable warmth within nine cells. One bounded cardinal flood considers
  terrain, bodies and closed doors; it refuses paths through exposed flame.
  With no reachable warmth they wait and search again after 30 ticks.
- Exposed campfires, burning items/actors, lava edges, burning oil and capsule
  patches provide warmth. A lamp's light/color does not. Leech-drained sources
  are excluded using the existing temperature rules.
- Standing warm for 48 ticks (0.8s) sheds the crust. Damage, sleep, stun, a shove
  or loss of warmth cancels this thaw. The pose changes and ice audibly cracks.
- Thawed pilgrims take eight-tick steps. They pursue visible players or decoys;
  a committed adjacent strike warns for 18 ticks (0.3s), deals 16 damage at the
  stored cell, then recovers for 30 ticks. The attack can miss or hit another
  actor. Damage, sleep, stun or displacement cancels it.
- They retain warmth for 240 ticks after leaving heat. On expiry they stop for
  36 ticks to refreeze. Successful chill starts that transition immediately and
  cancels an attack; the shell returns only on completion. Ordinary chill
  immunity, burning resistance and warm-cell rules remain in force.
- Cold pilgrims ignore noise destinations; thawed pilgrims can investigate
  them when they lack a visible target. Extinguishing a shelter fire can make
  the room safe, while a dropped torch can draw a cold pilgrim toward a trap.

One death roll gives 25% 2–5 gold, 15% Hot Broth, otherwise nothing. New wool
scraps scatter locally on death and when a Wool Wrap burns. These light pieces
use the existing debris motion/collision; they do not affect the simulation.

## State and assets

The shared entity fields hold phase, thawed/crusted state, phase and warmth
clocks, heat-search retry, pending freeze cue, strike origin and target cell.
No extra per-enemy storage, floats, local RNG or wall-clock time affects behavior.
The gameplay protocol is F1; snapshot layout is unchanged.

Five original 16px body poses distinguish frozen, thawing, thawed, striking and
refreezing. Seven offline sounds cover ice strain, awakening, refreezing,
warning breath, sleeve slash, crust impacts and death. Wool has its own sprite.

Validation uses a strict build, existing snapshot codec checks, audio headroom
and a static `pilgrims` pose capture. No live playtesting; behavior/balance still
await the user's feedback.
