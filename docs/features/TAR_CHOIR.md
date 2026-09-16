# Tar choirs and settling tanks

Implemented a nineteenth Industry enemy behavior, with a room that supplies
ways to change the encounter instead of adding another specialized feed item.
Player balance feedback is still needed; no interactive playtesting was run.

## Encounter

Three stationary, single-tile 32-HP singers share an immutable generation-tagged
identity. They sway and inflate together, sounding low bubbling thirds, then
cough separately at 0.8, 1.0 and 1.2 seconds. Their landing cells form a short
strip around the target's position when the warning begins. A lone survivor
still attacks, aiming directly at the committed cell.

Each glob travels one cardinal cell every six ticks along an integer supercover
path: no homing and no diagonal corner cuts. It deals six damage on contact and
leaves one ten-second tar patch. Bodies intercept the shot; grates pass it;
solid cover stops it at the preceding cell. A successful parry disperses it.
Tar uses the existing flask rules: sticky footing, finite flammable fuel,
cold-hardened passable crust and water washing. Fire can hurt the singers too.

Breaking sightlines between committed singers cancels their pending coughs and
puts them on different recovery clocks. Damage, cold, sleep, stun, rooting and
displacement also interrupt. Once separated, each surviving subgroup can act
independently; a dead, sleeping or recycled original leader cannot lock the
others. Normal synchronized volleys have a 3.2-second start-to-start cycle.

The final living member's death alone rolls 30% for a Tar Flask. The following
15% solvent-rag slot remains empty until that item has a useful implementation.
Already-launched shots survive the shooter's death.

## Room integration

Settling tanks are one of eleven optional Industry maintenance-room roles.
The mirrored footprint places a trio beside two small tar basins, shoot-through
rim grates, two scrap-bin covers, two Cold Flasks, two Emergency Foam cans and
a shallow-water source. It spends three threat and two equipment budget units.
The central three-cell-wide crossing remains dry and unobstructed.

The entire footprint is checked before placement. This room has a dry ruin
floor rather than the generic southeast lava quadrant. The first generation
check caught a rejected footprint: its middle singer and supplies crossed the
protected route. Moving those to side alcoves and explicitly carving the dry
work floor fixed actual generation, without relaxing route protection.

## Presentation and state

Two original transparent assets provide the singer and flying glob. A local
swell/sway and compressed cough pose communicate the windup without enabling
debug attack squares. Six offline-generated OGG cues provide three chest
voices, cough, disrupted rhythm and death; normal tar splash visuals are shared.
`tools/sound/tar_choir.py` is the reproducible sound source.

Entity and projectile state use existing shared slots; no per-species fields or
new replicated schema arrays. Snapshot validation covers voice indices, phase,
mask, clocks and the bounded projectile path. Gameplay version is
`0x2026091564`; co-op peers need the same build.

## Verification

Strict release build passed. Temporary focused checks covered delayed physical
volleys, matching hashes after a mid-volley snapshot, disrupted cadence, six
interrupt mechanisms, generation-safe leader recycling, final-only loot, lone
survivor targeting, corner cover, grates, cold/water material interactions,
entity-pool preflight and malformed snapshot rejection.

Across 128 generated Industry floors, four complete trios appeared; every floor
retained reachability and its required lock. All six sounds loaded with SDL
dummy audio. Static software-render captures checked the room and close-range
singer silhouettes. These checks establish integration, not encounter balance.
