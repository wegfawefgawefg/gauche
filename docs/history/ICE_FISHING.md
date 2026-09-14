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
balance remain for the user's playtesting. The follow-up below adds Widow and bait.


## Fishing Widow

The twelfth implemented Ice enemy occupies fishing huts, with Bell Divers joining
later rounds. She has 64 HP and seeks a cardinal lane within six tiles. A 0.6s
rod-raising tell fixes the cast direction and origin. The hook travels one tile
per four ticks; its first eligible actor takes 14 damage and gets reeled one tile
per ten ticks. Her own allies can intercept it. Shields, parries and damage
avoidance prevent attachment. There is no extra contact damage during retrieval.

Sidestepping the lane, intervening cover or a body crossing the tether cuts it.
Damage, sleep, stun, displacement or death interrupts the caster. She stands
still while casting/reeling and spends 1.5s untangling afterward. A finite hook
lifetime prevents endless pulling; a blocked pull releases instead of crushing
someone against a wall. Owner and victim references carry entity generations.
One drop roll gives 20% Fishing Line, 20% Smoked Fish, otherwise nothing.

Four body poses, a hook sprite and seven original offline sound cues cover warn,
cast, latch, reel, snap, untangle and death. The tether ends at the actual caught
actor, with no actor interpolation. Snapped line scatters local rope fibers.

## Smoked fish and bait

The twenty-second implemented Ice item heals 14 immediately (21 Restorative),
stacks to six and costs six gold. Fishing huts supply two; cold shops/rewards and
Widow drops can supply more. It has dedicated eating/nibbling sounds and local
fish-bone debris, the ninth regional loose material.

Hungry eels search up to six connected wet steps for dropped fish. The bounded
route respects cover and impassable bodies. Eating consumes one fish, heals the
eel by four, rests it for 1.5s and suppresses new feeding for five seconds of
awake activity. Bait competes with starting a shock, never cancels an existing
charge. Wolves, bears, boars and dogs also accept fish through their existing
nine-tile rich-food scent routing. Seal interactions and fish crates remain open.

This follow-up uses existing saved fields; snapshot layout stays 33 and gameplay
protocol advances to F3. Strict build and existing codec checks pass. Static
`widows`, `fish-bait` and `fish-items` captures and PNG/audio checks validate asset
wiring and panel fit; no live gameplay or balance claims are made.
