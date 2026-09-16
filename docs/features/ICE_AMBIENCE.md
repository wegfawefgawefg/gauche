# Ice ambience and small environmental scenes

## Implemented soundscape

Twenty new original OGG cues and their offline source script are committed for
the drowned observatory. They supplement action sounds; they never replace an
attack warning or enter the deterministic hearing system.

| Cue | Placement and playback |
| --- | --- |
| Ice groan | Occasional reservoir-ice resonance; stops when that tile thaws. |
| Water under ice | Low loop at water and exposed shallow openings. |
| Dripping thaw | Loop at a wall-fed pool fitting, with local ripple rings. |
| Thin wind | Quiet global bed, quarter gain while the listener occupies ruin flooring. |
| Window whistle | Positional loop at a broken window ornament. |
| Hanging chain | Enter cue at a wall-side mooring beside shallow water; 35s cooldown. |
| Distant bell | Rare global resonant bell, 70s nominal cooldown. |
| Timber creak | Enter cue at the window's timber frame; 60s cooldown. |
| Snow settling | Occasional soft powder noise at existing snow; no gameplay avalanche. |
| Boiler idle | Follows a live tank's actual position, audible only while it has fuel. |
| Pressure hiss | Steady harmless pipe leak; visually distinct from a timed boiler vent. |
| Pipe knock | Occasional mechanical response from the same pipe fitting. |
| Waterwheel | Mechanical/water loop at a wall-fed pool, with a slowly rotating wheel. |
| Slush lap | Gentle surface loop at open water/shallow banks; stops on freezing. |
| Glass tinkle | Enter cue attached to an intact lens case; 45s cooldown. |
| Observatory motor | Loop attached to the actual beam-lamp prop and a small rear gear. |
| Cloth flutter | Loop at a torn flag on a snowy wall edge; slight visual sway. |
| Far animal call | Sparse global wildlife call, quarter gain indoors; 50s cooldown. |
| Chimney draft | Attached to a stove; fuel and lit state control audibility and smoke. |
| Submerged knock | Rare positional knock from a sealed decorative hatch in deep water. |

Each source has explicit gain and near/far radii. The existing six-loop/three-event
voice budgets, quadratic falloff, stereo pan, short gain fades, local 0.1s scheduler
and shared 2.5s event spacing remain in use. Enter cues do not all play on joining
inside their trigger areas. Default ambience levels leave foreground tells room.
Moving entity sources now pan and attenuate from their current cell, including
existing forest owner-bound sources.

## Local scenery ownership

Nine sparse 16x16 fittings: window, chain, hatch, pipe, wheel, gear, flag, drip
opening and chimney. Scene placement derives from the generated terrain and
existing apparatus without advancing `game.rng`. At most four of each fitting,
spaced sixteen tiles apart; room geometry determines which are possible. A floor
without pools or machinery does not receive their sounds arbitrarily.

Fittings are cosmetic overlays, not interactive props or hidden collisions. Pipes
are harmless leaks, not heat sources; hatches do not summon enemies. They use
normal world lighting and sit behind actors and usable props. Small rear fittings
leave the stove/lamp readable. Source rings and smoke use existing local particles.
All source schedules, handles, scene lists, mixer voices, ornament motion and
particles live outside snapshots and hashes. No network format change.

Removing a supporting wall, changing a site's floor material, covering it with a
prop or destroying its apparatus suppresses the relevant decoration and audio.
The original scene stays anchored, so demolition does not cause ornaments to
wander onto another wall. Fuel-empty tanks stop their loop; dead/reused entity
handles fail normally. An extinguished stove retains its visible cold chimney.

## Validation and remaining work

Strict game and static-render builds passed. The twenty new audio files decode
as finite mono samples with headroom; loops include offline overlap blending.
Nine new sprites are 16x16 RGBA. Static mixed-fixture and generated floor captures
were inspected; placement summaries cover all twenty cue types across the fixture
and generated terrain. Existing gameplay was not advanced for these captures.
No live playtest or new test suite was run. Final listening, balance and subjective
mixing remain the user's playtest feedback.

Forest and Ice now each have twenty authored/placed ambient cues. The other two
biomes, remaining Ice items/debris, richer authored rooms and unique floors are
still unfinished. This does not complete the master ambience or biome tasks.
