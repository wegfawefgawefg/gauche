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
nine-tile rich-food scent routing. The next slice adds seals and fishing creels.

This follow-up uses existing saved fields; snapshot layout stays 33 and gameplay
protocol advances to F3. Strict build and existing codec checks pass. Static
`widows`, `fish-bait` and `fish-items` captures and PNG/audio checks validate asset
wiring and panel fit; no live gameplay or balance claims are made.


## Seal thieves and fishing creels

The thirteenth implemented Ice adversary is a 56-HP Seal Thief. Reservoir and
fishing-hut banks can host one within the floor threat budget. It has fourteen-tick
ordinary steps, seven while carrying, with ordinary water/ice footing rules.
Its eight-step bounded cardinal search prefers smoked fish over meat and eggs,
respects walls, props and bodies, and excludes lava and deep water. It steals one
loose item rather than taking from a player's inventory. Generation-checked food
references are re-evaluated as loose food floats, moves or gets picked up.

The item keeps its complete instance state in the seal's held inventory and is
visible beside its mouth. With food, the seal chooses a reachable water edge,
favoring distance from a visible nearby threat. It settles for three seconds
before consuming the food and healing by that item's effective healing value.
Threats within two cells interrupt eating. It then stays fed for eight seconds
of awake activity. If no bank is reachable, it keeps the item instead of deleting
it or eating through cover. Death returns carried food intact plus an independent
25% chance of raw meat.

An adjacent visible player or decoy provokes a half-second warning bark, then a
16-damage bite at the committed cell, followed by 0.8s recovery. Damage, sleep,
stun or displacement cancels the stance. It does not chase a distant player to
bite them. No actor position interpolation was added.

Fishing huts now scatter woven creels in their existing protected-path-aware
prop pass. A creel has 12 HP, blocks walking, burns and breaks under attacks.
Breaking rolls once: 35% smoked fish, 20% fishing line, 15% air bladder, 30% empty.
The broken prop cannot reroll contents. Wicker strips and fishing floats are the
tenth and eleventh regional debris types; creels also leave rope fibers. Wicker
gets light-material drift, floats heavier friction, all in the existing local
cosmetic pool. Broken creels reconstruct settled litter on join.

Five seal poses, basket art, two debris sprites and eight original offline audio
cues are committed. Barks and basket impacts participate in the existing hound
hearing rules independently of client audio playback. Death sounds now use a
small explicit species dispatch instead of an expanding damage-path expression.

Protocol F4 rejects older gameplay; entity/item/prop snapshot layout stays 33.
Validation: strict game/render/codec build, existing snapshot codec check, static
`seals` render, 16x16 RGBA asset checks and decoded finite/headroom audio checks.
No live playtesting; encounter density, tells and feeding balance remain the
user's to judge. Separate landing layouts and the rest of the catalog stay open.
