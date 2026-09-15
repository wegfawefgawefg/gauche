# Powder Monkey and blasting alcove

Industrial generated floors reserve a dry blasting alcove, separate from the
mine crew's workfront. Its short optional stone spur sits off the protected route.
One Powder Monkey costs two threat points before incidental encounters; nearby
Fuse Scissors introduce its counterplay. Neither special room can be overwritten
by secret-room selection. Main objective and exit generation remain unchanged.

## Charges and counterplay

The 42-HP monkey has two unarmed Quarry Charges. With a nearby target it searches
reachable cells within six steps for dry staging space beside ordinary stone or
breakable blocking cover. A bounded cardinal search excludes lava and occupied
cells. It prefers nearby cover around the fight and moves toward it, searching
at most once per twelve ticks. It cannot plant at an arbitrary player position.

At its actual staging cell the monkey creates a separate physical charge with a
two-second fuse. It spends 24 ticks lighting/placing it, then takes fast six-tick
steps toward cells outside the charge's forward lane and rear blast. The same
charge remains at that cell if the monkey dies, gets stunned, is displaced or
loses sight of the player. Finite fuse timers continue independently of its
behavior. A failed entity allocation consumes no charge.

The bomb uses the existing directional Quarry Charge impact: at most three stone
cells ahead, 24 damage in that lane and the charge/rear cell. Reinforced exits
and outer boundaries still stop the cut. Water turns it into the actual ground
item, cold slows its fuse, and scissors recover it if pack space permits. A
quenched/removed charge ends the monkey's retreat; generation-checked handles
prevent a newly reused slot from becoming its old bomb. After recovery it may
spend its second charge, but it cannot generate an infinite supply.

Empty monkeys flee. If adjacent to a target they show a 24-tick claw tell aimed
at one fixed cell, dealing six damage with a 30-tick recovery. Dodging, damage,
sleep, stun, rooting or displacement can spoil the swipe. Interrupting the
lighting action switches to escape without relocating or deleting the bomb.

One death roll offers 25% one unspent charge, next 15% Fuse Scissors, otherwise
nothing. A monkey that already spent both charges cannot drop another on that
first roll. An already-armed bomb remains separate from the drop table.

## Art, audio and state

Four native 16px sprites distinguish the small red work vest, curled tail,
lighting pose, running stride and claw tell. Seven generated OGG cues cover cork
pop, panicked chatter, claw warning, scratch, death and two quick bootsteps.
Quarry fuse/spark/explosion assets are shared with the player tool. Local audio
and debris do not affect AI or deterministic state.

Shared counter_a stores remaining charges, entity_a the checked bomb handle,
label_a/timer_a the phase, timer_b the search delay, and point_a/point_b committed
origins/targets. No per-species fields were added. Snapshot 44 and gameplay
0x2026091525 cover the new kind and population, with phase/fuel decode validation.

Validation: Release game and renderer builds, focused direct-function checks for
actual dispatch, finite separate charges, retreat, death/stun/displacement
persistence, quenching, stale handles, dodging, entity-capacity failure and
snapshot/hash round trips. Sixteen generated Industrial stages contain the
reserved monkey and preserve required locks/reachable objectives. Static
lighting and escape poses were inspected. No autonomous playthrough or permanent
test suite.
