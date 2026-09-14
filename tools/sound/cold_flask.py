"""Offline glass, frost and small thawing-water cues."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.23)
whoosh = band(noise(len(t), 2609), 600, 3700)*np.sin(np.pi*t/.23)**2
whoosh += .22*np.sin(2*np.pi*(760*t-700*t*t))*np.exp(-t*32)
save('cold_throw', whoosh, .24)
t = clock(.48)
glass = sum(np.sin(2*np.pi*f*t)*np.exp(-t*d) for f, d in [(1700, 35), (2761, 48), (3917, 61)])*.16
frost = band(noise(len(t), 2617), 1400, 6400)*(1-np.exp(-t*110))*np.exp(-t*10)
save('cold_burst', glass + frost, .35)
t = clock(.53)
hiss = band(noise(len(t), 2621), 900, 5900)*(1-np.exp(-t*80))*np.exp(-t*9)
save('cold_quench', hiss, .20)
t = clock(.27)
drip = .4*np.sin(2*np.pi*(570*t-510*t*t))*np.exp(-t*40)
drip += band(noise(len(t), 2633), 700, 2300)*np.exp(-t*18)
save('ice_thaw', drip, .10)
