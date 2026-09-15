# Emergency doorstop and reservoir sluices

Ice now has 40 of its planned 50 items. The rest of the master list remains open.

## Doorstop

The 21-gold wedge is placed into an adjacent **open moving gate**, including
encounter gates. Closed gates and ordinary key doors reject it. Placement moves
the actual item into an eight-byte tile prop with 25 HP; Durable gives 50 HP.
It is nonblocking, survives footsteps, and holds the gate until removed or broken.
It does not generate fuel, although a fire already burning beneath it can damage
it like other metal props. Physical attacks and explosions can break it into
local brass debris.

Pickup from the player's feet or the facing cell recovers its remaining HP and
Durable attribute. Entity allocation and inventory capacity are checked before
removing the prop. A full inventory can use the existing legal swap behavior.
Loose items at the feet take priority over fixtures at the feet, then fixtures
ahead. Pickup and its HUD preview now use the same ordering for wedges, candles,
grounding spikes and pressure valves, including a valve at the player's feet.

The item panel shows condition, placement/recovery and its one-cell utility
pattern. It is included in Ice rewards, shops, the loadout editor and a 5%
maintenance-locker drop. Reservoir sluice chambers leave one outside the gate.

## Gates and chambers

Gates separate their controller's requested position from their actual position.
A wedge or living impassable actor defers closure. Removing a wedge or breaking
it allows closure once the doorway clears; closure never crushes the occupant.
Encounter controllers still own their wave/party rules and request open/closed
positions through this shared gate function.

Timed sluices alternate 180 open ticks and 120 closed ticks. A brief amber blink
and mechanical warning precede closure by 30 ticks. The cycle keeps running while
jammed, with a short strain cue on a rejected closing attempt. Gate grates and
open frames use top-down sprites; the small wedge sits at the threshold's edge.
Eight original offline-generated sounds cover gate movement/warning and wedge
fit, lift, impact, break and strain. Applicable actions enter positional hearing.

Reservoirs now reserve at least an 8-by-7 half-extent so a five-by-five chamber can
fit off the central walkways. Placement checks every cell against protected
routes, room bounds, props, actors and traversability. It does not overwrite a
required corridor. The dry chamber contains a maintenance locker and six coins,
with one timed opening facing a clear approach. Failed placement skips the
chamber without partially writing geometry or consuming an entity slot.

## State and validation

Shared gate slots hold requested closure, controller/timed mode, open/closed
durations and the remaining cycle timer. Wedge props hold remaining HP and a
Durable bit. Both are deterministic and included in existing hashes/snapshots;
no snapshot layout fields were added. Gameplay version is `0x2026091508`.
Snapshot decoding validates gate modes, timings, collision flags and wedge state.

Strict game/render/codec builds passed. The existing codec fixture round-tripped
a damaged Durable wedge holding a requested-closed gate open. A temporary direct
check exercised closed-gate/key-lock rejection, preservation of damage on pickup,
occupied-doorway deferral, closure after clearing, timed reopening and breakage.
Four constructed Ice floors retained the required key route; two contained
sluices after correcting an initial footprint collision with reserved walkways.
Final SDL-dummy world and item captures were inspected, and the three 16px RGBA
sprites/eight decoded sound files passed format and headroom checks.
No live playtest or network session was run.
