# Atmosphere studies — pass 02

Open [the listening gallery](../../../assets/music/atmosphere/index.html).
Its 2:51 comparison reel includes all 15 candidates, with numbered seek buttons.
Full pieces, section markers, optional looping, notes and OGG downloads are below.
Only one player plays at a time. No server, account or autoplay is involved.
Browser notes can be exported as text; local storage is best-effort for file URLs.

The user's feedback rejected the first pass's obvious melody/backing structure
and fake sampled-string sound. These are new compositions with extended material
gestures, irregular contact rhythms and deliberate gaps. The old sketches are
unchanged. Neither preview batch is installed as the game soundtrack.

## What to compare

| # | Piece | Length | Direction |
|---|---|---|---|
| 01 | Root Lung | 1:36 | Forest: quiet, long wooden breaths |
| 02 | Knots in the Bark | 1:28 | Forest: dry hollow contact; much less tonal bed |
| 03 | Something under the Footbridge | 1:34 | Forest horror: strained wire beneath wood |
| 04 | Ice Has Weight | 1:42 | Ice: held rubbed glass and low pressure |
| 05 | Thaw in the Pipes | 1:32 | Transition: hollow drops and soft pipe/metal |
| 06 | After the Last Shift | 1:40 | Industry: interrupted low skin/steel pulse |
| 07 | Do Not Wake the Press | 1:12 | Threat/boss rhythm study |
| 08 | The Door in the Roots | 1:48 | Warmer, uneasy title/menu direction |
| 09 | A Machine Dreams of Moss | 1:38 | Stranger, less pastoral title/menu direction |
| 10–15 | Title mark, entry, clear, win, loss, shop | 8–15s | Six finite cues |

The nine studies are shorter direction previews, not the final ~3:30 catalog.
The loop's quiet interval is part of the arrangement. Short cues end rather than
loop. Musical identity, boredom, harshness and actual in-game audibility need the
user's ears; spectral/loudness checks cannot approve those things.

## Rebuild

Authoring dependencies: Python 3, NumPy, SciPy, Numba, Matplotlib, FFmpeg with
libvorbis. This pass does **not** use FluidSynth, SoundFonts or MIDI.
The initial render used NumPy 1.26.4, SciPy 1.11.4 and Numba 0.67.0.
Numba compiles the feedback recurrences; it is only an offline authoring dependency.
Do not add it to the game's build dependencies.

```sh
python3 -m venv --system-site-packages /tmp/teeming-music-venv
/tmp/teeming-music-venv/bin/pip install numba
/tmp/teeming-music-venv/bin/python tools/music/atmosphere/render.py
/tmp/teeming-music-venv/bin/python tools/music/atmosphere/publish.py
```

If the system lacks the numerical/plotting packages, install `numpy scipy
matplotlib numba` in that venv. FFmpeg is an external executable dependency.
Use `render.py --only root_lung` to iterate one piece. `--masters /some/path`
also writes float WAV masters (OGGs and editable sources belong in the repo;
bulky lossless intermediates do not). `--output` redirects rendered tracks;
the publishing script intentionally uses the repository's default gallery folder.

## Synthesis and arrangement

- `materials.py`: four coupled delay lines with Householder scattering,
  frequency-dependent damping, fractional delay movement and nonlinear feedback
  return. Sustained, irregular pressure excites wooden, glass, wire and pipe-like
  cavities. These are synthetic material suggestions, not claims to accurate
  physical models of named real instruments.
- Strikes use inharmonic damped modes, imperfect tuning, contact noise and
  pitch relaxation where appropriate. Rubbing uses noise-driven IIR resonators.
  Wood, coconut, stone, steel, glass/ice, bell and skin have different ratios and
  decay profiles. The occasional metal beat is intentionally slightly uneven.
- A separate damped feedback delay network supplies stereo late reflections.
  Wet amount and decay differ between the drier wood study and larger ice/pipe
  spaces. Low voices stay close to center; contact sounds use moderate spread.
- `forest.py`, `zones.py`, `titles.py`: authored event times in seconds, timbres,
  registers, levels, sparse rhythm groups and section markers. No global tempo
  grid or generated chord/melody/accompaniment template. Seeds make rendering
  reproducible, not gameplay-random.
- `render.py`: stereo placement, periodic reverb/filter pre-roll, DC/sub-bass
  cleanup below 30 Hz, one fixed gain, OGG export and encoded loudness measurement.
  JSON sidecars also contain each source score for timestamp-based feedback.
- `publish.py`: decode/check exports, inspect waveforms/spectra, assemble a short
  unnormalized excerpt reel, and build the self-contained listening page.

Most studies target -26 LUFS, the threat study -25, and cues -27. A -7 dBTP
pre-encode ceiling takes priority if necessary. FFmpeg loudnorm is used only as
a meter: its processed stream is discarded. No compressor/limiter erases the
swells and rests. Inspect the actual encoded measurements in
`assets/music/atmosphere/encoded_analysis.json` and `analysis.png`.

Loop boundaries are checked against actual decoded samples. That catches a hard
join, not a musically awkward return. Render pre-roll settles delay/filter state;
arranged sound envelopes also end before the boundary. Cues reserve decay time
and fade their final half-second to zero.

All compositions, excitation signals and synthesis are original to this pass.
No supplied reference recording or third-party instrument sample is embedded.
The Fable/other local references remain inspiration available for future passes.
