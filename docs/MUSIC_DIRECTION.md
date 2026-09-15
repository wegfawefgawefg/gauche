# Music direction — 2026-09-15

User feedback: the current music is much too loud and is not the desired musical
direction. Replace it with original, deliberately composed music. Lowering the
volume is a separate immediate fix, not completion of the composition work.
MASTER_TASKS.md remains the execution checklist.

## Scope and character

- A few exploration songs per biome, typically about **3 minutes 30 seconds**
  each, designed for seamless looping. First planning target: three per biome
  (twelve across four zones), subject to iteration rather than a settled ceiling.
- Atmospheric, place-specific music with occasional memorable melodic phrases.
  Give Forest the user's Fable-like woodland/fairytale direction. Avoid generic
  space ambience, constant lead melody, and a short pattern repeated unchanged
  for three minutes. Mood, musical character and earworms all matter.
- Develop an actual arrangement: contrasting passages, motif variations,
  instrumental entrances/exits, rhythmic changes and room for quiet. The loop
  boundary should feel intentional without every phrase sounding identical.
- Boss music is a requested candidate. Initial planning proposal: one shorter
  boss loop per biome, with encounter-specific additions where useful. Keep
  tension and momentum without masking attack tells.
- Include main-menu music and short identity/transition cues for starting a
  level, winning a level and a shop appearing. Establish related motifs without
  playing the same loud flourish at every transition. A full shop loop is a
  candidate, not a substitute for the requested arrival jingle.
- The fourth biome's musical identity follows its eventual world identity;
  military/robot remains provisional. Forest, Ice and Underworks should each
  have their own harmonic, rhythmic and instrumental character.

## User-supplied references

Local folder: `/home/vega/Music/samples/`. Files found:

- `Russell Shaw Fable Summer Fields Extended 1 Hr..mp4`
- `Fable II - Bowerlake Best Quality.mp4`
- `Donkey Kong Country 2 OST - Forest Interlude.mp4`
- `Final Fantasy Crystal Chronicles Echoes of Time - Forest.mp4`
- `Tekken 6 Edge of Spring.mp4`
- `Spyro 2 - Summer Forest.mp4`

These filenames have been inventoried; their audio has **not yet been analyzed**.
Only audio matters for this task. The user authorizes extracting/chopping study
sections, examining waveforms, amplitude distributions and time/frequency plots
at multiple resolutions, and consulting online descriptions when useful.
Listen to representative passages and examine arrangement, phrasing, timbre,
space and dynamics as well as spectral energy. Plots alone do not establish
whether a tune is good. Use references to guide original compositions; do not
ship the reference recordings or copy their melodies.

## Offline production

Generate finished audio assets offline. The user explicitly permits installing
suitable tools/libraries and using instrument synthesis, analog-style synthesis,
Python DSP, samplers or a mixed workflow. MIDI may be useful as a score/control
format; default MIDI playback is not the intended final timbre. No obligation to
use only sine waves, the current effects synth or an in-game procedural sequencer.

Start with a representative Forest composition and a small set of timbral
sketches, then develop the full arrangement before multiplying it across zones.
Use expressive envelopes, voicing, articulation, humanized performance where
appropriate, and restrained space/effects. Preserve Gauche's quirky identity.
Keep reproducible composition/render sources alongside asset provenance and
instructions so motifs, instrumentation and mixing remain editable.

Validate exported loop joins, including decay/reverb tails and codec playback.
Retain enough arrangement development within each song to survive repeated
exploration. Author short cues to end cleanly or hand off musically into a loop.

## Mixing and playback

Music supports play at a modest default level. It must leave combat tells,
footsteps, environmental sources and menu cues intelligible. Measure current and
new files for perceived loudness, peaks and clipping; set deliberate per-track
gain rather than treating peak normalization as equal perceived volume.
Keep the user's saved master/music/SFX sliders functional and unchanged.

Add biome-aware selection with multiple tracks, avoiding immediate repeats,
gentle transitions and stable playback across menus/rooms. Handle boss entry,
victory, shop arrival and return to exploration explicitly. Jingles should not
stack on top of one another; use brief music ducking or a musical handoff where
appropriate. A paused/resumed game or a reconnect should not restart cues in a
burst. Track selection/playback remains local presentation state, outside
gameplay RNG, hashes and snapshots.

Current source audit: `audio.cpp` loads only `title.ogg` and `playing.ogg`; music
gain is `0.4125 * master * music` in both start and settings-update paths. This
identifies existing behavior, not a measurement of the files' loudness. Multiple
biome tracks, the new compositions and the new mixing policy remain unimplemented.

Use offline listening/render/analysis checks for production and the user's
playtesting for whether it feels right in play. Finishing a render or hitting a
duration target does not certify musical quality.
