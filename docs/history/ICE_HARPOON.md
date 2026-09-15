# Ice harpoon gun

Implemented as Ice's 39th item. The remaining Ice equipment and other master
list work are still open.

## Use and physical behavior

- Single loaded harpoon, eight spare. 28 damage, ten-cell range, three ticks per
  cell, 0.6s recovery, 1.1s reload. Price 32 gold; not stackable or consumed.
- Hits occur when the projectile reaches a cell. Cover stops the shaft; fragile
  props receive impact damage. No implicit digging. Shields and parries prevent
  attachment; immovable fixtures can take damage but cannot be reeled.
- A surviving movable body becomes the endpoint. Hold use to pull one cardinal
  cell every ten ticks; releasing use leaves it attached. Adjacent bodies stop
  one cell away. A blocked destination or a gripping victim prevents the pull,
  without invoking shove's crush behavior.
- The owner can move with the tether, within weapon range and clear physical
  sight. The line does not lock the victim's movement or bypass wall corners.
  Stowing, cutting, lost cover, sleep/stun, death or stale handles releases it.
- Secondary cuts an active line even with no spare ammunition. A fresh Secondary
  press reloads afterward. Holding Secondary cannot cut then silently reload.
  Pickup/drop interactions release the line before moving inventory contents.
- Strong, Heavy, Agile and Long alter their normal stats. Piercing and All
  Piercing continue through bodies and attach to the last surviving movable
  victim after the flight ends; they do not produce multiple tethers.

## Presentation and content

- Original 16px gun and shaft sprites; gun remains in the user's hand while the
  tether is out. The line ends at the victim's current authoritative cell even
  when that actor stepped later in entity order. No actor interpolation.
- Six offline-generated cues: launch, impact, latch, reel, cut, reload. The reel
  is a restrained short ratchet, not a repeating warning tone.
- Launch participates in positional hearing and muffling. Echo pebbles can
  record/replay the launch as their 21st supported voice; playback cannot fire it.
- Inspection shows damage, reel speed, magazine/reserve and the actual range
  pattern. Active inventory state reads LINE OUT. Descriptions fit the panel.
- Fishing huts rotate it among fishing tools; Fishing Widows have a 5% chance
  to drop one, alongside 20% fishing line and 20% smoked fish. Ice reward/shop
  pools and the debug item selector include it. Loadout ammo/attributes use the
  ordinary gun editor, with no live tether copied into a spawn template.

## State and validation

`Item.flight` holds the generation-checked projectile reference. The projectile
stores owner and victim handles, remaining range, damage, piercing, travel/reel
beat and a held-use request consumed each tick. No new per-entity fields or
snapshot layout were needed. Gameplay version is `0x2026091507`; old rulesets
cannot join this version. Snapshot decoding validates both projectile fields
and inventory-to-owner links.

Strict game/render/codec builds passed. The existing codec fixture round-tripped
an attached tether. A temporary direct function check exercised travel-before-
damage, release-versus-hold reeling, obstruction without crushing, cutting and
reload edge handling. Two SDL-dummy captures were inspected; both 16px RGBA
assets and all six decoded audio assets passed format/headroom checks.
No live playtest or network session was run for this item.
