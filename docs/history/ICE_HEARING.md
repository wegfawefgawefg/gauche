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
