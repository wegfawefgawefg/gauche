# Industry ambience and material scraps

Implemented September 15, 2026. Music remains parked; these are local environmental
sounds, independently mixed beneath combat. Industry now has eight of twenty
ambient families and ten of twenty catalog debris materials integrated. This adds
seven cues and two new materials; copper curls and coal crumbs already worked and
are now correctly marked in the Industry catalog. Remaining rows are candidates.

## Sound behavior

- Furnace breath: uneven low combustion noise follows loaded, undamped Stokers
  and fueled boiler tanks. Empty/frozen sources stop; damp Stokers stop.
- Distant picks: filtered work impacts follow a peaceful foreman only while a
  living linked Pickhand is cutting. Near listeners (within six cells of the
  foreman) hear the existing action cues instead. Minimum repeat interval 12s.
- Chain sway: irregular chain contacts follow crane travel, lock and return.
  Idle, sleeping, stunned and frozen cranes stop the loop.
- Cooling ticks: quiet metal contraction beside frozen cranes/tanks, or wet
  tanks below 25 pressure. Minimum repeat interval 10s.
- Slag bubbling: low glugs attached to actual lava. Quenching that source tile
  turns the loop off. Up to sixteen separated basins may be registered.
- Water hammer: pipe knocks at unfrozen, fueled tanks above 30 pressure.
  Minimum repeat interval 18s.
- Shift bell: rare quiet distant clank, minimum repeat interval 60s. Pure local
  atmosphere; it never issues crew orders or wakes enemies.

Existing positional roller ambience remains. New cues use the shared 10Hz local
scheduler, six simultaneous loop voices, three event voices, 2.5s global spacing
between events, smooth gain and stereo falloff. Stated repeat intervals apply
following playback; initial cooldown is half the nominal interval. The emitter
list remains capped at 128. Cue-specific limits allow spread across the large
Industry floor without increasing simultaneous voices. Actors are followed with
generation-safe handles; removed/reused slots cannot keep a machine sounding.

Placement occurs on floor load. This pass does not implement discovery of new
machines created mid-floor. Nearby same-family sources are spatially deduplicated;
not every machine has its own voice. Gameplay RNG, hearing and hashes are untouched.

## Loose material

Crates emit one/two bent nails alongside their existing fragments. Destroyed
Magnet Cranes emit three chain links alongside five steel washers. Both use the
shared cosmetic floor collection: rotation, foot/rake impulses, clipped contact
shadows, terrain collision and settling. Friction is .75 for nails and .70 for
links. No fragment-to-fragment simulation, no loot, no network state and no new
floor-wide litter pass. Height/bouncing is not implemented for these fragments.

Existing copper curls already come from cut cables, scrap bins and boiler
breakage; coal crumbs come from thrown coal and spent/broken stoves. Reuse is
intentional and is not counted as four newly created materials. The ore-bin
catalog also now records its already implemented 15% Coolant Can drop range.

## Assets and verification

`tools/sound/industrial_ambience.py` generates all seven original Vorbis assets
under `assets/ambience`. Long beds use seeded full-length noise and overlapping
ends; no runtime synthesis. Loops are 10–12 seconds, events 2.2–5.2 seconds.
Decoded peaks measured .229–.325 and RMS .014–.064; no clipping. Source gain is
.09–.19 before spatial attenuation. Listening/mix approval remains with the user.

Original generated transparent sprites were copied unchanged:

- `debris_bent_nail.png`: `exec-c3b491d1-0292-4fff-b5a4-211fdf8358fa.png`
- `debris_chain_link.png`: `exec-85370a7f-65de-4c30-8ed5-d10ddcb63deb.png`

Source directory: `/home/vega/.codex/generated_images/01a09a6b-6ba6-7143-be03-94f4eb87aa9b/`.
Both were visually inspected, have actual alpha, and render as small subdued
fragments at gameplay scale. Static SDL software capture:
`/tmp/teeming-industry-scraps.png`.

Strict release build passed. Temporary direct checks covered condition changes,
near/far work sounds, dead/reused entity handles, cue registration/source limits,
loading every ambience asset through SDL_mixer, unchanged game hashes during
local audio/debris updates, crate nails and crane death links. No interactive
playtest or permanent test suite was added. No gameplay protocol change.
