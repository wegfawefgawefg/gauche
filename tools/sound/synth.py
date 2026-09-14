"""Offline mono SFX building blocks. No audio synthesis runs in the game."""
from pathlib import Path
import subprocess
import tempfile
import wave

import numpy as np
from scipy import signal

RATE = 44100
ROOT = Path(__file__).resolve().parents[2] / "assets" / "sounds"


def clock(seconds):
    return np.arange(round(seconds * RATE)) / RATE


def noise(length, seed):
    # LFSR: The same seed makes the same source material on every regeneration.
    state = seed or 1
    result = np.empty(length)
    for i in range(length):
        state = (state >> 1) ^ (0xB400 if state & 1 else 0)
        result[i] = state / 32767.5 - 1
    return result


def band(samples, low, high):
    return signal.sosfilt(signal.butter(2, [low, high], btype="bandpass",
                                      fs=RATE, output="sos"), samples)


def save(name, samples, peak=0.65):
    # HEADROOM: Short endpoint fades prevent clicks; no clipped normalization.
    samples = np.asarray(samples, dtype=float)
    samples -= samples.mean()
    fade = min(220, len(samples) // 4)
    samples[:fade] *= np.linspace(0, 1, fade)
    samples[-fade:] *= np.linspace(1, 0, fade)
    samples *= peak / max(0.001, np.max(np.abs(samples)))
    ROOT.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as temporary:
        source = Path(temporary) / "source.wav"
        with wave.open(str(source), "wb") as stream:
            stream.setparams((1, 2, RATE, 0, "NONE", "not compressed"))
            stream.writeframes((samples * 32767).astype("<i2").tobytes())
        subprocess.run(["ffmpeg", "-loglevel", "error", "-y", "-i", str(source),
                        "-c:a", "libvorbis", "-q:a", "5", str(ROOT / (name + ".ogg"))],
                       check=True)
    print(f"{name}: {len(samples)/RATE:.2f}s, peak {peak:.2f}")
