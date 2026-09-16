# Native supply pools, ammunition and the fallback fist

Implemented 2026-09-15 from the brother's Forest balance feedback. This is an
initial balance pass; Hearth, reward-value matching, creative augments, throwable
damage and broader enemy/encounter tuning remain open.

## Availability

`items/supply_catalog.cpp` contains one explicit row for every item kind: origin,
native weight, earliest local stage, bundle count and eligible general sources.
Compile-time checks require enum-ordered, complete coverage. Metadata lookup is
direct; random selection scans the small catalog during generation/reward creation.
This is separate from the still-existing item-behavior definition provider chain.

General rewards, shops, caches, secrets and workshops reference the catalog.
Deleted the duplicated Forest/Ice reward/shop lists and Industrial-only list.
Specific sources keep their purpose: fish huts, machinery lockers, animal remains,
guaranteed room tools and similar supplies still have explicit rules. Industrial
generic room loot no longer falls through to Forest cache/orchard/workshop rules.

Weights start at common 8, uncommon 3 and rare 1. Strong/complex finds mostly
arrive later in their native biome; secret rooms can draw one stage ahead and
favor rarer native entries. Forest's ordinary weapons include bows, blades, tools
and later crossbows/blunderbusses. Modern guns belong to Industry; no common
fallback places SMGs or rockets in Forest. The haunted house's fixed Shotgun
was replaced with a native Blunderbuss. Debug kit selection remains unrestricted.

Ordinary eligible general sources have a 1/2000 chance to draw from foreign
origins; secrets have 1/500. Workshops never import. Shared supplies are ordinary
eligible finds and do not consume an import roll's foreign selection. These
chances are for any imported item, not each imported gun individually. No hidden
per-tile import rolls occur. Actual aggregate run frequency depends on the number
of generated sources/players, and should be revisited with human playtesting.

A fixed-seed temporary sample of 200,000 Forest-stage-two reward-item selections
produced 96 imported items, of which three were SMG/rocket/shotgun. The same-size
secret sample produced 355 imports, of which 31 were those weapons. These samples
are implementation checks, not claims of measured live-run balance.

Bundles give selected consumables multiple pieces rather than automatically one.
Counts clamp to actual item stack capacity. Shop preview, capacity check, item
comparison and committed purchase all use the same bundle; price is for the
displayed offer. Source-specific supplies may deliberately use different counts.
The larger cancelable replacement-picker overhaul is still pending.

## Ammunition and fists

| Weapon | Rounds added by one Ammo |
| --- | ---: |
| SMG | 18 (previously 90) |
| Rocket Launcher | 1 (previously 2) |
| Shotgun | 4 |
| Pistol | 18 |
| Musket | 5 |
| Bow | 12 |
| Crossbow | 8 |
| Blunderbuss | 3 |
| Lens Carbine | 6 |
| Harpoon Gun | 4 |
| Rivet Gun | 12 |

Ammo still supplies each carried weapon, including duplicates; it does not
refill only the selected gun. New gun kinds have a one-magazine fallback until
explicitly tuned. No useful recipient means no consumption. Reserve additions
are bounded against integer overflow. Gun details show refill quantity and Ammo
details list the actual carried recipients/amounts instead of a meaningless pattern.

New SMGs start with 30 loaded +18 spare (was +120); rockets with 1+1 (was 1+4);
shotguns with 6+6 (was 6+30). Existing serialized inventories keep their actual
contents. New items use the new defaults, including debug defaults before editing.

Baseline Fist deals 4 damage, down from 10. Three-tick windup and twelve-tick
cooldown are unchanged. This reduces free sustained damage without introducing
sluggish input. Earned fist gear/augments remain later content work.

## Verification and integration

Release game and static renderer build successfully. A temporary direct-call
check covered catalog identity/capacity, six source filters across all twelve
floors, deterministic selection, rare imports, actual ammo use/no-op consumption,
duplicate gun refills, reserve overflow, real fist windup/damage, shop full-space
failure and single purchase, 24 generated floor reachability/locks, reward bundles,
haunted supplies and snapshot/hash round trips. No permanent test suite added.
A static software-rendered Ammo/SMG/Fist panel capture was inspected; quantities,
reserve, damage and timing fit without overlap. No autonomous live playtesting.

Gameplay version is `0x2026091531`; snapshot layout stays at 49. Multiplayer peers
must rebuild together. No new gameplay fields were added by this slice.
