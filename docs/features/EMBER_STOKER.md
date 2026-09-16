# Ember Stoker

The old Ember's hidden pistol is replaced by a 48-HP furnace worker with a coal
sack and shovel. Existing Industrial encounters and spawners use the new behavior;
the dedicated furnace-aisle room and Slag Scoop remain catalog work.

A loaded stoker finds a cardinal firing lane within eight cells, visibly packs
for 30 ticks, then spits one physical pellet. The lane is fixed during the tell.
The pellet advances a tile every five ticks, does 12 damage and ignites a struck
vulnerable actor for the existing weak burn. Impact can ignite actual oil, sap,
wood or other existing fuel. Bare stone does not become an endless fire source.
Walls, closed gates and blocking props stop it; a prop broken by this impact
still shelters whatever was behind it. The pellet has no dig power.

Six shots are available initially: one loaded, five in reserve. A fired stoker
rests 24 ticks, scoops for 60 ticks, transfers exactly one reserve to its mouth,
and rests 18 ticks before aiming again. Damage, sleep, stun, rooting or movement
away from the windup origin cancels a pending action. Coal transfers/spending
happen only on completion, and projectile-capacity failure leaves loaded coal
intact. With no coal left, the stoker approaches and commits to a single-cell
30-tick shovel attack for 14 damage with 36-tick recovery. Stepping away avoids it.

Water or cold spoils the next ignition without erasing finite fuel. A damp shot
is grey, deals four impact damage, and cannot ignite anything. Wet floor cells
also cool a pellet in flight; direct quenching can catch the projectile itself.
Standing in water keeps the next shot damp. Cold now slows the grounded worker;
its old fire/scorch resistance remains. Traps, puddles, footsteps and loose props
use normal grounded contact rules. Its own flame/warmth exists only while dry
coal is loaded, so a quenched worker no longer perpetually warms its cell.

The Reflecting Pan and Mirror Knight can reverse a pellet, transferring its
checked owner handle; the original hard lifetime prevents endless reflection.
The player's Coal Lump can refill one reserve on an adjacent stoker, spending
one actual lump; a full five-lump reserve rejects it. Refueling does not remove
dampness, reset hostility or grant a free attack. One death roll has a 25% chance
of returning up to two still-unspent lumps. An empty sack returns none.

Four native 16px worker poses and hot/cold pellet sprites accompany seven new
offline OGG sounds: cough/tell, coal scoop, spit, impact, heave, shovel swing and
death. Worker boots use the mine crew footstep pair; shots create semantic noise
independent of local playback. Existing debug attack patterns show the fixed
firing lane and committed shovel cell.

The existing Ember entity kind now stores phase, fuel and dampness in shared
labels/counters; checked projectile handles and travel clocks use the shared
projectile struct. Snapshot 42 and gameplay 0x2026091523 cover the new behavior,
sprite IDs and projectile kind. Decode validates fuel and hot/cold damage state.

Validation: Release game and renderer builds passed with no compiler warnings.
Focused direct-function checks cover delayed/fixed shots, ammo transfer,
displacement/damage/stun interruption, water/cold, hot/cold impacts, wall cover,
shovel dodging, refueling limits, reflected ownership, airborne quenching,
entity-capacity failure and snapshot/hash round trips. Four generated Industrial
stages retain reachable objectives and required locks. Static hot/cold scenes
were inspected; no autonomous playthrough or permanent test suite was added.
