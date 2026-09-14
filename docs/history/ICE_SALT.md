# Ice salt remedies and reservoir bait

## Implemented: salted kelp

- Item 24/50 in the regional catalog: five per stack, price 4, one-second
  cooldown. Consume one to clear both nausea duration and its damage timer,
  paying three actual HP. No nausea means no use, cooldown or consumption.
- Salt can kill at three HP or less. It bypasses guard and follows the existing
  damage/recovery rules. The description warns about that cost and the detail
  panel shows the current HP delta, or that there is no nausea to cure.
- Utility pattern targets the user's own cell. It has no healing or weapon
  damage; Restorative and unrelated weapon attributes cannot roll on it.
- Reservoir supply placement uses the existing budget and safe placement rules.
  Ice shops and equipment rewards include it in their regional item pool.
- Dropped kelp is real bait. Eels follow it within six connected water steps,
  eat one, and rest for 1.5 seconds with five seconds of satiety. Committed
  charges still finish. Fish heals them; kelp instead cures nausea and costs HP.
- Seals collect one actual kelp item, retreat to a bank and eat after three
  seconds. They retain their eight-second satiety. Smoked fish stays preferred.
  If interrupted before eating, the held food remains recoverable.
- Every eater consumes the actual item before damage is applied, including
  lethal damage. A seal cannot drop another copy of the consumed meal.
- Original 16px folded kelp and wet scrap sprites, plus a salt-crunch/wet-chew
  cue generated offline. Three scraps enter the existing local cosmetic pool;
  wet scraps use heavy-piece friction and do not affect gameplay.
- Gameplay protocol advances to F6. No snapshot field/layout change is needed.

## Implemented: brine flasks

- Item 25/50: stack three, price 9, 0.75-second cooldown. Travels visibly at
  eight ticks per cell to range five using the existing flask arc. It flies
  over bodies and lands short of solid cover; no instant distant splash.
- Radius-one diamond splash. Big gives radius two; Long reaches nine cells.
  Strong/Heavy affect the four base wound damage and Agile the cooldown.
  Damage requires an actor to be alive and below maximum health before this
  splash; it can hurt the thrower or friends, bypasses guard and is not a DOT.
- Capture affected cells before melting cover, with the existing corner-safe
  line checks. Exposed ice blocks melt, but do not expose new splash targets
  in the same action. Ordinary walls/props are not damaged by the brine.
- Salt water lasts eight seconds in the saved surface liquid state. It melts
  native ice to shallow water and temporary ice to its recorded pool type.
  New freezing fails while brine is active; washing with plain water replaces
  brine and removes that protection. Brine itself does not grant chill immunity.
- Inherits water's washing, fire dousing, conduction, puddle footsteps and
  floating-item behavior. Water cooling lava also applies. No extra unsaved
  timer or rendering-only gameplay state. Ice masons cannot build into wet brine.
- Ice regional shops/rewards include the flask. Salt-green flask and puddles,
  original slosh/splash audio, a small splash ring and local broken glass.
  Splash is audible to investigating enemies at radius seven.

## Implemented: frozen lunch tins

- Compact 16-HP blocking props placed in Ice shelters, preserving protected
  paths and existing clutter spacing. Metal is not fire fuel and does not break
  from footsteps. Regular damage opens it; capsule warmth or a nearby exposed
  flame can thaw it open. Steam leech heat suppression still applies.
- Exactly one ordinary break/loot path: 30% broth, 20% kelp, 20% poultice,
  30% empty. A broken tin cannot reroll when warmed or struck again.
- Original tin/lid sprites, metallic impact and opening sounds. One local
  metal lid plus ice chips scatter on opening; saved broken-prop state allows
  settled cosmetic litter to be reconstructed on rejoin. Opening is heard at
  radius seven. This is regional debris type 15/20.
- Protocol advances to F7 for these rules. Snapshot layout remains 34; the
  existing codec fixture now includes active brine and a damaged lunch tin.

Neither this slice nor the previous kelp slice completes the Ice biome or
master goal. Additional enemies, items, ambience, rooms and other biomes remain.

## Verification

Strict game/render/codec builds, the existing codec checks and a static item
comparison capture, plus a static pool/tin scene. No live playtest or new test suite. The player remains
responsible for feel and balance feedback.
