# Distinct palettes — pass 03

[Listening gallery](../../../assets/music/contrasts/index.html), including a
1:45 reel. Five new candidates total 9:12. The approved **Shop Arrives** cue is
linked unchanged, not re-rendered or copied into another competing version.

The user found pass 02's supposedly different tracks all sounded like the same
pipe palette. Only its shop cue is accepted for use. The pipe material might
support **one** Ice song, with further development; the other tracks are not
approved soundtrack selections.

- **Beneath the Thaw**, 3:30: that one Ice candidate, with six authored sections.
  Brittle pulse groups gather/change/disappear; coherent rubbed-glass upper
  sounds carry a sparse recurring figure. The exposed middle drops the low pipe
  completely before a changed return. This full-length render still needs ears.
- **Seed and String**, 1:24: Forest prepared strings, low plucks, hollow wood and
  seed rattles. Dry small groups with changing pitch/register and gaps.
- **Counterweight**, 1:28: Industry's seven-step skin/metal rhythm, rack scrapes,
  rough oscillator bass and a shutdown/restart. No sustained resonator bed.
- **Watchtower Relay**, 1:18: provisional fourth-zone dry phase-modulated relays,
  clipped bass and radio-like interruptions. Fourth-biome identity is not settled.
- **The Crooked Door**, 1:32: title alternative using stretched/reversed scraps
  of original prepared-string synthesis around occasional low plucks.

`build.py` rejects use of the previous breath/friction/glass beds in the four
non-Ice sketches. This is a production constraint to prevent repeating the last
mistake, not evidence that the new pieces are subjectively successful.

## Sources / rendering

`palette.py` supplies separate excitation families: a plucked lossy string loop,
granular resampling of that original voice, coherent glass overtones, harmonic
oscillator bass, inharmonic phase-modulated relays, rattles, rack scrapes and
band-limited radio textures. There are no outside recordings or SoundFont patches.

The previous renderer accepts an optional instrument map; its old default path
and earlier scores are unchanged. Shared export, metering and routing do not
force the same composition or instrument. Reverb is deliberately shorter/drier
outside Ice, except the title's fragment space.

Use the same NumPy/SciPy/Numba/FFmpeg environment as pass 02:

```sh
/tmp/teeming-music-venv/bin/python tools/music/contrasts/build.py
/tmp/teeming-music-venv/bin/python tools/music/contrasts/release.py
```

`--only beneath_the_thaw` rebuilds one track; `--masters /some/path` also saves
lossless float WAVs outside the repo. Seeds reproduce the source gestures.
Each JSON sidecar contains events/markers and measured encoded loudness.
One fixed mix gain preserves dynamics. The Forest/title masters are quieter
than their requested -26 LUFS because their transient headroom takes priority.
No limiter is used to make all pieces equally dense.

The release script decodes all five exports, checks duration, finite samples,
peak headroom and small loop-boundary steps, then builds a reel without changing
the excerpt levels. User listening approval and game integration remain open.
