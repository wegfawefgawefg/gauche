# Forest population and supply reference — 2026-09-17

## Decision

The user likes Forest as it currently plays and considers these odds good. Preserve this balance as a reference; the investigation is not authorization to increase weapon supply, reduce crowds, or retune feature probabilities. Working title proposal: [Teeming](../../design/NAME_AND_TRAILER.md).

Gameplay revision: `ec3b1ec` (player-count-independent networking). **256 ordinary maps per floor, seeds 1–256, 1,024 maps total.** Each floor starts from a fresh Game with that seed. This is not 256 sequential simulated runs. Floor totals add measured per-floor means; no whole-run appearance probability is inferred.

No simulation ticks, player traversal, subsequent drops, shop/reward choices or inherited inventory. Full-map placement does not establish that a player will notice or collect a supply. Haunted House and other whole-floor unique layouts are outside this ordinary-layout sample.

## Population

Combat = mobile fighters + territorial specialists + boss; worm heads count once. Conditional defenders = bears, mimics, ants and gnomes. Background life and enemy-generating structures are separate. Categories come from the preserved sampling source, rather than a universal hostility flag.

| Floor | Combat mean | Range | Conditional mean | Background life mean | Sources/controllers mean |
|---|---:|---:|---:|---:|---:|
| 1-1 | 35.98 | 12–63 | 4.62 | 6.45 | 0.00 |
| 1-2 | 57.50 | 24–110 | 8.43 | 7.65 | 0.49 |
| 1-3 | 74.49 | 47–124 | 10.90 | 8.94 | 1.02 |
| 1-4 | 79.99 | 46–127 | 11.47 | 11.76 | 1.41 |

## Actual feature and species presence

Percentage of sampled floors with at least one. These include theme effects and placement failures; they are not independent initial-roll odds.

| Feature/species | 1-1 | 1-2 | 1-3 | 1-4 |
|---|---:|---:|---:|---:|
| Cross-room river | 62.1% | 53.9% | 55.9% | 41.8% |
| Open sectors | 40.6% | 50.0% | 100.0% | 51.2% |
| Giant tree | 19.9% | 24.6% | 22.3% | 22.3% |
| Timber grove | 12.9% | 28.1% | 33.2% | 27.7% |
| Connected bear den | 27.0% | 31.6% | 35.5% | 30.9% |
| Spider cave | 16.0% | 41.8% | 45.3% | 39.8% |
| Snake tunnel/pit | 18.0% | 31.2% | 34.0% | 35.9% |
| Root maze | 14.8% | 25.0% | 23.8% | 26.6% |
| Ant colony | 20.3% | 35.9% | 32.4% | 31.6% |
| Mushroom district | 25.8% | 33.6% | 40.6% | 40.2% |
| Spore Toad | 0.0% | 23.4% | 34.8% | 45.3% |
| Bear | 73.8% | 81.6% | 83.6% | 86.3% |
| Wolf | 0.0% | 98.0% | 99.6% | 100.0% |
| Bat | 100.0% | 100.0% | 100.0% | 100.0% |
| Zombie | 99.6% | 96.9% | 99.6% | 100.0% |
| Forest Spider | 27.7% | 61.3% | 61.7% | 59.8% |
| Ant | 20.3% | 35.9% | 32.4% | 31.6% |
| Gnome | 17.6% | 21.1% | 28.9% | 30.9% |
| Old Growth boss | 0.0% | 0.0% | 0.0% | 99.2% |

Old Growth is the optional Forest 1-4 bear boss and clearing. Its roll always passes, but placement failed for seeds **216 and 225**: 254/256 actual bosses. The exit does not require killing it. This is recorded as behavior, not silently fixed by this audit.

## How rolls compose

- One weighted major identity and one compatible minor modifier (including None). Major themes compete for the major slot. Timber/Undergrowth and Mushroom Woods/Mushroom Flush have explicit incompatibilities.
- Landmarks then receive separate, theme-adjusted rolls. Spider cave and snake habitat can coexist: the sample contained both on 8/256, 31/256, 34/256 and 37/256 floors respectively.
- Reservation order is giant tree → timber grove → bear den → spider cave → snake tunnel → root maze. Earlier reservations can leave no suitable connected footprint for later successful rolls.
- Further terrain passes also check circumstances and progression constraints. Rivers need a source/outlet/route; a successful initial roll is not a guaranteed built river.
- River base attempt odds are 1/2, 1/2, 1/2, 1/3; Wet Woods boosts the attempt to 1/1. The observed rates above include that boost and placement outcomes.
- Ordinary encounters roll pack family, size, layout, members, specialist and wildlife slots. Content stage gates, scene-specific supplies and secret-table stage advancement are distinct rules.

Code references: [feature rules](../../../src/world/generation_report.hpp), [theme rules](../../../src/world/generation_themes.hpp), [theme effects](../../../src/world/generation_themes.cpp), [reservation order](../../../src/world/route_graph.cpp), [encounters](../../../src/world/forest_encounter_rules.cpp).

## Item references

- [Counts and appearance rates](ITEM_COUNTS.md): weapons, tactical supplies, healing/food, all other observed ground items, separate hidden/stored loot, and stack totals. Counts are individual item units; ammunition is refill items, not bullets; herb bags count bags, not activations.
- [Combat definitions and pickup-instance rates](COMBAT_ITEMS.md): effects, base damage, durability, charges and ammunition, plus item sources. NPC inventory is not assumed obtainable.
- [Raw item instances](item-instances.csv), [summary](item-summary.csv), [base definitions](item-catalog.tsv), [generation observations](generation.csv).

Dedicated ground weapons (hatchet, spear, maul, knife, bow, crossbow, blunderbuss, boomerang) average 1.61 / 2.09 / 2.19 / 2.26 per floor. None appeared on 22.3% / 10.9% / 11.7% / 12.5% of floors. Including pickaxes, rakes and digging claws lowers the no-weapon fraction substantially. This is a descriptive grouping, not a claim that the other items cannot fight.

## Using the baseline

Compare future revisions against these same seeds and definitions. Counts, species mix, feature co-occurrence and supplies are useful reference points for how populated Forest feels. They are not compulsory counts for Ice or Industry. Pair them with walkable/reachable area, occupied-room coverage, quiet stretches, spawning during play, weapon endurance, travel time, and actual playtests; equal head counts can feel very different. See the earlier [density/coverage comparison](../../engineering/FOREST_GENERATION_COMPARISON.md).

## Reproduction

The standalone probes are preserved in [sources/](sources/). Compile each with the game’s Release C++ flags and `-Isrc`, link against the existing `gauche` objects/libraries while replacing `src/main.cpp.o`, and run from the repository root. Generation probe writes CSV to stdout. Item probe writes CSV to stdout plus `item-catalog.tsv` in its working directory. They do not launch SDL windows or run gameplay. Use revision `ec3b1ec` to reproduce this historical baseline; a newer revision measures newer behavior.

The built-in `--audit-forest` remains a smaller 16-seed/floor audit with walking-density metrics; it is not the 256-seed probe archived here.
