"""Offline ice stress, heavy fracture and dripping runoff. Seeded synthesis."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
RATE=44100
ROOT=Path(__file__).resolve().parents[2]/'assets'/'sounds'
rng=np.random.default_rng(643)
def save(name,a):
    a*=.3/max(.01,float(np.max(np.abs(a))))
    with tempfile.TemporaryDirectory() as d:
        path=Path(d)/'sound.wav'
        with wave.open(str(path),'wb') as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(RATE)
            f.writeframes((a*32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg','-v','error','-y','-i',str(path),'-c:a','libvorbis','-q:a','4',str(ROOT/(name+'.ogg'))],check=True)
t=np.arange(int(RATE*1.1))/RATE
noise=rng.uniform(-1,1,len(t));low=np.convolve(noise,np.ones(45)/45,mode='same')
a=np.sin(2*np.pi*(140*t-27*t*t))*.13*np.exp(-t*4)
for start in (.02,.13,.3,.5):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*35)*(noise*.15+np.sin(age*2*np.pi*950)*.04)
save('pillar_crack',a)
a=low*np.exp(-t*5)*2+np.sin(2*np.pi*(72*t-20*t*t))*.17*np.exp(-t*9)
for start in (.0,.035,.12,.23,.34,.55):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*25)*(noise*.32+np.sin(age*2*np.pi*2100)*.03)
save('pillar_fall',a)
a=low*.3*np.exp(-t*3)
for start,freq in ((.0,1850),(.16,1450),(.33,2240),(.6,1700),(.85,2500)):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*38)*np.sin(2*np.pi*(freq*age-1300*age*age))*.13
save('pillar_melt',a)
