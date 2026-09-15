# Ash Sleeper and quiet ash loft

Implemented 2026-09-15 for Industry's encounter/ecology pass. The sleeper is a
44-HP ash mound that responds to work noise, not player proximity alone. Its
half-second shaking rise gives warning before it can pursue. The swirling form
commits a cardinal adjacent cell for a half-second, hits for eight, and recovers
for half a second. Leaving the marked cell avoids the hit. Damage cancels a
committed swipe; displacement/root spoils its origin, and stun/sleep/toss settle
it. Continuous machine noise refreshes agitation but cannot repeatedly cancel
its attacks. It remains normally damageable in all forms.

Six seconds without fresh agitation starts a half-second settling animation.
Quenching or cold settles it and suppresses waking for two seconds, refreshed
while standing wet or chilled. Neither transition heals it. It uses ordinary
body collision and floor contacts; fan-specific interactions remain future
content rather than an invisible new dependency.

Loud world events with hearing radius at least seven use the existing bounded
sound propagation around walls/props/doors. Quiet footsteps, bumps and muffled
weapon starts do not automatically wake it. Direct scripted loud noises also
work. Real damage wakes the injured sleeper without requiring a second noise.
The check runs on deterministic emitted actions before the local sound-buffer
cap; mixer volume and ambience cannot affect AI. Its own inhale/swipe/settling
sounds do not wake nearby sleepers or keep itself awake indefinitely.

Shared slots store phase, two clocks, agitation, committed origin/direction and
the normal audible destination. No new entity fields. New snapshot state checks
and gameplay compatibility version `0x2026091555`. Dedicated files are
`entities/ash_sleeper.*` and `world/ash_loft.*`.

## Room and acquisition context

The optional ash loft occupies a checked side footprint: two sleepers, a
three-cell manually driven belt, actual Nail Board cargo, a Belt Crank, coolant,
and scrap/ore bins. The central protected cross stays clear. The normal crank
item action produces noise and wakes nearby sleepers; the internal belt-motion
helper alone does not emit that action sound. Full footprint and five available
entity slots are checked before placement, with mirrored fallback.

The loft joins the optional maintenance-room selection, costs two threat and
two equipment budget, and cannot replace objective/exit/secret rooms. Incidental
Industry encounters can also choose a sleeper for two threat. The planned ash
sack (25%) and filter mask (15%) drops are still unimplemented, so the current
single death-roll slot remains empty. Nearby room containers provide ordinary
native loot. Bagged-ash and fan-outlet variants are not claimed complete.

## Presentation and validation

Two original transparent PNGs: `ash_mound.png`, `ash_whirl.png`. The mound shakes
before rising and the active silhouette shrinks down while settling. Five dry
noise/grit cues come from `tools/sound/ash_sleeper.py` (peaks 0.15–0.25). Death
scatters existing cosmetic coal crumbs; the separate Ash Tuft debris entry is
still pending.

Generated originals retained under
`/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`:
- `exec-e9f59322-4c54-43d3-a60d-68377ad3735b.png`: initial mound; rejected halo.
- `exec-8938d45d-530a-4a11-98d2-ae8965105865.png`: cleaned hard-edged mound.
- `exec-760f4257-7b98-4e84-bb7a-5e8cf0e3e8e3.png`: raised ribbon/whirl form.

Prompts requested muted charcoal/cool-gray opaque pixel clusters, no face or
humanoid head, transparent alpha, low ash clumps / asymmetric corkscrew ribbons;
edits explicitly removed glow, smoke and background haze. Both final sprites
were inspected, as was `/tmp/gauche-ash-loft.png` from an SDL dummy static render.
The render also loaded all five OGGs. No autonomous playthrough.

Strict release build and temporary direct checks passed: quiet/loud/muffled and
wall-blocked hearing; waking despite a full cosmetic sound buffer; complete rise
and attack warnings; dodging, damage/displacement interruption; water suppression
without healing; natural settling; mid-rise snapshot continuation and invalid
strike-direction rejection; protected footprint refusal; real crank-action wake.
64 generated Industry floors retained reachable routes and required locks, with
22 sleepers and six lofts. Live balance/readability still needs human feedback.
