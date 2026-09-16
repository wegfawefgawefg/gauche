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

Blasting alcoves reserve two threat points for a gunner. Placement tries short
three-bar side partitions with open ends outside protected routes, accounting
for existing actors and props. If no safe post fits, the ordinary reachable
room placement supplies the gunner. Incidental Industrial encounters also mix
in gunners. This is a firing post, not the full planned belt/crank assembly room.
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

Snapshot 48 and gameplay `0x2026091529` require matching rebuilt peers. The
Industrial implementation now has six catalog enemies and six of fifty regional
items. This milestone adds its fifth native catalog debris material; the full
catalog and dedicated assembly-room machinery remain unfinished.
