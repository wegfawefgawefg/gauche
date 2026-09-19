# First music sketches

The user's listens led to a new, less melodic direction. See
[pass 02: atmosphere studies](atmosphere/README.md) for 15 new candidates,
including title/menu and short cues. This first batch is preserved for comparison.

Open `assets/music/sketches/index.html` in a browser. It uses local relative OGG
files, native audio controls, optional looping and section markers; no server,
network connection or automatic playback is needed.

These original arrangements are listening previews, not the completed 3:30 biome
catalog or final instruments/mixes. They currently do not replace game music.
The user will listen and steer the next pass.

- **Under the Hollow Bough**, 1:40: Dorian woodland, cello/bassoon, answering flute,
  harp and soft frame drum. A contrasting middle changes the harmonic color.
- **Lanterns among Ferns**, 1:31: darker clarinet theme, pizzicato pulse and a bowed
  cello answer. Sparse upper harmonics above a substantial lower register.
- **The Shift Below**, 1:34: low horns, cello pulse, modal drums and distant metal;
  a stopped-machinery passage leaves space before the rhythm returns.

## Rebuild

Available on the authoring machine: Python 3, NumPy, SciPy, Matplotlib,
FluidSynth 2.3.4 with `/usr/share/sounds/sf2/FluidR3_GM.sf2`, and FFmpeg.
No game or sound card is opened by these scripts.

```sh
python3 tools/music/render.py
python3 tools/music/listening_page.py
python3 tools/music/analyze_audio.py
```

Render just one with `--only hollow_bough`; use `--soundfont /path/to/FluidR3_GM.sf2`
or `--output /path/to/previews` as needed. The page/inspection scripts currently
operate on the default preview directory.

The score files hold named pitches, note lengths, authored melodies, harmony,
phrases, instrumentation, expression curves and section changes. Fixed seeds add
small performance variations and generate percussion/textures. No MIDI file is
required; FluidSynth supplies sampled instrument voices, not the composition.
Membrane drums, metal resonances and bowed-air beds are generated in Python.
Render sources use one stereo room with damped reflections; natural tails fold
back across the loop seam. Final full-length arrangements still need listening
and seam refinement if those loop transitions feel musically awkward.

Masters use a single fixed gain aiming at approximately -23 LUFS with at least
3 dB true-peak headroom, rather than flattening dynamics with a limiter. The
analysis filter measures only; its compressed output is discarded. The encoded
OGGs are measured again by `analyze_audio.py`. Numerical checks do not certify musical
quality or audibility in a busy game mix.

## Instrument provenance

Compositions and custom synthesized layers are original to this Teeming pass.
The supplied video references were not sampled or embedded in these renders.
Sampled instruments use **FluidR3** by Frank Wen and contributors, released under
the MIT license. See `assets/music/sketches/INSTRUMENT_LICENSE.txt` for its copyright
and license notice. The SoundFont itself is an external authoring dependency.
