# Whiteout weather interaction

This slice adds the fourteenth Ice enemy, twenty-third regional item and two
regional debris types (thirteen implemented). It does not finish the Ice catalog
or count action sounds toward the twenty ambient cues.

## Drummer

The Whiteout Drummer has 44 HP and twenty-tick walking steps. It approaches a
visible target, including decoys, until within six cells. It fixes its stance and
squall center, then plays three distinct drum beats at 0, 24 and 48 ticks. At 72
ticks it raises a four-second, radius-two squall and rests for three seconds.
It cannot damage actors directly. Nearby Echo Hounds can hear its beats.

The committed center does not follow a moving target. Sparse moving flecks mark
that center during the buildup. The tell still draws when its caster is just
outside the viewport. Actual damage, sleep, stun or displacement interrupts the
beats and leaves 1.5s recovery. Newly obstructed geometry can cancel the burst.
Hearing uses the existing c-slots; phase, beat count, stance and target occupy
the shared label/counter/timer/point fields.

## Snow Globe

Break a globe in the adjacent aimed cell for six seconds of radius-two whiteout.
It costs twenty gold, stacks to two and consumes one on success, with a one-second
cooldown. Big enlarges the area to radius three; unsupported modifiers cannot
roll. Pattern, comparison panel and use all derive from the effective item data.
It appears in weather-station equipment, cold rewards and cold shops.

A wall or blocking prop prevents breaking it at that cell. The burst spreads
through a bounded cardinal air flood; walls, blocking props and closed doors
constrain it. An entirely fresh six-second field is not consumed into an already
equivalent field. Ordinary expired/partially expired fields can be refreshed.
It causes no damage, chill, freezing, slippery ground or projectile collision.
The glass sound and resulting local shards originate at the break cell.

## Visibility and state

Whiteout is a separate saved surface countdown, not repurposed smoke/fire/fuel.
The existing target-hiding and clear-attack-sight paths both read it, so neither
side receives special targeting immunity. Already committed attacks and manually
aimed projectiles can still pass through it; obscuration does not delete shots.
Existing smoke thresholds retain their behavior. Flakes and softly joined haze
are presentation only, respect lighting and fade at expiry without moving actors.

Surface timers enter the deterministic hash and snapshot codec. Snapshot layout
34 carries the new field; gameplay protocol F5 rejects older simulations. The
existing codec fixture now includes a nonzero whiteout duration for round-trip
coverage. No new gameplay test suite was added.

## Weather station and assets

The eighth cold encounter role uses a courtyard with snow at the edges and a
small ruin floor in the center. It combines a drummer with a Frost Bat on early
rounds or an Echo Hound later. Both encounter and item placement use the existing
floor budgets. Required-path protection still governs decoration placement.

Weather vanes are sparse nonblocking 18-HP metal props, not fuel or loot boxes.
They break into copper curls and brass. Snow Globe glass and copper curls use
the normal local debris collision/friction pool and do not affect gameplay hashes.
Broken vanes reconstruct settled litter on reconnect.

Four drummer poses, globe and vane sprites, and two debris sprites use original
16x16 artwork. Seven offline sound cues cover the three beats, squall, globe
break, drummer death and vane break. These are action cues; weather-station
ambient beds remain separate unfinished content.

Validation: strict game/render/codec build, existing snapshot round trip, static
drummer/whiteout/item-panel captures, PNG checks and decoded audio headroom.
No live playtesting; usefulness, density and encounter balance remain for the
user's feedback.
