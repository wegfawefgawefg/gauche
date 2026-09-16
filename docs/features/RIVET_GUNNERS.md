# Rivet Gun and Gunner

The Rivet Gun fires three traveling rivets, nine ticks apart. Each deals nine
base damage and travels up to eight cells at three ticks per cell. The player
gets twelve loaded rounds and twenty-four spare; the shared ammo supply and
ninety-tick reload replenish this ordinary finite magazine. Recovery lasts
thirty-six ticks after the last shot. The descriptor and inventory card explain
per-rivet damage, burst spacing and magazine capacity.

The first projectile fires on use. Releasing finishes the committed burst;
aiming elsewhere does not redirect its remaining shots. The displayed weapon
keeps that same aim. Walking changes the real origin of each subsequent shot.
Switching slots, dropping, cancelling, interacting or reloading cancels the
unfired rounds. Partial magazines fire only their available rounds. Each shot
must allocate successfully before spending ammunition; the remaining burst
aborts on allocation failure. Held use can repeat after recovery. Each discharge
spends one muffling charge; wrapped discharges suppress their noise, while
impacts remain audible.

Rivets reuse the existing physical arrow/bolt behavior with their own sprite and
impact sound. This preserves actor contact, shot direction versus instigator
attribution, parrying with a finite outer lifetime, piercing, strong/heavy/long
variants, solid-cover collision and shoot-through grates. Casings and small
metal impact litter are client-local cosmetics using the existing bounded pool,
friction, raking and push rules. They cannot become ammo or alter snapshots.

The 64-HP Rivet Gunner braces for thirty ticks, fires three nine-damage rivets
nine ticks apart in its committed lane, then spends ninety ticks reloading its
renewable enemy feed. Damage, displacement or control interruptions during the
brace/burst force recovery. A target dodging sideways does not redirect shots.
A bounded local search finds reachable cardinal firing lanes, favoring some
stand-off distance and nearby grates. It does not walk through cover. Its fat
struct uses label_a/timer_a for phase, counter_a for unfired rounds, point_a for
brace origin and point_b for committed direction; hearing uses the c slots.
The displayed gun is cosmetic, not a second inventory or hidden hitscan attack.

During brace/burst, small amber sight marks show the committed direction in
normal play, without enabling the debug damage-grid overlay. They brighten as
the brace completes and stop at actual solid cover; grates, actors and smoke
do not stop rivets. Reload, displacement and incapacitation suppress the cue.
Native poses distinguish planted bracing, recoil and feeding a new strip. A
short air puff accompanies each actual launch, including the last burst round;
local deduplication prevents repeated flashes or replay from an older snapshot.

Blasting alcoves reserve two threat points for a gunner. Placement tries short
three-bar side partitions with open ends outside protected routes, accounting
for existing actors and props. If no safe post fits, the ordinary reachable
room placement supplies the gunner. Incidental Industrial encounters also mix
in gunners. This is a firing post, not the full planned belt/crank assembly room.
Ordinary population also places one mixed gunner/Pipe Guard group where space
allows. It prefers adding an escort to a specialist gunner. A new gunner costs
two ordinary threat points and its escort one; the floor allowance is unchanged.
New gunners stop at a floor total of two on early Industry stages and three on
later stages, and cannot start within eight cells of another gunner. Specialist
rooms retain their own allowance. Posts and escorts stay beyond ten cells of
the entrance, have dry sidesteps and start three to five cells apart, off the
same row/column. Quiet refuges and already crowded rooms stay excluded.

One death roll awards a Rivet Gun at 0–24, Ammo at 25–44, otherwise nothing.
The gun also joins implemented Industrial shop/reward stock.

Seven native PNGs cover gun, rivet, four enemy poses and casing. Five offline
synthesized OGGs provide pneumatic shot, impact, feed reload, brace and death.
Existing crew footfalls are shared. Static `rivets` and `rivet-items` captures
show the firing lane and inventory. All art uses the existing small flat palette.

Validation: release game and renderer builds; temporary direct-call checks of
burst timing and fixed aim, release/cancel/switch behavior, partial magazines,
allocation failure, reload values, actual travel/hit through a grate, enemy
brace/burst/reload and displacement, reachable lane selection, and sixteen
Industrial floor/seed combinations with required objectives reachable and locked.
Snapshots roundtrip during the player and enemy actions. No live playtesting or
new permanent test suite; balance and feel remain for player feedback.

Current validation also covers all four burst directions, committed aim after a
sidestep, real delayed damage through grates, interruption and recovery, local
puff timing/expiry, ordinary group budget/reuse/crowding limits, and snapshots.
The 64-floor Industry sample placed 64 mixed groups: 56 reused existing gunners,
eight added one. Total gunners changed from 106 to 114 and Pipe Guards from 576
to 560; the spent threat allowance stayed fixed. The 128-floor generation audit
passed route/lock preservation, report neutrality and repeatability. Five static
ordinary-scene views were inspected with the debug grid off. Human balance and
broader combat density remain open. Protocol versions live in `net_protocol.hpp`
and `net_codec.hpp`; network peers must rebuild together.
