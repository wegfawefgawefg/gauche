# Strikebreaker

Industrial enemy five of twenty. Guards join existing workfronts from the second
Industrial stage onward, reserving three additional threat points before incidental
encounters. They begin neutral and escort their generation-checked foreman rather
than hunting every player who enters. No new autonomous playtest was run.

A guard has 150 HP, a finite 90-HP shield and a 20-tick walking beat. The shield
blocks ordinary frontal attacks through the same facing/blockable rules as player
shields; rear attacks, unblocked area damage, sleep, stun, chill, rooting and attack
recovery expose the guard. Blocking spends shield HP without overflow into health.
Breaking it causes a one-second recovery, and it stays broken. The separately
drawn shield follows the actual facing edge and lowers when it is not protecting.

Harming a linked nearby worker, foreman or the guard itself records the real
attacker for ten seconds. Same-crew accidents are ignored. Other crews and workers
behind sight-blocking cover do not magically alert this guard. Missing/dead threat
handles cancel pursuit; missing foremen leave the guard near its original post.
A nearby worker's death can still alert the guard before corpse removal.

Adjacent combat begins with an 18-tick shove tell. The guard tries one cardinal
shove, respecting facing shields, grip and ordinary occupancy. Hard obstacles can
crush through existing rules; entry into water/ice/hazards uses the normal contact
path. Then a 42-tick overhead tell commits to the victim's resulting cell, at most
two cells away. Moving away works. It hits whatever is in that cell for 32 damage,
including an ally, then leaves 54 ticks of recovery. Springs or teleports triggered
by the shove never extend the hammer beyond its two-cell reach. Displacement, damage, cold,
stun or sleep interrupts pending actions. The shield is down during both tells.

Pursuit reuses the bounded crew walking/digging route scorer. An obstructing
ordinary rock/prop needs a full overhead tell; impact does 32 terrain damage at
dig power 1 and 64 prop damage. Reinforcement and outer boundaries retain their
break rules. Existing score estimates are conservative for this stronger hammer.

One drop roll: 0–19 Press Hammer; 35–54 five to nine gold; remainder empty. The
catalog's Steel Toe Cap is still unimplemented, so its reserved range stays empty.
Six native sprites and ten offline sound cues include shield block/break, brace,
overhead heft, shove, slam, alarm, death and separate heavy steps.

## Direct projectile attribution

Damage now accepts an optional generation-checked instigator separately from the
incoming contact cell. Arrows/bolts, recoverable throws, harpoons, snowballs, ice
bricks, frost/coal pellets, widow hooks and hitscan bullets pass their current owner.
Physical impact direction still decides shielding; retaliation remembers the
actual shooter. Reflected shots carry their new owner. A stale owner cannot blame
a newly allocated actor in the old slot. This also repairs mine-worker retaliation
against those traveling hits. Blast/beam ownership propagation is not added here.

## Verification

Release game and static renderer builds pass. Temporary direct-call checks cover
neutral behavior and crew alerts, finite front/rear shielding, cold exposure,
shove/hammer timing and dodging, an actual spring escape without extended hammer
reach, displacement/stun interruption, a real traveling
arrow's shooter attribution, stale owner reuse, eight generated Industrial floors,
required gates and snapshot/hash roundtrips. The hammer/boiler checks also pass
after the shared combat changes. Static `breakers`/`breaker-raise` captures inspect
the guard, lowered shield and broken-shield appearance. No new permanent tests.

Snapshot format 46 and gameplay `0x2026091527` require matching rebuilt peers.
Combat feel and balance remain for human feedback.
