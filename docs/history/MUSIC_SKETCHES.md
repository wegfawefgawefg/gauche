# First original music previews — 2026-09-15

[Listening page](../../assets/music/sketches/index.html), with three local OGG
players, section-jump buttons and optional looping. No autoplay or server.

| Sketch | Direction | Length | Encoded LUFS | True peak | Loudness range |
| --- | --- | ---: | ---: | ---: | ---: |
| Under the Hollow Bough | Forest: cello, bassoon, answering flute, harp, frame drum | 1:40 | -22.98 | -3.31 dBTP | 8.2 LU |
| Lanterns among Ferns | Forest: clarinet, bowed cello, pizzicato, sparse upper harmonics | 1:31 | -22.97 | -6.27 dBTP | 7.6 LU |
| The Shift Below | Industry: low horns, cello pulse, membrane drums, distant metal | 1:34 | -22.98 | -5.35 dBTP | 14.0 LU |

Original authored motifs, chord movement, contrasting passages and reprises.
Scores are editable Python note/phrase notation, rendered offline with FluidR3
sampled instruments plus custom synthesized percussion and low bowed-air beds.
Instrument provenance/MIT notice is included with the previews. No reference-video
audio was incorporated. The supplied references remain available for later
inspiration; no complete reference-analysis detour was taken.

The mix uses fixed per-song gain, measured headroom and folded release/reverb
tails. Its measurement pass does not apply dynamic compression. The actual OGG
exports were decoded and checked: finite samples, no clipping, expected durations,
small boundary discontinuities, and waveform/spectrogram inspection showing
low/mid/high content and section-level dynamics. Numerical analysis does not
substitute for the user's listening, especially for timbral quality and musical
loop transitions.

The original `title.ogg` and `playing.ogg` both measured -8.97 LUFS, +4.12 dBTP
before the game gain. Runtime music gain is now **0.12 instead of 0.4125**, about
10.7 dB lower, consistently at song start and when updating settings. Existing
user sliders are preserved. The game builds with the change.

These sketches are not installed as the gameplay soundtrack and do not count as
the finished multi-track biome quota. Full ~3:30 arrangements, Ice/fourth music,
boss/menu/event pieces and biome-aware playback remain open. The next musical
iteration should follow the user's listens. Return to game implementation after
this preview milestone, as requested.

Authoring/rebuild details: [tools/music/README.md](../../tools/music/README.md).
