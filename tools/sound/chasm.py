"""Descending air whistle with small stone ticks, no bottom impact. Offline and seeded."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
rate=44100
t=np.arange(int(rate*.9))/rate
rng=np.random.default_rng(7302)
noise=rng.uniform(-1,1,len(t))
air=np.convolve(noise,np.ones(31)/31,mode='same')
envelope=np.minimum(1,t/.035)*np.exp(-t*4)*(1-t/.9)
frequency=960*np.exp(-t*2.8)+18*np.sin(t*2*np.pi*7)
phase=2*np.pi*np.cumsum(frequency)/rate
signal=(air*.35+np.sin(phase)*.24+np.sin(phase*2)*.018)*envelope
for start in (.01,.08,.17):
    dt=np.maximum(0,t-start)
    signal+=(t>=start)*np.exp(-dt*75)*np.sin(2*np.pi*(970+start*300)*dt)*.045
signal*=.18/max(.01,float(np.max(np.abs(signal))))
output=Path(__file__).resolve().parents[2]/'assets/sounds/chasm_fall.ogg'
with tempfile.TemporaryDirectory() as temp:
    wav=Path(temp)/'fall.wav'
    with wave.open(str(wav),'wb') as f:
        f.setnchannels(1);f.setsampwidth(2);f.setframerate(rate)
        f.writeframes((signal*32767).astype('<i2').tobytes())
    subprocess.run(['ffmpeg','-v','error','-y','-i',str(wav),'-c:a','libvorbis','-q:a','4',str(output)],check=True)
