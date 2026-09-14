"""Offline skin-drum triplet, whiteout gust and glass/metal breakup."""
import numpy as np
from synth import clock, noise, band, save

for i,pitch in enumerate([170,192,220]):
    t=clock(.31)
    phase=2*np.pi*(pitch*t+5*(1-np.exp(-t*35)))
    a=np.sin(phase)*np.exp(-t*16)+.32*np.sin(phase*1.59)*np.exp(-t*24)
    a+=band(noise(len(t),5501+i),600,4400)*np.exp(-t*65)*.3
    save('drum_beat'+str(i+1),a,.29+i*.025)
t=clock(1.25)
env=(1-np.exp(-t*15))*np.exp(-t*2.6)
a=band(noise(len(t),5505),330,3600)*env
a+=np.sin(2*np.pi*(230*t+25*t*t))*.018*env
save('whiteout_rush',a,.27)
t=clock(.54)
a=band(noise(len(t),5507),1900,8500)*np.exp(-t*24)
for pitch in [1450,2310,3560]: a+=np.sin(2*np.pi*pitch*t)*np.exp(-t*12)*.08
save('globe_break',a,.29)
t=clock(.55)
a=band(noise(len(t),5509),220,2600)*np.exp(-t*9)
a+=np.sin(2*np.pi*(130*t-45*t*t))*np.exp(-t*10)*.3
save('drummer_death',a,.29)
t=clock(.65)
a=band(noise(len(t),5511),1100,6000)*np.exp(-t*34)*.2
for pitch in [710,1194,1906]: a+=np.sin(2*np.pi*pitch*t)*np.exp(-t*8)*.11
save('vane_break',a,.27)
