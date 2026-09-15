# Pressure Rat

Implemented 2026-09-15: a small Industrial creature whose carried boiler bladder
becomes a visibly inflated, committed lane hazard. This supplies a different
kind of encounter from the workers and gun emplacements, and gives cold/water,
cover and decoys another use.

## Behavior

22 HP. It roams at a 12-tick movement interval. A visible player or shared straw
decoy within seven cells can attract it; it approaches until cardinally aligned,
then inflates for 36 ticks (0.6 seconds). The enlarged sprite grows in place and
a rising rubber/kettle whistle announces the fixed heading.

After the tell, it rushes one real cell every six ticks. It never steers toward
the moving target. The first blocking terrain, prop or creature causes a burst
at the rat's current cell. Even a newly broken obstruction stops that charge.
The burst deals 8 damage to props and actors in the center and four cardinal
neighbors, using ordinary sight/blocking rules. Allies can be hurt; bucklers
can block. It does not ignite the floor or pass steam through a wall. The rat
dies once before applying splash damage, and the ordinary death path rolls its
loot once. Killing it before collision vents it safely, with no delayed blast.
A 24-cell pressure limit also bursts it in an otherwise open lane.

Cold, water/quenching, roots, displacement, sleep, stun or being thrown vent the
pressure. A deflated rat flees for two seconds at an eight-tick interval rather
than immediately attacking again. Wet ground, continuing chill, and ongoing
control effects keep it deflated; normal movement slowing still applies. A
surviving dry rat can eventually recover and prepare a fresh charge. No external
bomb entities or invented projectile hit positions are involved.

This creates several practical choices: sidestep the committed lane, shoot it
before launch, cool it with carried Ice gear, or intercept it with a barricade.
Narrow shots still pass through the barricade, but the rat's body stops there.
The one-cell burst hits the panel, leaving a player two cells from the rat safe.

## Integration and remaining habitat work

Ordinary Industrial encounter selection now chooses a Pressure Rat one quarter
of the time, Rivet Gunner one quarter, or Ember Stoker one half. A rat costs one
threat point; those other enemies cost two. Later floors can place a pair through
the existing encounter rule. Forest/Ice selection is unchanged. Named room
encounters keep their existing populations and budgets.

One death roll: 25% Raw Meat, otherwise empty for now. The proposed 10% Rubber
Hose range remains empty until that item is implemented. Dedicated pipe crawls,
outlets and gnawing/refill ecology remain habitat work; this milestone makes
the creature work independently with its carried boiler bladder. It does not
claim to implement the entire planned pipe scene.

## State, presentation and verification

Shared slots: `label_a` phase, `timer_a` warning/cooling, `counter_a` remaining
rush cells, `point_a` expected position, `point_b` fixed cardinal heading.
No additional entity fields. Decoder validates phases, bounds, direction and
spent state. Gameplay compatibility is `0x2026091542`; snapshot 50/wire 14 stay.

Three new transparent sprites distinguish roaming/inflation/dash. Facing is
rendered cardinally; only inflation changes sprite size, with the ground anchor
unchanged. Existing local steam particles accompany a new burst cue. Four
sounds are synthesized offline by `tools/sound/pressure_rat.py`: inflation
0.6s, rush 0.3s, burst 0.55s, safe death vent 0.25s, peaks 0.23–0.34.

Strict release build and a temporary direct-function check passed: full tell,
committed dash after target movement, body/cover collision, single burst,
water/cold/control interruption, safe early kill, pressure-distance limit,
snapshot replay and invalid-state rejection. Sixty-four generated Industrial
floors retained required reachability and locks, and contained 48 rats. Static
SDL-dummy inspection showed all three poses at game scale. No autonomous live
playtest or permanent new test suite. Threat/readability remain for feedback.

Generated sources preserved under
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:

- `exec-7a366d6f-e879-44e3-8466-c0d2deffd7df.png` -> `assets/graphics/pressure_rat.png`.
- `exec-bdeadd4c-7bb1-4d48-ad5f-06f2b291a12b.png` -> `assets/graphics/rat_inflate.png`.
- `exec-f73ebdc4-6b81-4b22-a668-a9870cdca98e.png` -> `assets/graphics/rat_dash.png`.

The built-in image generator was asked separately for one grey rat with a
copper back boiler, an inflated copper-bellied rat with taut bands, and a
stretched rushing rat with a short steam puff. Each prompt specified rightward
facing, a view from above, broad flat chunky pixel shapes, grey/charcoal/muted
copper/cream/dull pink, true transparency, no floor/shadows/text/grid and one
sprite. Originals are copied without bitmap edits.
