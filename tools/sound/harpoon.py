"""Offline spring launcher, steel barb, rope ratchet, cut and reloading mechanism."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.34)
save('harpoon_fire',band(noise(len(t),1511),250,6400)*np.exp(-t*24)+
     .5*np.sin(2*np.pi*(210*t+25*(1-np.exp(-t*22))))*np.exp(-t*20),.48)
t=clock(.18)
save('harpoon_impact',band(noise(len(t),1513),300,4700)*np.exp(-t*38)+
     .4*np.sin(2*np.pi*130*t)*np.exp(-t*30),.37)
t=clock(.24)
save('harpoon_latch',band(noise(len(t),1517),1300,6200)*np.exp(-t*52)+
     .2*np.sin(2*np.pi*1080*t)*np.exp(-t*21),.25)
t=clock(.14)
pulses=sum(np.exp(-np.maximum(t-s,0)*100)*(t>=s) for s in [0,.035,.07])
save('harpoon_reel',band(noise(len(t),1523),900,3700)*pulses,.16)
t=clock(.23)
save('harpoon_release',band(noise(len(t),1531),700,5900)*np.exp(-t*28)+
     .15*np.sin(2*np.pi*(800*t-800*t*t))*np.exp(-t*28),.22)
t=clock(.64)
clacks=sum(np.exp(-np.maximum(t-s,0)*60)*(t>=s) for s in [.02,.36,.51])
pull=np.sin(np.pi*np.clip((t-.05)/.31,0,1))**2
save('harpoon_reload',band(noise(len(t),1543),450,4800)*(clacks+.16*pull),.29)
