"""Seeded short molten contact: low gulp, sharp hiss, receding crackles."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np

RATE = 44100
ROOT = Path(__file__).resolve().parents[2] / "assets" / "sounds"
rng = np.random.default_rng(913)
t = np.arange(int(RATE * .65)) / RATE
noise = rng.uniform(-1, 1, len(t))
low = np.convolve(noise, np.ones(41)/41, mode="same")
signal = (noise*.22 + low*.8) * (1-np.exp(-t*350)) * np.exp(-t*10)
signal += np.sin(2*np.pi*(145*t-80*t*t)) * np.exp(-t*18) * .24
for start in (.015, .065, .12, .23):
    age = np.maximum(0, t-start)
    signal += (t>=start)*np.exp(-age*100)*noise*.17
signal *= .38/max(.01, float(np.max(np.abs(signal))))
with tempfile.TemporaryDirectory() as folder:
    path = Path(folder) / "sound.wav"
    with wave.open(str(path), "wb") as f:
        f.setnchannels(1); f.setsampwidth(2); f.setframerate(RATE)
        f.writeframes((signal*32767).astype("<i2").tobytes())
    subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", str(path),
                    "-c:a", "libvorbis", "-q:a", "4", str(ROOT/"lava_sizzle.ogg")], check=True)
