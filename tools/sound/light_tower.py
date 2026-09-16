"""Seeded offline steel buckling and a heavy truss/lamp collapse."""
from pathlib import Path
import subprocess,tempfile,wave
import numpy as np
RATE=44100
ROOT=Path(__file__).resolve().parents[2]/'assets'/'sounds'
rng=np.random.default_rng(842)
def save(name,a):
    a*=.32/max(.01,float(np.max(np.abs(a))))
    with tempfile.TemporaryDirectory() as d:
        path=Path(d)/'sound.wav'
        with wave.open(str(path),'wb') as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(RATE);f.writeframes((a*32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg','-v','error','-y','-i',str(path),'-c:a','libvorbis','-q:a','4',str(ROOT/(name+'.ogg'))],check=True)
t=np.arange(int(RATE*1.2))/RATE;noise=rng.uniform(-1,1,len(t))
a=np.zeros(len(t))
for freq,amp in ((79,.3),(137,.13),(351,.08),(727,.03)):
    a+=np.sin(2*np.pi*(freq*t-12*t*t)+np.sin(t*32)*.3)*amp*np.sin(np.pi*t/1.2)**2
for start in (.01,.18,.43,.7):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*45)*noise*.15
save('tower_groan',a)
t=np.arange(int(RATE*1.7))/RATE;noise=rng.uniform(-1,1,len(t));low=np.convolve(noise,np.ones(65)/65,mode='same')
a=low*4*np.exp(-t*4)
for start in (.0,.027,.08,.19,.36,.61):
    age=np.maximum(0,t-start);env=(t>=start)*np.exp(-age*14)
    a+=env*(noise*.16+np.sin(age*2*np.pi*143)*.1+np.sin(age*2*np.pi*479)*.06+np.sin(age*2*np.pi*1831)*.025)
save('tower_crash',a)
