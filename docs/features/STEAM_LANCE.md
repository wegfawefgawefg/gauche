# Steam Lance — 2026-09-15

Industry now has twenty-three of fifty regional items. The Steam Lance is a
reusable weapon with a preparation loop: fill from clean water, stand near actual
heat for 1.5 seconds, then spend that fill on a two-cell 18-damage scalding thrust.
The 0.45-second brace commits facing; movement remains available. A real health
hit, control interruption, cancel/drop/pickup or switching items cancels the
windup without spending water. After a damage interruption, release Use before
trying again. A blocked health hit does not interrupt it.

The thin jet passes grates and successive creatures but stops at solid cover.
Its footprint is fixed before damage/quenching changes the scene. Cold water
inflicts no damage and still douses fires, burning actors and exposed fuses,
with the shared water/lava effects. Both hot and cold releases leave five-second
water patches. It does not create fire or grant protection against friendly
scalding. A completely obstructed thrust retains its fill.

Strong changes damage, Long extends to three cells, Agile/Heavy change recovery
and bracing. Big, Piercing, Durable and Fragile are not applicable. No ammunition
refill and no passive damage boost. Price twenty-four; native stage-one weapon,
reward, shop, cache, secret and workshop tables use the master supply row.

## A place to use it

Lamp alcoves now include a checked shallow-water cell on the dry approach.
Half supply a cold-filled lance; the other half retain two Emergency Foam cans.
The existing thirty-second finite-fuel stove heats the lance, the water refills
it, and the existing pair of Furnace Moths offers a reason to use both scalding
and quenching. Their cold lamp still supplies harmless lure light. No extra
threat/equipment budget and no obstruction of the protected central route.

## Shared rules and presentation

`items/heated_water.*` owns the kettle/lance fill-source predicate and thermal
clock. `loaded` remains empty/cold/hot; `spare` stores heating progress or remaining
heat. Hot contents cool after thirty seconds away from a heat source; wetness
cools them immediately. Dropping/stowing/swapping does not reset heat. The kettle
retains its previous shape, damage, timings and sounds. Windup's copied lance is
not a second independently heating item, avoiding duplicate readiness sounds.

`items/steam_lance.*` owns the lance definition and release; existing committed
item actions own bracing. Shared HUD meters show EMPTY, COLD, heating progress or
HOT with remaining seconds. Details show contents, actual damage, windup and
range. Debug loadouts select contents/heat. Snapshot validation constrains water
states and thermal clocks. No new wire fields; gameplay version `0x2026091562`.

Two original sprites are saved as `assets/graphics/steam_lance.png` and
`assets/graphics/lance_hot.png`. Built-in imagegen sources retained under
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:

- `exec-ace43573-1bc6-4123-be4e-755ddf41d147.png`: isolated diagonal steel lance,
  brass pressure reservoir, wooden grips, small valve lever, sparse top-down
  pixel silhouette and transparent background.
- `exec-35fcec03-e2e2-4705-a253-89b3b419d9e3.png`: hot-state edit of that inspected
  sprite, orange reservoir, ivory gauge and two small steam clusters; same pose.

Original alpha preserved. Eight offline sounds from `tools/sound/steam_lance.py`
cover filling, ready/cooling valves, bracing, hot/cold discharge and impacts;
peaks 0.15–0.34. Scald/splash use existing local steam and ripple particles.
The loud discharge enters shared hearing; no synthesized music work.

## Evidence and remaining feedback

Strict release build passed. Temporary direct checks covered kettle/lance clean
water and heating/cooling parity, complete 27-tick aimed windup, two-target damage,
side-lane exclusion, interruption without charge loss, cold quenching, grate
passage and solid cover, meaningful modifiers, reload refusal, mid-brace snapshot
continuation/rejection, and debug normalization. Sixty-four generated Industry
floors retained reachability and required locks; eleven lances appeared.
Eight sounds loaded under SDL dummy audio, and the static hot-item/brace/details
render was inspected. No autonomous playthrough or permanent test suite.
Live usefulness, readability and balance remain for user playtesting.
