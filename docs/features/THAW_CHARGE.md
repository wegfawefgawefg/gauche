# Thaw Charge

Implemented 2026-09-15. Ice regional item 43/50.

Place one charge at your feet while facing a breakable ice wall. The sealed
chemical fuse lasts two seconds and continues through water, stowing, movement,
sleep or the placer's death. The core brightens and ticks faster during the last
half-second. It is a planted projectile with a physical timer, not an instant
inventory explosion. Another live charge on the same cell rejects placement.

The burst opens at most two ice walls in the facing direction, leaving shallow
water. Material, HP, map bounds and Unbreakable rules are checked again at burst
time. Other walls and solid fixtures stop the cut. An ice wall already opened by
another tool does not move or extend the original two-cell reach. The charge
stays at its placed cell; changed materials and protection rules still apply.

Steam deals ten damage to the center and adjacent cardinal cells, including the
placer and friends. Cover is evaluated before melting, keeping the far side of a
wall sheltered from that burst. Steam leaves three seconds of water, sharing
existing quench rules. Big widens the steam cross to radius two; Strong/Heavy/Agile
use normal damage/cooldown changes. No Long modifier; wall reach is always two.

Stack three, price 19. Quarry equipment rolls can supply two; maintenance lockers
have a 3% chance. Included in Ice rewards/shops and the generic debug item selector.
Three small sprite states and four offline-generated cues cover placement, fuse,
fracture and steam. Local fragments/vapor stay outside gameplay hashes. Inventory
shows the steam pattern, fuse and wall limit.

The projectile reuses its timer, placement cell, facing, saved item and owner
handle. Domain validation bounds the fuse and verifies fixed placement. Snapshot
format remains 37; gameplay compatibility is `0x2026091512`.

Validation: strict game/render builds; focused direct calls for failed/duplicate
placement, stack consumption, exactly 120 fuse ticks, two-wall melting, changed
materials, protected ice/map boundary, friendly steam damage and cover, Big/Long
rules, water/owner-death persistence and snapshot round trip/invalid fuse rejection.
Inspected dummy-driver armed, breach and inventory renders. No autonomous playthrough.
