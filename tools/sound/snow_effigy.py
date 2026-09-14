"""Offline settling snow, joint creaks, twig strike and hushed thawing."""
import numpy as np
from synth import clock, noise, band, save

for name,seed,duration,pitch,peak in [
    ('effigy_wake',5671,.65,270,.24),('effigy_still',5673,.16,430,.16),
    ('effigy_creak',5675,.32,180,.23),('effigy_warn',5677,.43,370,.29)]:
    t=clock(duration)
    phase=2*np.pi*np.cumsum(pitch+60*np.sin(t*12)+25*np.sin(t*71))/44100
    env=np.sin(np.pi*t/duration)**2
    a=(np.sin(phase)+.16*np.sin(phase*3))*env*.2
    a+=band(noise(len(t),seed),400,2400)*env*.5
    save(name,a,peak)
t=clock(.24)
a=band(noise(len(t),5679),180,3600)*np.exp(-t*28)
a+=.15*np.sin(2*np.pi*175*t)*np.exp(-t*36)
save('effigy_hit',a,.34)
t=clock(.7)
a=band(noise(len(t),5681),900,6500)*(1-np.exp(-t*35))*np.exp(-t*5)
save('effigy_thaw',a,.24)
t=clock(.52)
a=np.zeros_like(t)
for at,seed in [(.01,5683),(.08,5685),(.23,5687),(.35,5689)]:
    age=np.maximum(0,t-at)
    a+=band(noise(len(t),seed),180,4300)*np.exp(-age*45)*(t>=at)
save('effigy_death',a,.32)
