# Ice hearing and distraction

## Echo Hound (2026-09-14)

The tenth ice enemy is a blind sound hunter, with 44 HP and nine-tick movement.
It waits for an actual propagated noise, commits to that cell for at most 180
ticks, and does not query the player's current position to steer that trail.
Adjacent to the remembered cell, it gives a 24-tick jaw/ear warning and bites
that fixed tile for 14 damage, even if its occupant has left. It recovers for
45 ticks and listens for another 24. Reaching an empty destination, timing out
or failing to find a route gives a 30-tick listening pause instead.

A newer sound updates pending memory for the next listening phase; it does not
bend the current pursuit or committed bite. Memory expires after 300 ticks.
The shared c-slots hold that pending sound, while point_a holds the committed
destination, point_b the bite cell and counter_a/b the bite origin. Displacing
an active bite cancels it. Actual damage, sleep and stun interrupt into recovery.
A real hit remembers the impact source without acquiring a live target handle.
There is no sight-based fallback chase, invisible diagonal strike or sliding
body interpolation. Another actor or breakable prop can take the fixed bite.

The existing bounded cardinal acoustic flood supplies hearing: walls, solid
props and closed doors seal sound; sound can go around corners. Footfalls reach
four cells, two on snow and six in water. Grounded creatures can attract a
hound; hounds ignore their own pack's footfalls/calls to prevent feedback loops.
Ordinary attack cues reach four, throw/bow impacts seven, gunfire and loud
breakage ten, explosions twelve. Hand Bells and thrown Firecrackers use their
existing propagated distraction rules and also reach hounds.

Action-noise classification happens on deterministic emitted gameplay events,
before the cosmetic audio buffer cap. Sound playback, volume settings, local
ambient audio and whether any client hears the OGG cannot change the response.
The noise path first checks for a nearby living hound before allocating a flood.
Other enemy species retain their existing hearing behavior; ordinary weapon and
footstep propagation is currently an Echo Hound capability.

Echo tunnels spawn a hound for two threat points; later rounds can pair it with
a Frost Bat. One loot roll gives 25% raw meat. The planned additional muffling
felt drop is still pending that item's implementation. Four generated 16px body
poses distinguish high listening ears, tracking legs, warning jaws and recovery;
four generated OGGs provide listening whine, double growl, bite and death.

Gameplay compatibility advances to EE; snapshot layout remains 32 because the
enemy uses existing shared state. Strict game/render/codec builds pass without
warnings, existing snapshot codec check passes, and a static pose capture was
inspected. Sprite formats and decoded audio headroom pass. No live playtest or
new suite was run; behavior and balance remain for the user's playtesting.
The master goal is still incomplete.

## Muffling Felt (2026-09-14)

The nineteenth regional item supplies two applications, four with Durable,
costs fourteen and takes 45 ticks to apply. Each application wraps the next
unwrapped melee weapon or gun after the felt's inventory slot, wrapping around
once. Its detail card names the actual target slot and weapon before use. Ground
and reward previews ask the player to equip it before predicting a target.
Existing wrapping is never refreshed or overwritten; no valid target means no
use or cooldown spent. Consumable throws, noisemakers, food and fixtures cannot
be wrapped. Fists, bows, repairable blades and firearms can.

A wrapped weapon suppresses ordinary use-noise propagation to Echo Hounds and
plays those cues at one-quarter local volume. Impact sounds, broken scenery,
explosions, footsteps and real damage reactions stay unchanged. Rocket launch
can be quiet while its later explosion remains loud. Melee windup, bow draw and
release, gun report and launched crossbow/rocket cues use an item-aware emission
helper; all impact paths retain ordinary emission. The sixth use is still quiet.
A gun volley spends one charge regardless of pellets; failed allocation/empty
clicks spend none. Completed melee actions spend one, cancelled windups none.
Bow release spends one after successfully creating an arrow, never per draw tick.

The remaining six-use allowance lives on the weapon as `muffled_uses`, independent
of ammo, durability, use count or rare attribute. Drops, trades and saved/rejoined
state preserve it. Emptying a wrap produces a quiet unwinding cue and local felt
scraps; exhausting the two-application item has its own empty-sleeve cue rather
than a box smash. One icon and three generated cloth sounds are included.
Inventory/HUD/compact cards show a small Q-count badge, leaving ammo, condition
and cooldown numbers in their own places. Full details spell out QUIET USES.
The HUD also now allows seven characters in its condition readout so an existing
durable rake displays 800/800 instead of truncating the last digit.

Regional shops/rewards and alternating echo-tunnel supplies include the felt.
Echo Hound's single loot roll is now 25% raw meat, 10% Muffling Felt, otherwise
nothing. No artifact or inventory-UI state is repurposed to store its effect.
Snapshot layout advances to 33; gameplay compatibility to EF. Both inventory
and ground/payload items include the new byte in the codec and hash, with bounds
and item-eligibility validation. Cosmetic sound events carry a muffled playback
flag; that flag does not depend on local audio settings.

Strict builds pass without warnings. The existing codec round-trip includes a
partly used wrapped weapon and passes. Static inventory/held comparisons and
compact HUD captures were inspected; image format and decoded audio headroom
pass. No live playtest or new suite was run. The master goal remains open.


## Recoverable alarm clock

The twentieth Ice item is a small brass clock. Place it in a free adjacent dry
cell; it waits three seconds, then makes five acoustic pulses at one-second
intervals. Each pulse travels up to ten cells through the existing hearing
flood. Walls seal sound; listeners investigate and sleeping actors wake.
The clock neither damages nor stuns. Its countdown ticks are only audio cues.

After five seconds of ringing, it becomes a loose item nearby. Its 8 HP (16
with Durable) carries back into item condition; recovery cannot repair it.
Resin Glue can repair the carried clock. Attacks can destroy the placed clock,
ending its distraction and scattering local brass casing and gear debris.
A full entity pool leaves the stopped clock intact until recovery has room.

Placed state uses compact Prop fields: variant records Durable, hp stores
condition, growth_ticks counts down from 480. The snapshot layout is unchanged;
the gameplay protocol is F0. Existing codec coverage includes a damaged Durable
clock mid-ring. New winding, ticking, double-bell ringing, stopping and breaking
sounds are generated offline. Three 16px poses distinguish its states.
Echo-tunnel equipment, Ice rewards and shops can supply it.

Validation: strict build, existing codec checks, offline audio headroom and
static `alarms` / `alarm-items` captures. Gameplay and balance remain for the
user's playtesting.
