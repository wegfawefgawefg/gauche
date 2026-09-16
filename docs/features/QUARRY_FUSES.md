# Quarry Charges and Fuse Scissors

Quarry Charges and Fuse Scissors bring the Industrial regional item count to
three. Both enter the implemented Industrial reward/shop pool. Workfront supply
rolls can leave two charges or one pair of scissors, each at a one-in-three
chance independently of the whistle. Powder Monkeys and their dedicated rooms
remain pending; no unimplemented drop is registered.

## Quarry Charge

Place at your feet while facing adjacent ordinary stone. Three seconds later it
cuts up to three forward cells through eligible stone walls using shared terrain
damage with dig power 1. Nonstone walls, unbreakable boundaries, stronger dig
requirements, hard fixtures and impassable ground stop the bore. Blocking props
receive damage but stop the remaining lane. The charge cannot cut reinforced
Industrial exit enclosures. Open gaps do not extend the three-cell reach.

The cut lane deals 24 damage, including to allies. The charge cell and one cell
behind it take the same dangerous backblast; rear cover is resolved before the
forward bore changes any tiles. Damage to props/actors uses shared pipelines.
The inventory diagram marks the rear cell and the player in red. Stacks hold
three charges, price is 19, placement cooldown is half a second. This fixed
assembly has no item modifiers; placement failures spend nothing.

The charge stays physical after its owner leaves or dies. A three-second fuse
gets brighter and faster-sounding near completion. Water quenches it into the
same recoverable item. Cold slows an exposed fuse to half speed for two seconds;
existing heat can clear that chill. Scissors can recover it before detonation.

## Fuse Scissors and shared exposed fuses

Scissors target the shortest live, landed fuse at your feet or directly ahead,
with stable entity order breaking ties. They recover the exact modifier-bearing
item if inventory insertion succeeds. The bomb remains live if the pack is full;
no snip is spent. Airborne bombs, expired fuses, mines without an exposed fuse,
and sealed Prism Bomb/Thaw Charge reactions refuse. Twelve successful snips,
0.4-second cooldown, price 8; Durable doubles uses and Fragile halves them.

Ordinary Bombs, Firecrackers, Pitch Bombs and Quarry Charges share exposed-fuse
classification. Water directly hitting their cell quenches them; a landed fuse
in a puddle also goes out before the next burn tick. Cold quenching slows the
clock without changing throw travel. Airborne bombs cannot be cut with scissors.
Quenching converts the existing projectile slot to a GroundItem, retaining its
cell, generation and exact item, so entity-capacity exhaustion cannot delete the
reward or duplicate the bomb. Scissor recovery atomically inserts and removes.

The cold/wet rules now apply to existing Forest bombs as well as the new tool;
sealed Ice explosives retain their behavior. Cold fuse sparks render blue.

## Assets, storage and validation

Four native 16px sprites cover the charge, lit/urgent states and scissors. Six
new offline OGG cues cover placement, fuse, rock burst, snip, dousing and worn-out
scissors. Shared terrain impacts supply stone chips and broken-wall feedback;
these effects remain client-only. Charge explosions emit semantic noise for
nearby hearing creatures independently of audio playback.

Shared projectile fields store the charge's fixed origin, facing, actual item,
checked owner and fuse. Existing freeze_ticks stores temporary fuse chill.
Snapshot 43/gameplay 0x2026091524 cover the new items, projectile kind and shared
fuse behavior; charge decoding checks item, fuse, placement and cardinal axis.

Focused checks cover three-cell bore, 24-damage rear/own-cell blast, reinforcement,
invalid placement, exact recovery, full-pack refusal, half-speed cold clock,
water conversion, airborne refusal, modifier preservation and sealed-fuse
exclusion. Four generated Industrial floors retain required locks, reachable
objectives and snapshot/hash round trips. Release game/static-renderer builds
and inventory/world render inspection are the validation for this content;
no autonomous playthrough or permanent test suite was added.
