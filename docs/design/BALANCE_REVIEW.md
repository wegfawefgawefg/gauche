# Balance while building content

**2026-09-17 decision:** the user likes Forest as it currently plays. Preserve
the [measured Forest baseline](../measurements/forest-2026-09-17/README.md) as a
reference; earlier discussion of supply scarcity is not an approved rebalance.

2026-09-15. Source: [verbatim user feedback](../user_messages/2026-09-15-balance-and-archive.txt).
Execution status lives in [MASTER_TASKS.md](../MASTER_TASKS.md). This specification
extends the [world interaction review](WORLD_INTERACTION_REVIEW.md).

## Latest priority: worthwhile things to do in Ice and Industry

The user's latest playtesting finds Ice and Industry/lava **barebones and boring
compared with Forest**, which feels good. Catalog counts are not a substitute for
interesting spaces and encounters. Resume game content after the bounded music
pass; do not keep generating music indefinitely while these areas remain thin.

There are too many items whose main identity is feeding something or clearing a
very short, narrow status effect. The user still welcomes useless/joke finds
(Pressure Valve was their example), but wants enough strong, broadly useful toys
alongside them. Do not solve this by deleting all oddities or multiplying every
item's damage. Audit actual frequency, slot cost, encounter value and the ratio
of general-purpose gear to special-case items in the player's real supply.

Fishing Line is specifically **fun to operate but lacks useful targets/situations**.
Give existing toys worthwhile applications in the world: safely retrieving loot,
traversing meaningful optional barriers, manipulating an encounter, accessing a
reward, or combining with another item. A new tool and a new room should improve
one another. Avoid another catalog of clever implementation with little reason
for players to use it. Review repeated feed/remedy designs before adding more.

First response: [optional salvage and native combat supplies](../features/SALVAGE_POCKETS.md).
This is an initial connection between existing systems, not completion of the
Ice/Industry content-density or utility review.

First implementation pass: [native supply/ammo/fist changes](../features/SUPPLY_BALANCE.md).
The starting-point findings below describe the pre-change behavior. Availability,
SMG reserves/refills, general consumable bundles and baseline Fist are now changed;
Hearth now uses [finite cooked-meal sharing](../features/HEARTH_BALANCE.md).
Offer-value matching, acquisition timing, augments and broader balance remain pending.

## Preserve what is working

The user's brother enjoyed roughly an hour in Forest. Preserve abundant toys,
experimentation and clever interactions. The requested correction is progression
and dominant choices, not making all finds scarce or removing cross-biome surprises.
Balance is ongoing content work, not a cleanup step after the content count is met.

Target the intended four-biome, sixteen-floor progression. This is the design
horizon, not a claim that all sixteen floors/fourth-biome content already exist.
Early floors should leave substantial room to grow; high power can emerge later
through equipment, combinations and repeated choices rather than one early pickup.

## Verified starting points, not tuned conclusions

- In `src/run.cpp`, the Forest item-reward path selects the native list only half
  the time. The fallback is eleven broad items including SMG and RocketLauncher.
  Those two therefore are ordinary fallback outcomes, not specially rare imports.
  This is one confirmed source; still audit rooms, containers, enemies and shops.
- In `src/item.cpp`, an Ammo use supplies every carried gun. Most receive at least
  twelve rounds or three magazines. SMG magazine size is thirty, so it receives
  ninety spare rounds per use. Rocket Launcher receives two. Review the total
  loadout benefit, not just the number shown for the selected weapon.
- Fist currently has ten base damage, twelve ticks of cooldown and three ticks of
  windup, without weapon durability. Compare effective attack cycles, reach and
  safety against actual melee tools; nominal damage alone misses the advantage.
- Hearth's catalog promises one HP each second to the owner and friends within
  four tiles. Audit actual triggers, stacking and co-op behavior before choosing
  new numbers. Its ongoing run-wide healing is not equivalent to a small consumable.

## Native loot and ammunition

Forest's ordinary weapon identity should be bows, blowguns and other fitting
woodland tools. SMGs, machine guns and rocket launchers must be exceptionally rare
off-biome finds, not outcomes of a common fallback list. Preserve occasional
exciting imports. Classify each item by origin, availability, power and sources;
do not confuse “available everywhere” with “equally likely everywhere”.

