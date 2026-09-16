"""Seeded rock pressure, steam jet, molten spatter and condensation tails."""
from pathlib import Path
import subprocess
import tempfile
import wave
import numpy as np
ROOT=Path(__file__).resolve().parents[2]/"assets"/"sounds"
RATE=44100
rng=np.random.default_rng(1793)

def save(name,a):
    a*=.32/max(.01,float(np.max(np.abs(a))))
    with tempfile.TemporaryDirectory() as folder:
        path=Path(folder)/"sound.wav"
        with wave.open(str(path),"wb") as f:
            f.setnchannels(1);f.setsampwidth(2);f.setframerate(RATE)
            f.writeframes((a*32767).astype("<i2").tobytes())
        subprocess.run(["ffmpeg","-v","error","-y","-i",str(path),"-c:a","libvorbis","-q:a","4",str(ROOT/(name+".ogg"))],check=True)

for name,span in [("pressure",1.0),("steam",1.5),("lava",1.5),("cool",.65)]:
    t=np.arange(int(span*RATE))/RATE;noise=rng.uniform(-1,1,len(t))
    low=np.convolve(noise,np.ones(151)/151,mode="same")
    mid=np.convolve(noise,np.ones(9)/9,mode="same")
    if name=="pressure":
        a=(low*1.8+mid*.09)*np.sin(np.pi*t/span)**.6
        for start in (.08,.3,.46,.59,.71,.81):
            age=np.maximum(0,t-start)
            a+=(t>=start)*np.exp(-age*40)*(noise*.035+np.sin(age*2*np.pi*113)*.11)
    elif name=="steam":
        env=np.minimum(1,t*30)*np.minimum(1,(span-t)*7)
        a=(mid*.6+noise*.08+low*.5)*env*(.8+.2*np.sin(t*27))
    elif name=="lava":
        a=low*.8*np.sin(np.pi*t/span)**.5
        for start in np.arange(.02,1.35,.11):
            age=np.maximum(0,t-start)
            a+=(t>=start)*np.exp(-age*26)*(np.sin(2*np.pi*(174*age-100*age*age))*.12+mid*.1)
    else:
        a=(mid*.36+noise*.09)*(1-np.exp(-t*170))*np.exp(-t*7)
    save("fissure_"+name,a)
