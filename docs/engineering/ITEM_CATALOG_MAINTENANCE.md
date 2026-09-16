# Item catalog maintenance — 2026-09-15

The catalog previously searched every newer provider, followed by another search
through Forest providers, for each definition lookup. It now indexes one explicit
140-entry table by `ItemKind` and calls only that kind's owner. Definitions remain
in their domain modules; the table contains references, not duplicated statistics.

A compile-time count/order check rejects missing or misordered enum entries.
Legacy items explicitly have null providers and retain their existing inventory,
pattern and use definitions. Invalid enum values still return null. The Forest
base weapon provider no longer searches other modules.

Industry one-shot tool dispatch lives in `items/industrial_use.cpp`; ordinary
melee/guns and continuous actions keep their existing owners. Shared successful
use completion lives in `items/use_completion.cpp`: sound, Hearth sharing,
cooldown, flash, durability, use count and stack consumption. It receives the
original actor/item references, preserving ownership even if an action changes
selection. Early-return actions still own their own completion. `item.cpp` is
414 lines after the split.

No gameplay, item statistics, loot weights, snapshots or protocol changed.

Validation: strict release build passed. A temporary direct comparison retained
the old lookup/use implementations and checked exact definition pointer identity
for all 256 underlying enum values. Across all 140 item kinds, four scene setups,
four target distances and ordinary/last-use inventory states, 4,480 calls produced
identical success results, encoded game state and sound event fields. This is a
refactor equivalence check, not a content balance assessment. No interactive
playtest or permanent test suite was added.
