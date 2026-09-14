# Fishing line

The twenty-first Ice item is a utility spool, distinct from the forest Rope Hook.
It catches loose items rather than pulling creatures or the caster toward walls.
Fishing huts alternate it with Air Bladders; Ice rewards and shops also supply it.

## Cast and retrieve

- Six-tile cardinal cast, four ticks per tile in each direction. Walls, blocking
  props, closed doors and impassable actors stop the outbound hook. It reels
  back empty after a miss or obstruction rather than disappearing at max range.
- The first loose item is caught. It remains the same GroundItem entity, with
  the same generation and complete item instance: stack, condition, charges,
  ammo, attributes and special state. No copy is made into a second loot item.
- Reel toward the cast origin. Pick it up normally when it reaches your feet.
  Other loose items stop retrieval early rather than overlapping two piles.
  It can travel over deep or shallow water; regular item wet/fire rules still
  apply. Fishing does not make a walkable bridge or move the player.
- The caster must remain at the original cell. Death, displacement, a blocked
  tether or lifetime expiry releases the cargo in place. Switching items is
  allowed. One active fishing hook per caster, even across multiple spools.
- Another player can pick up cargo while it is being reeled. Generation-checked
  references then release the hook safely. Two fishing hooks cannot claim the
  same loose entity. A float stops when caught; a new float or external movement
  releases the fishing line instead of fighting over the item's position.
- 20 casts and 0.75s cooldown. Durable gives 40 casts; Long gives ten-tile reach.
  Other attributes are ineligible. The final cast still finishes after the empty
  spool is consumed. Failed creation or a second concurrent cast costs nothing.

## Presentation and state

A small pale hook travels with a thin tether. Once it catches cargo, the actual
item remains visible at its actual position; the hook does not cover its icon.
Seven offline sounds cover cast, obstruction, snag, reel, retrieval, snap and an
empty spool. Snaps and empty spools leave local rope-fiber debris with light-piece
friction and wind response. These cosmetics do not steer the simulation.

The projectile's existing fields hold outbound/reeling phase, owner and cargo
handles, original/expected cells, finite travel range and clocks. There is no new
item or entity layout. Gameplay protocol F2 prevents mixing old simulations;
snapshot layout remains 33.

Validation: strict build, existing snapshot codec checks, static `fishing-cast`,
`fishing-reel` and `fishing-items` captures, and audio/PNG checks. Live gameplay and
balance remain for the user's playtesting. Fishing Widow, fish bait and other
catalog items remain separate unfinished work.
