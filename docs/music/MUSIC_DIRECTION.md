# Music direction — 2026-09-15

User feedback: the current music is much too loud and is not the desired musical
direction. Replace it with original, deliberately composed music. Lowering the
volume is a separate immediate fix, not completion of the composition work.
MASTER_TASKS.md remains the execution checklist.

## Listening feedback after the first three drafts

The user's listens to *Lanterns among Ferns*, *Under the Hollow Bough* and *The
Shift Below* supersede the first pass's melodic/orchestral interpretation below.
Preserve those drafts, but start the next compositions from scratch:

- Much more negative space, longer held sounds, stranger timbres and an
  avant-garde atmospheric direction. Some atmospheric horror is welcome.
- Avoid the obvious foreground melody plus backing part. The first pass felt
  cheesy/toyish, especially the apparent fake violin/cello duet. A low cello-like
  background has useful weight, but sampled strings are not the required sound.
- The Fern horn's **timbre** was promising; its melody was too clear and its level
  too prominent. Keep any similar voice quiet, near the other material. The user
  also mentions SimCity 4 as an atmospheric reference, not a tune to reproduce.
- Explore actual feedback/resonance synthesis and less familiar instruments.
  Do not merely change SoundFont programs around the same arrangement.
- Percussion should suggest the environment: hollow coconuts/wood, rock on rock,
  steel clangs. The occasional slightly off chime in Shift Below was acceptable.
  Drums and chimes sounded less artificial than the sampled strings.
- Supply a **broad, varied set** for comparison, including title/menu directions
  and win/lose/transition cues now, rather than postponing them until after one
  exploration style is settled. The user needs enough alternatives to say
  “more like this, less like that.”

Pass 02 delivers [15 original atmosphere studies and cues](ATMOSPHERE_STUDIES.md)
with a local listening gallery and comparison reel. These are audition sketches;
listening feedback, full-length development and game integration remain open.

## Scope and character

### Feedback on pass 02 / approval boundary

The user found the second batch cool as a sound, but **all essentially the same**.
Different event spacing and nominal material names did not create different
musical identities. Do not use that pipe texture as the whole game's soundtrack.
**Shop Arrives — wheels and a crooked bell is approved** for shop arrival: preserve
`assets/music/atmosphere/shop_arrives.ogg` unchanged when integrating it. None of
the other pass-02 pieces are accepted for use as-is. Their material may become
**one developed Ice song**, which needs more happening/real development.

Pass 03 provides [one 3:30 Ice candidate and four contrasting studies](../../tools/music/contrasts/README.md).
Separate the dominant instruments, rhythm, register, space and composition—not
just presets within one resonant drone. The user then asked to finish the current
small music batch and return to game content. Further music iteration waits for
listening feedback; do not keep generating indefinitely.

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
- Industry instrumentation candidates from the user: drums, war drums and horns.
  Explore their weight and pulse without requiring every passage to be a march.
- Use a full tonal range: deep frequencies, substantial middle voices such as
  cellos, and higher detail. The score should not become exclusively small,
  bright plucked sounds. Let low strings, bass and sustained voices carry phrases
  as well as lighter instruments; these are options, not a mandatory orchestra.

## User-supplied references

Local folder: `/home/vega/Music/samples/`. Files found:

- `Russell Shaw Fable Summer Fields Extended 1 Hr..mp4`
- `Fable II - Bowerlake Best Quality.mp4`
- `Donkey Kong Country 2 OST - Forest Interlude.mp4`
- `Final Fantasy Crystal Chronicles Echoes of Time - Forest.mp4`
- `Tekken 6 Edge of Spring.mp4`
- `Spyro 2 - Summer Forest.mp4`

These filenames have been inventoried; their audio has **not yet been analyzed**.
Keep the samples available as ongoing inspiration and return to them selectively
while composing. The user explicitly says there is no need to investigate all
tracks now or make reference analysis a prerequisite to writing music.
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
MIDI itself is entirely optional: a simple text note notation or an iterated
composition script is equally acceptable. Choose the representation that helps
write and revise the music; no format/framework requirement should hold it up.

The initial small Forest-led sketch pass was useful but too narrow. Following
the user's listening feedback, compare a broader set of genuinely different
material/space directions and short cues before choosing full arrangements.
Use expressive envelopes, voicing, articulation, humanized performance where
appropriate, and restrained space/effects. Preserve Teeming's quirky identity.
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
Respect dynamic range: preserve quiet passages, stronger swells, expressive
attacks and decays. A quieter overall mix must still have body and contrast;
avoid flattening everything into constant loudness. Full low-to-high frequency
coverage and loud-to-quiet musical movement are both desired, but need not occur
all at once throughout every passage.

Add biome-aware selection with multiple tracks, avoiding immediate repeats,
gentle transitions and stable playback across menus/rooms. Handle boss entry,
victory, shop arrival and return to exploration explicitly. Jingles should not
stack on top of one another; use brief music ducking or a musical handoff where
appropriate. A paused/resumed game or a reconnect should not restart cues in a
burst. Track selection/playback remains local presentation state, outside
gameplay RNG, hashes and snapshots.

First preview milestone: [three original sketches](MUSIC_SKETCHES.md)
and their local listening page are available for the user's feedback. These
shorter arrangements are not completion of the full song catalog.
`audio.cpp` still loads only `title.ogg` and `playing.ogg`; their measured loudness
was -8.97 LUFS before playback gain. Music gain is now `0.12 * master * music`
(previously 0.4125), in both start and settings-update paths. Biome-aware playback
and installing the new compositions in the game remain unimplemented.

Use offline listening/render/analysis checks for production and the user's
playtesting for whether it feels right in play. Finishing a render or hitting a
duration target does not certify musical quality.
The user will do repeated listening and give feedback; provide editable musical
passes that can improve through those listens, rather than treating the first
render as final.
