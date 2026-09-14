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

## Remaining salt work

Brine flasks and frozen lunch tins remain unimplemented catalog entries.
This slice does not mark the Ice biome or master goal complete.

## Verification

Strict game/render/codec builds, the existing codec checks and a static item
comparison capture. No live playtest or new test suite. The player remains
responsible for feel and balance feedback.
