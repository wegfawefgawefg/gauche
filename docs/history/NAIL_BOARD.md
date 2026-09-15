# Nail Board — weapon and recoverable floor trap

Implemented 2026-09-15 for the Industry useful-content pass. This adds a simple
weapon with a second practical job, following feedback about too many narrow
feed/remedy items and too few worthwhile tools.

Primary commits a six-tick swing for 10 damage, followed by 18 ticks of cooldown.
It has 35 repairable condition; each swing spends one. This is intentionally a
quicker ordinary attack than the initial 39-tick windup/cooldown draft, which
would have made its melee output worse than Fist. It does not dig or shove.

Secondary lays the actual board one cell ahead, taking it out of the inventory.
It arms for 18 ticks; the next grounded walker takes 16 damage. A trap hit costs
four condition and turns the trap into the same worn ground item; when those
four points exhaust it, the board breaks. No condition is restored by deploying,
retrieving or triggering it. The placer and friends are susceptible. Flying,
airborne-tossed and sled-riding actors pass safely. A held directional shield
does not block a foot trap. A generation-safe placer handle supplies attribution.

Normal pickup can retrieve an armed or arming board from the adjacent tile.
It uses the existing inventory capacity/swap flow. Recovery reuses the trap
entity even when the pool is full. Occupied, wet or burning placement cells,
lava, and failed entity allocation leave the held board intact. Direct attacks
and heat also reduce deployed condition; heat costs five per half second.
Recovery takes the remaining trap health, preserving attributes and wear.
Strong/Heavy/Fragile scale trap damage from their melee damage multiplier;
Big/Long affect the swing, not the one-cell trap footprint. Durable doubles
condition. The standard glue repair and melee muffling paths remain available.

Native stage-one supply entry: common weight 7, price 11, one board, eligible for
weapon/reward/shop/cache tables. Assembly's former loose coal cargo becomes one
Nail Board; the other belt carries three Bolt Pouch handfuls. Existing coal
sources elsewhere remain. This supplies two usable combat items in that room.

Item and trap behavior live separately in `items/nail_board.*` and
`traps/nail_board.*`. The small `traps/contact.*` dispatcher routes shared
melee/drill/splinter hits to woodland traps and Nail Boards without adding
Industry behavior inside the woodland implementation. The existing Trap entity
stores the full item and condition; decorative clutter remains compact props.
No new snapshot fields. Gameplay version `0x2026091554`; trap state is validated
when decoding snapshots.

## Assets and checks

Original transparent icon: `assets/graphics/nail_board.png`. Generated source
retained at
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/exec-0feccb0b-3887-4cc2-b347-2fccfb87402a.png`.
Prompt requested one diagonal rough plank with three bent iron nails, chunky
muted pixel clusters, transparent background and no scenery/text/shadow.
The same silhouette represents the laid board. `tools/sound/nail_board.py`
generates four original swish/thud/metal/splitting cues, peaks 0.17–0.26.
Breaking scatters existing local WoodChip and BentNail materials.

Strict release build passed. Temporary direct checks covered committed melee
and wear, arming without early damage, one-shot trap conversion, adjacent
recovery, deployed damage/heat, flying and sled exceptions, owner damage,
mid-arming snapshot continuation, invalid timer rejection, exhausted-condition
breakage, invalid placement and full-pool refusal. 64 generated Industry floors
retained reachability and required locks and contained 76 boards. SDL dummy
loaded all four OGGs and produced `/tmp/gauche-nail-board.png`; inspected the
static armed/held rendering. No autonomous playthrough or new permanent suite.
Human play feedback still determines final weapon feel and trap balance.