Audit room drops, themed containers, enemy drops, guaranteed supplies, shops,
rewards and alternate generation layouts. Keep debug loadouts explicitly exempt.
Replace accidental shared-list leakage with explicit native/shared/import rules.
Measure effective chance per entire biome/run, accounting for the large number
of rolls; a small chance per box can still make imports routine. Strong imports
need both rare acquisition and finite ongoing support. Review their starting ammo.

Reduce SMG resupply substantially from ninety rounds. Choose a per-weapon refill
budget based on damage, fire cadence and expected useful encounters, not just
magazine size. Make the ammo description communicate actual per-gun quantities.
Review whether supplying every carried gun is intended and how duplicates benefit;
do not silently change that behavior while merely lowering SMG numbers.
Let a rare SMG provide an exciting advantage without guaranteeing exclusive use
for an entire biome. Native weapons should remain worthwhile without requiring
the player to deliberately refuse superior equipment.

## Fist as fallback, punching as a build

Substantially nerf baseline Fist so real melee finds offer compelling improvements.
Preserve crisp input and readable short windup; do not make punching feel broken
or rely only on a frustrating delay. Review damage, cadence, reach, interruption,
resource-free sustained use and safe kiting against actual enemy attack cycles.
An unarmed player still needs an understandable survival option, but it should
not dominate acquired melee weapons or make durability a punishment for upgrading.

Separate stronger punching into earned gear/augments. User candidates: heavy
glove, lightning/light/heavy/fire/poison fists. Give them explicit identities:
heavy glove offers stronger slow impact; lightning fist interacts with conduction;
fire fist ignites suitable material; poison fist trades immediate damage for a
condition; light fist could illuminate or briefly flash (meaning remains open).
Specify whether each is an equipped item, permanent augment or timed effect,
and how it changes the persistent fallback without duplicating inventory fists.
Define stacking, triggers, friendly fire, counters, provenance and availability.

## Augments, Hearth and three-choice rewards

Audit persistent artifacts, item attributes, temporary buffs and ordinary items
separately before tuning: “augment” in the feedback may refer to several systems.
Hearth is an immediate priority. Unlimited recovery can erase food/remedy choices,
attrition and danger; co-op may multiply that benefit. Consider bounded healing,
conditional triggers, a resource cost or later availability. These are options,
not already selected rules. Merely making an overpowering early reward rarer does
not fix its strength when it does appear.

Define a progression envelope over sixteen floors: modest early specialization,
stronger mid-run synergies, powerful late combinations. Review absolute strength,
earliest appearance, price, frequency, duplicate stacking and interactions together.
More creative augments should change decisions, not only multiply every stat:
last-round effects, earned punch modifications, terrain-conditioned mobility,
heat/cold tradeoffs and narrowly triggered recovery are candidate directions.
Attach costs, caps and counters that fit the fantasy; avoid arbitrary restrictions.

Generate three-choice offers with comparable expected value for that stage and
current build, while retaining different kinds of benefit. A permanent healing
aura, movement bonus and one glowing plant seed are not automatically peers.
Weak toys may remain common ground finds; reward offers can supply an appropriate
bundle, useful variant, themed package or a different item instead. Do not inflate
every seed into a permanent power just to match an overtuned artifact.
Compare immediate survival, sustained benefit, utility and synergy without making
every offer numerically identical. Coordinate with the new replacement/confirmation
flow so players understand both the choice and what it costs them.

## Working checks and iteration

Follow-up: some throwables are underpowered and arrive in inadequate quantities.
Audit actual items before choosing changes; increase damage where their attack
doesn't justify use, and provide useful stacks where a single disposable item
doesn't justify a slot. Tune damage, radius, utility/status effects, travel/windup,
recoverability, source stack count, stack maximum and price together. Not every
utility throwable needs a damage buff, and stronger reusable throws need not get
the same quantity as weak expendable ones. Distinguish a stack of consumables
from uses remaining on one item. Carry intended quantities consistently through
ground drops, containers, rewards and shops, respecting normal stack merging.

Use small temporary summaries/calculations for effective loot frequency, starting
and replenished damage budgets, real attack cycles, healing over a floor, and
co-op/stacking extremes. Include imports and native replacements at early, middle
and late floors. No large new test suite or autonomous live playtest is needed.
Record initial values and changes with reasons, then adjust from human feedback.
Revisit this review whenever a new dominant tool or permanent power is added.
