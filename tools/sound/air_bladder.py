"""Rubbery creaks, escaping air and soft water displacement, synthesized offline."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.36)
env = (1-np.exp(-t*65))*np.exp(-t*11)
air = band(noise(len(t), 2503), 450, 3600)*env
air += .16*np.sin(2*np.pi*(160*t+210*t*t))*env
save('air_inflate', air, .28)
t = clock(.25)
plop = .4*np.sin(2*np.pi*(350*t-300*t*t))*np.exp(-t*35)
plop += band(noise(len(t), 2521), 500, 3000)*np.exp(-t*22)
save('air_float', plop, .18)
t = clock(.42)
hiss = band(noise(len(t), 2531), 1300, 5200)*(1-np.exp(-t*60))*np.exp(-t*10)
save('air_deflate', hiss, .13)
t = clock(.13)
ripple = band(noise(len(t), 2539), 700, 2700)*np.sin(np.pi*t/.13)**2
ripple += .13*np.sin(2*np.pi*(650*t+1400*t*t))*np.exp(-t*46)
save('air_paddle', ripple, .075)
t = clock(.16)
fold = band(noise(len(t), 2543), 200, 1700)*np.exp(-t*28)
save('air_empty', fold, .13)
