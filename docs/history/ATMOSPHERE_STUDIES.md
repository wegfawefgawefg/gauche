# Atmosphere comparison pass — 2026-09-15

The first three drafts sounded too cheesy, melodic and artificially orchestral
to the user. This pass starts new compositions, preserving the original OGGs
unchanged. The revised direction is recorded in [the music brief](../MUSIC_DIRECTION.md).

## Listening artifacts

[Open the new gallery](../../assets/music/atmosphere/index.html), starting with
its **2:51 comparison reel**. It contains numbered excerpts from every candidate,
then full-track controls, section buttons, optional loops and private browser
notes that can be downloaded. It works locally without a web service.

There are **15 candidates, totaling 15:15**:

- Three Forest studies: warm sustained wooden breath; dry coconut/wood contacts
  with much less tonal bed; and a darker wire/wood horror direction.
- One Ice study: sustained rubbed-glass-like feedback with low pressure and
  sparse stone contact.
- An Ice/Industry pipe-and-droplet transition, and a heavier Industry study with
  interrupted hollow percussion and an occasional slightly off bell.
- One threat/boss percussion candidate.
- Two title/menu studies: *The Door in the Roots* and *A Machine Dreams of Moss*.
- Six short cues: title mark, level entry, level clear, run won, run lost and shop
  arrival. These are finite 8–15-second candidates, not looping background beds.

The nine longer studies range from 1:12 to 1:48. They are for choosing directions
before full-length arrangements, not fulfillment of the ~3:30-per-song biome
catalog. No preview music or cue has been installed into gameplay yet.

## Production

Sources and rebuilding instructions are in
[tools/music/atmosphere](../../tools/music/atmosphere/README.md). The new renderer
uses original synthetic material resonances rather than SoundFont strings:
coupled nonlinear feedback delay lines for held cavities, noise-excited resonant
rubbing, inharmonic impact modes and a damped stereo feedback reverb.

Scores specify long gestures, irregular small contact groups, different levels
and empty intervals in seconds. There is no melody-plus-chord-accompaniment
template, generated drum-kit backing, or copied reference recording. Source
scores and seed-controlled synthesis remain editable; OGG JSON sidecars record
the exact gestures for timestamp-specific feedback.

## Verification and remaining judgment

All 15 exported OGGs decoded to their intended duration with finite samples and
no clipping. Actual encoded loudness is approximately -26 LUFS for the atmosphere
and title pieces, -25 for the threat study and -27 for short cues. The loudest
encoded true peak is **-8.22 dBTP**. One fixed gain preserves dynamics; the meter's
normalized output is discarded. Long-study measured loudness ranges are
**11.5–20.1 LU**.

Decoded loop boundary steps are at most **0.0000644** full-scale amplitude; finite
cues end at zero. The waveform/spectrogram overview was inspected and the local
gallery was rendered with headless Chrome. Original sketch audio SHA-256 checks
passed unchanged. No game windows or gameplay sessions were opened.

Measurements and plots are in
[encoded_analysis.json](../../assets/music/atmosphere/encoded_analysis.json) and
[analysis.png](../../assets/music/atmosphere/analysis.png). They establish signal
properties, not that these are musically good or the right degree of horror.
The user still needs to judge timbre, texture, silence, repetition, musical loop
return and which short cues communicate their intended events. Full catalog,
soundtrack selection/mixing and actual cue handoffs remain open.
