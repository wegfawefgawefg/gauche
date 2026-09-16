"""Quiet timber strain, falling fragments and an icy melt/clatter, offline."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'sounds'
RATE = 44100
rng = np.random.default_rng(817)

def save(name, signal):
    signal *= .26 / max(.01, float(np.max(np.abs(signal))))
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory) / 'source.wav'
        with wave.open(str(path), 'wb') as out:
            out.setnchannels(1); out.setsampwidth(2); out.setframerate(RATE)
            out.writeframes((signal * 32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg', '-v', 'error', '-y', '-i', str(path), '-c:a', 'libvorbis',
                        '-q:a', '4', str(ROOT / (name + '.ogg'))], check=True)

t = np.arange(int(RATE * .8)) / RATE
noise = rng.uniform(-1, 1, len(t))
grain = np.convolve(noise, np.ones(17) / 17, mode='same')
creak = np.sin(2*np.pi*(83*t+28*t*t)) * (.2+.8*np.sin(2*np.pi*9*t)**8)
save('roof_creak', (grain*.6+creak*.18)*np.minimum(1,t*50)*np.exp(-t*4))
crack = grain*np.exp(-t*6)
for onset in (.01, .07, .13, .25, .38):
    age=np.maximum(0,t-onset)
    crack += (t>=onset)*np.exp(-age*50)*(noise*.18+np.sin(2*np.pi*140*age)*.1)
save('roof_break', crack)
melt=grain*.3*np.exp(-t*4)
for onset, frequency in [(.03,1800),(.12,2500),(.29,1150),(.44,2100)]:
    age=np.maximum(0,t-onset)
    melt += (t>=onset)*np.exp(-age*35)*np.sin(2*np.pi*(frequency*age-650*age*age))*.12
save('roof_melt', melt)
