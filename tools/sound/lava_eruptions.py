"""Seeded pressure swell and a brief molten launch gulp, rendered offline."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np

RATE = 44100
ROOT = Path(__file__).resolve().parents[2] / "assets" / "sounds"
rng = np.random.default_rng(318)

def save(name, signal):
    signal *= .34/max(.01, float(np.max(np.abs(signal))))
    with tempfile.TemporaryDirectory() as folder:
        path = Path(folder)/"sound.wav"
        with wave.open(str(path), "wb") as f:
            f.setnchannels(1); f.setsampwidth(2); f.setframerate(RATE)
            f.writeframes((signal*32767).astype("<i2").tobytes())
        subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", str(path),
                        "-c:a", "libvorbis", "-q:a", "4", str(ROOT/(name+".ogg"))], check=True)

t = np.arange(int(RATE*1.15))/RATE
noise = rng.uniform(-1, 1, len(t))
low = np.convolve(noise, np.ones(211)/211, mode="same")
envelope = np.sin(np.pi*t/1.15)**.8
signal = (low*2+np.sin(2*np.pi*(57*t+22*t*t))*.07)*envelope
for start in (.05, .28, .47, .63, .77, .88):
    age = np.maximum(0, t-start)
    signal += (t>=start)*np.sin(2*np.pi*(175*age-60*age*age))*np.exp(-age*18)*.06
save("lava_rumble", signal)
t = np.arange(int(RATE*.48))/RATE
noise = rng.uniform(-1, 1, len(t))
signal = noise*np.exp(-t*22)*.08
signal += np.sin(2*np.pi*(240*t-185*t*t))*np.exp(-t*14)*.23
signal *= 1-np.exp(-t*400)
save("lava_pop", signal)
