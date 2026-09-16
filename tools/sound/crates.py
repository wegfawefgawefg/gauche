"""Dry hollow box knocks and a burst of splintering boards, seeded offline."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'sounds'
RATE = 44100

def save(name, signal):
    signal *= .38 / max(.01, float(np.max(np.abs(signal))))
    # Avoid a discontinuity at either edge of the encoded sample.
    signal[:88] *= np.linspace(0, 1, 88)
    signal[-220:] *= np.linspace(1, 0, 220)
    with tempfile.TemporaryDirectory() as folder:
        path = Path(folder) / 'crate.wav'
        with wave.open(str(path), 'wb') as f:
            f.setnchannels(1); f.setsampwidth(2); f.setframerate(RATE)
            f.writeframes((signal * 32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg', '-v', 'error', '-y', '-i', str(path),
                        '-c:a', 'libvorbis', '-q:a', '4', str(ROOT / (name + '.ogg'))], check=True)

for index, name in enumerate(('crate_knock1', 'crate_knock2', 'crate_break')):
    rng = np.random.default_rng(871 + index)
    t = np.arange(int(RATE * (.7 if index == 2 else .3))) / RATE
    noise = rng.uniform(-1, 1, len(t))
    body = np.convolve(noise, np.ones(19) / 19, mode='same')
    a = body * np.exp(-t * 48) * .9 + noise * np.exp(-t * 180) * .18
    # Inharmonic short panel resonances, not a pitched musical note.
    for freq, decay, weight in ((173, 32, .22), (289, 46, .13), (463, 65, .06)):
        a += np.sin(2 * np.pi * t * freq * (1 + index * .045)) * np.exp(-t * decay) * weight
    starts = (.0, .026, .059, .11, .16, .235, .31) if index == 2 else (.012, .034)
    for k, start in enumerate(starts):
        age = np.maximum(0, t - start)
        a += (t >= start) * np.exp(-age * (85 + k * 7)) * (noise * .3 + body) * (1 - k * .09)
    save(name, a)
