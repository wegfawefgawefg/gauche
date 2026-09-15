# Rail Switch Key and working junctions

Industry has twenty-five of its fifty planned regional items. The key is a
contextual tool: a T siding supplies one and shunters have a 20% drop slot.
It does not dilute the general shop, reward or cache pools with a tool whose
fixture may be absent. This follows the feedback about narrow items arriving
without useful situations.

## Actual rules

An adjacent key turns live points clockwise to the next connected track end.
Twelve successful turns exhaust it; Durable provides twenty-four. A cart on
the points, a broken fixture, a missing rail underneath, or fewer than two
connected exits refuses without spending. It cannot create rails.

Points use the existing compact Prop variant as a saved cardinal direction.
At its next eight-tick wheel beat, a rolling cart on points takes that exit.
The choice is fixed: cutting or blocking it stops the cart even if another
exit is clear. A reverse selection can send a cart back toward its shunter.
Destroying the 24-HP iron fixture leaves carts following their incoming facing.
A stopped cart still needs a push; changing points does not start it remotely.
Chain hauling remains an explicit directional pull, not automatic routing.

## Room and presentation

The existing thirteen-cell siding gains a short perpendicular branch where
its complete footprint and walking margins fit. Both orientations work. The
straight fallback retains Brake Shoe supply; junction supply replaces that
with a key. Cargo/threat/equipment budgets are unchanged. Reserved walking
paths remain open, and no new solid actor is placed on them.

Finite freight tracks now draw ties and paired rails from connected neighbors.
Old zero-HP train track keeps its original drawing. A rotating lever and small
brass arrow show the selected exit. Two original transparent assets were made
with the built-in image generator and copied intact, preserving alpha:

- `assets/graphics/rail_switch_key.png`: heavy iron socket T key, brass handle,
  diagonal inventory silhouette, minimal muted pixel art, transparent ground.
- `assets/graphics/rail_points.png`: top-down low iron mounting plate with
  east-pointing brass-knob lever, minimal muted pixel art, transparent ground.

The full prompts requested one asset each, no lettering, scenery or border,
with a logical 16-pixel aesthetic. Original generated images remain in the
session image directory. `tools/sound/rail_points.py` produces three offline
OGGs: double-latch lever clack, fixture break, exhausted socket. No runtime synth.

## Validation

Strict release build; temporary direct behavior checks for actual inventory
wear, occupied refusal, exact mid-junction snapshot/cargo replay, cut/blocked
exit stops, broken-point fallback, Durable and invalid variant rejection.
128 generated Industry floors retained exit reachability, required locks and
exact snapshot hashes; eight contained complete T sidings. Both mirrored
footprints were inspected by direct construction. All three sounds loaded via
SDL dummy audio. A static software render was inspected; no interactive
playtest or permanent test suite was added. Gameplay compatibility: `0x2026091566`.
