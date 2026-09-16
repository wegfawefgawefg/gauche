"""Offline dry axe thunk, stressed timber warning, heavy leafy fall."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
RATE=44100
ROOT=Path(__file__).resolve().parents[2]/'assets'/'sounds'
rng=np.random.default_rng(731)
def save(name,a):
    a*=.32/max(.01,float(np.max(np.abs(a))))
    with tempfile.TemporaryDirectory() as d:
        path=Path(d)/'sound.wav'
        with wave.open(str(path),'wb') as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(RATE)
            f.writeframes((a*32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg','-v','error','-y','-i',str(path),'-c:a','libvorbis','-q:a','4',str(ROOT/(name+'.ogg'))],check=True)
t=np.arange(int(RATE*.35))/RATE;noise=rng.uniform(-1,1,len(t))
a=(np.sin(2*np.pi*147*t)*.45+np.sin(2*np.pi*287*t)*.12+noise*.2)*np.exp(-t*23)
save('tree_chop',a)
t=np.arange(int(RATE*1.1))/RATE;noise=rng.uniform(-1,1,len(t))
a=np.sin(2*np.pi*(92*t+21*t*t))*.1*np.sin(np.pi*t/1.1)**2
for start in (.02,.11,.27,.46,.61,.79):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*35)*(noise*.25+np.sin(age*2*np.pi*340)*.18)
save('tree_creak',a)
t=np.arange(int(RATE*1.4))/RATE;noise=rng.uniform(-1,1,len(t));low=np.convolve(noise,np.ones(55)/55,mode='same')
a=low*np.exp(-t*3)*3+np.sin(2*np.pi*(61*t-9*t*t))*.33*np.exp(-t*9)
a+=noise*.11*np.exp(-t*2)*(1-np.exp(-t*30))
for start in (.0,.035,.12,.21,.39,.64):
    age=np.maximum(0,t-start);a+=(t>=start)*np.exp(-age*45)*noise*.23
save('tree_fall',a)
