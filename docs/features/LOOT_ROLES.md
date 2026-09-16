# Item roles and useful supply

`src/items/supply_catalog.cpp` owns each item's primary role alongside origin,
stage, weight, bundle and source membership. Combat, survival and mobility mean
portable, broadly useful gear. Utility needs a matching situation, target or
other equipment. Oddities provide small effects, play or distraction. These are
availability labels, not restrictions on creative use or promises of equal power.

Print the current registry with `build-release/gauche --audit-supply`; counts go
to stderr, per-item CSV to stdout. `--audit-generation` also reports actual
initial ground pickups by role in ordinary Ice/Industry floors.

## Native catalog targets

Counts on 2026-09-16, including scene-only items such as the Rail Switch Key.
Shared items are listed separately; Fist, editor Wall and Empty are excluded.
Each kind counts once, including items with multiple applications.

| Origin | Combat | Survival | Mobility | Contextual utility | Oddity | Total |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Shared | 3 | 3 | 1 | 2 | 1 | 10 |
| Forest | 28 | 5 | 5 | 13 | 4 | 55 |
| Ice | 14 | 6 | 7 | 23 | 2 | 52 |
| Industry | 17 | 5 | 4 | 9 | 1 | 36 |
| Fourth biome | 0 | 0 | 0 | 0 | 0 | 0 |

Within each biome's minimum fifty native items, target at least **18 combat,
6 survival, 6 mobility and 10 contextual tools**. The remaining ten slots are
flexible and include oddities. Thus at least thirty items offer broadly useful
combat, survival or movement choices. Existing toys stay in the catalog.

Current gaps: Forest needs one survival and one mobility option; Ice needs four
combat options; Industry needs one combat, one survival, two mobility and one
contextual option as part of fourteen further items. The fourth biome needs its
full catalog. Prioritize readable weapons and practical consumable stacks over
additional feeds, fuels and brief status cures. Passing fifty total is not enough
to close these role targets.

## Selection and placement

- Reward item offers draw combat/survival/mobility only. Artifact and permanent
  upgrade valuation remains a separate open balance task.
- General caches and the shop variety slot select those roles 70%, contextual
  tools 25%, oddities 5%. Secrets use 85/12/3; workshops use 40/55/5. Empty groups
  are omitted and remaining shares normalized. Native weight and stage apply
  within the chosen group, so adding many contextual items cannot dilute gear.
- Shops retain a medicine slot and a distinct weapon slot. Bundles, comparisons
  and committed purchases share the same supply counts.
- Existing import rolls remain 1/2000 per eligible draw and 1/500 for secrets.
  Reserved equipment, workshops and salvage rewards stay native/shared.
- Ordinary floors reserve one further combat/mobility pickup before themed
  supplies consume their equipment budget. It differs from the entrance tool
  and uses reachable, unoccupied dry ground before the locked gate. Budget is
  spent only on successful placement; it does not add a new supply allowance.
- Crates have a 15% equipment branch; snow caches have a 20% native equipment
  branch. Dedicated fitting lockers and other themed containers retain their
  contextual purpose and explicit contents.

Fishing Line already has real salvage plinths/current-carried loot to retrieve;
its placement near water is appropriate. Pressure Valve and Nozzle Elbow alter
actual warned jets; Sealant repairs pipes/belts/drives/tanks, while Belt Crank
runs connected unpowered belts. These remain contextual rather than competing
with a weapon as a level reward. Repeated copies across neighbouring machine
scenes still need review. In the sample, Industry contained 142 Cranks and 132
Brake Shoes across 64 floors; useful scene tools can still be overdistributed.

Throwable supply bundles retain three rocks/needles/bolt handfuls and two bombs
or similar heavy throws. SMG ammunition remains eighteen spare rounds per Ammo;
rockets receive one. This pass does not increase ammunition or damage.

## Verification and limits

Generation-only samples used seeds 1–64 per biome, cycling all four stages;
no gameplay ticks or autonomous runs. Initial broadly useful ground pickups
rose from 332/633 to 367/639 in Forest, 482/892 to 512/892 in Ice, and 855/1809 to
865/1808 in Industry. Minimum combat/mobility pickups per sampled floor rose
from 1 to 2 in Forest/Ice and remained 5 in Industry. Counts include scene cargo
and optional finds; they do not measure what a player actually acquires.

8,192 seeded item-reward draws per biome all satisfied the role filter; general
shop/cache draws were 70.8% broadly useful and secret draws 85.5%. Focused checks
covered stage/source filters, exclusions, determinism, real rewards/shops,
bundles, safe equipment placement, no-space behavior and snapshot round trips.
The existing 128-floor generation audit passed routes, required locks,
repeatability, report neutrality and snapshots. Human balance, repeated scene
supplies and the full enemy/container import audit remain open.

Ice weapon follow-up: Ice Axe and Tusk Pike add two native combat choices to
ordinary supply and fitting quarry/fishing scenes. Their 120/100 condition is
repairable; the axe cleaves and digs ice, while the pike shoves only at its tip.
A generation-only 64-floor sample placed 30 axes and 21 pikes. Four-direction
combat, blocking, crush/fall, wear, repair, snapshot and static render checks
passed, along with the 128-floor generation audit. Human balance remains open.
