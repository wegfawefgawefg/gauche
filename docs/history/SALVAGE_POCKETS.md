# Give existing toys something worthwhile to do — 2026-09-15

The user finds Fishing Line fun but without a useful purpose, and Ice/Industry
too thin compared with Forest. This first response connects existing tools to
optional rewards and fixes a basic floor-supply problem.

## Salvage scenes

Ice Fishing Huts/Reservoirs can contain a dry loot plinth surrounded by deep water,
with a Fishing Line on the casting bank and a creel alongside. The near crossing
is **exactly three water cells**: the existing Folded Bridge fits. The existing
hook travels four cells, catches the actual ground item and reels it to shore;
moving interrupts the line without duplicating its cargo. Existing teleport
tools offer another approach. Deep water does not currently freeze via Cold
Flask, so that is not advertised as a solution to this pocket.

Industrial Workfront/Blasting/Workshop/Cache rooms can contain the hot-ground
variant. It has a metal scrap bin, a dry loot spot and surrounding lava. A carried
fishing line retrieves its loose item; crossing safely requires an appropriate
tool or accepting the existing heat risk. No out-of-zone Fishing Line is handed
out in Industry. The water-only portable bridge does not bridge lava.

Both scenes put 5–9 coins alongside the weapon as an extra payoff for actually
reaching the plinth. Coins are placed before the item so the shared ground-item
placement search does not move them onto the bank. Neither is a required
objective. Every edit checks the protected route, existing bodies/props, room
bounds and a safe casting bank; inadequate space simply skips the feature.
At most one scene is added per generated floor, with its own small optional
reward budget. The footprint can rotate/mirror to fit an unused alcove.
Fishing Huts now reserve a broad clearing shape suitable for this activity.

`LootSource::Salvage` derives membership from the central **Weapon** rows, allows
one local stage of preview, favors rarer entries and never imports other biomes.
There is no second weapon-kind list. A recovered weapon is an actual equipped
item with its normal stack/uses/ammo, not a cosmetic prize or feed pickup.

## Ordinary floor supplies

Previously the three equipment slots could all be consumed by situational role
supplies. Every biome also got a Forest Stick at its entrance and the equipment
fallback was an explicit Bow. Ice and Industry now reserve one of those existing
three equipment slots for a native Weapon-source item near the entrance. The
remaining fallback uses the same native master-table source. Forest keeps its
opening Stick. Strong cross-zone guns do not enter the Forest through this path.

This establishes a floor of useful combat supplies, not a complete rebalance of
feed items, remedies, container drops or three-choice rewards. Further content
should add reasons to use the toys and improve enemy/room combinations.

## Checks

The strict Release game build passed. A temporary direct-function check verified
the Ice and hot layouts, identical seeded placement hashes, untouched protected
cells, weapon-source/native rewards, actual item preservation through fishing,
movement breaking the line, and the existing bridge spanning all three cells.
Static SDL-dummy captures of both scenes were inspected; no gameplay was run.

For each floor 5–12, 24 generated seeds retained reachable objectives and a
required locked exit, plus the native entrance weapon (192 layouts total).
Salvage appeared in 60/96 Ice and 14/96 Industrial layouts in that small sample;
these are placement observations, not promised spawn probabilities. Industrial
crew/terrain placement often leaves less eligible space.

The change adds no saved fields or simulation actors. Gameplay contract advances
to `0x2026091535` because generation/RNG and loot behavior changed; wire version
14 and snapshot layout 49 remain unchanged. Full co-op play and tuning remain
the user's feedback work.
