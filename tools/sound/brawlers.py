"""Quiet guttural warning/death cues and short club/pipe swings, offline."""
from pathlib import Path
import subprocess,tempfile,wave
import numpy as np
ROOT=Path(__file__).resolve().parents[2]/'assets'/'sounds'
RATE=44100
rng=np.random.default_rng(1904)
def save(name,signal):
    signal*=.24/max(.01,float(np.max(np.abs(signal))))
    with tempfile.TemporaryDirectory() as d:
        path=Path(d)/'source.wav'
        with wave.open(str(path),'wb') as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(RATE)
            f.writeframes((signal*32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg','-v','error','-y','-i',str(path),'-c:a','libvorbis','-q:a','4',str(ROOT/(name+'.ogg'))],check=True)
for name,pitch in [('frost_goblin',145),('pipe_guard',92)]:
    for suffix,duration in [('warn',.32),('swing',.22),('death',.5)]:
        t=np.arange(int(RATE*duration))/RATE
        noise=rng.uniform(-1,1,len(t));low=np.convolve(noise,np.ones(23)/23,mode='same')
        env=np.sin(np.pi*np.minimum(1,t/duration))**1.5
        if suffix=='swing':
            a=(low*1.5+noise*.12)*env
            if name=='pipe_guard':a+=np.sin(2*np.pi*1750*t)*np.exp(-t*35)*.04
        else:
            phase=2*np.pi*pitch*(t+(.35 if suffix=='warn' else -.5)*t*t)
            a=(np.sin(phase)+np.sin(phase*2.03)*.3+np.sin(phase*3.06)*.15)*.12
            a=(a*(.6+.4*np.sin(2*np.pi*29*t)**2)+low*.6)*env
        save(name+'_'+suffix,a)
