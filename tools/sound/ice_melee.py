"""Seeded wool-bound tool swishes, hard steel/tusk contacts and a broken haft."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
root=Path(__file__).resolve().parents[2]/'assets'/'sounds'
rate=44100
for i,name in enumerate(('ice_axe_swing','ice_axe_hit','tusk_pike_thrust','tusk_pike_hit','ice_tool_break')):
    rng=np.random.default_rng(2017+i);span=.32 if i in (0,2) else .4
    t=np.arange(int(span*rate))/rate;noise=rng.uniform(-1,1,len(t))
    low=np.convolve(noise,np.ones(27)/27,mode='same')
    mid=np.convolve(noise,np.ones(5)/5,mode='same')
    if i in (0,2):
        center=.105 if i==0 else .065;width=.045 if i==0 else .025
        a=(mid*.75+low*.8)*np.exp(-((t-center)/width)**2)
        a+=low*.2*np.exp(-t*35)
    else:
        a=(mid*.5+low*1.3)*np.exp(-t*(32 if i==1 else 48))
        for hz,volume in ((186,.24),(513,.08),(1197,.035)):
            a+=np.sin(t*2*np.pi*hz*(1 if i==1 else .78))*np.exp(-t*(34+hz/40))*volume
        a+=noise*.15*np.exp(-t*150)
        if i==4:
            for start in (.04,.075,.12,.19):
                age=np.maximum(0,t-start)
                a+=(t>=start)*noise*.13*np.exp(-age*100)
    a*=.32/max(.01,float(np.max(np.abs(a))))
    a[:88]*=np.linspace(0,1,88);a[-220:]*=np.linspace(1,0,220)
    with tempfile.TemporaryDirectory() as folder:
        path=Path(folder)/'tool.wav'
        with wave.open(str(path),'wb') as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(rate);f.writeframes((a*32767).astype('<i2').tobytes())
        subprocess.run(['ffmpeg','-v','error','-y','-i',str(path),'-c:a','libvorbis','-q:a','4',str(root/(name+'.ogg'))],check=True)
