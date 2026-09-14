# Shard colonies and crystal galleries

## Colony rules

- A colony starts as three distinct 28-HP crystal actors. They do not walk, but
  normal shove/pull and status tools can affect them. All keep the same saved
  generation-checked colony identity, even after its original node is destroyed.
- A visible player or decoy within seven tiles of any member wakes the colony's
  attack cycle. All endpoints spend 48 ticks charging before a single pulse.
  A 12-tick flash follows, then 108 recovery ticks before another warning.
- Each pair connects only within eight cardinal-distance tiles, with clear
  physical sight and both nodes still at their committed cells. Walls, solid
  props and closed fixtures interrupt a connection. Smoke does not block a
  charged physical link, but can prevent acquiring a player for the next cycle.
- Damage disables one endpoint for 90 ticks. Sleep/stun/root hold that disruption
  until released, followed by 90 ticks. Moving a charging crystal immediately
  breaks its current links. Later cycles can connect its new position if valid.
- The surviving pair remains dangerous. A lone crystal cannot attack; it remains
  breakable and can complete the colony's loot roll. Status on one member does
  not freeze or cancel unrelated members. The lowest awake living member drives
  their common clock; a dead or sleeping coordinator can be replaced safely.
- Pulse snapshots all currently valid links and damages each victim at most once
  for eighteen, even at overlapping edges. Shields and reflection retain ordinary
  rules. Other enemies can be hit; members of that colony cannot hit each other.
- Link cells follow an explicit integer grid line. The rendered warning marks
  those same cells, so diagonal damage does not use a separate visual path.
  After a pulse, the bright flash is cosmetic; it does not repeatedly damage.

## Gallery and rewards

- Crystal Gallery joins the Ice room pool: broad slate clearing with ice edging.
  Three endpoints fit a triangle in off-route alcoves, respecting protected
  crossings, spawn clearance and nearby objective fixtures. Allocation reserves
  enough actor slots first and abandons an incomplete formation safely.
- A colony costs three threat. Later galleries may also have a Frost Bat, which
  can complicate dodging with chill. Ice-brick supplies use the ordinary equipment
  budget and offer a way to interrupt sight between crystals.
- A single 25% Crystal Lens roll occurs on the colony's last death. No per-node
  lens rolls. Old slot generations cannot accidentally join an unrelated colony.
- Three original 16px sprites: resting, charged and disrupted/spent. Cold light
  and bright internal cracks show readiness; disabled bodies remain visible.
  Three original offline sounds: bowed-glass charge, pulse and splintering break.
- Broken nodes shed local crystal splinters. Existing debris resources suffice;
  this slice adds no renamed debris entries to the quota.

## Storage and validation

Shared slots: entity_a immutable colony identity; point_a committed position;
label_a phase; timer_a shared countdown; timer_b disrupted endpoint cooldown.
No per-colony controller entity, per-species struct or new snapshot field.
Gameplay protocol advances to FC; snapshot layout remains 34.

Strict game/render/codec builds, existing codec roundtrip with surviving nodes
referencing a removed origin, and static `shards`, `shard-cut`, `shard-pulse`
captures. Original asset dimensions/audio and file-size constraints checked.
No live playtesting or new test suite; user feedback owns final combat balance.

Ice now has eighteen of twenty enemy behaviors, twenty-seven of fifty items,
seventeen debris types and thirteen cold room roles. Icicle Spider, Boiler
Porter, remaining items and ambience, other biomes and master tasks remain open.
