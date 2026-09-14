"""Offline glass ringing, a lens discharge and small brass detents."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.25)
save('lens_fire', (np.sin(2*np.pi*(1450*t-1900*t*t))*.5 + np.sin(2*np.pi*2910*t)*.16 + band(noise(len(t), 4201), 1500, 6500)*.15)*np.exp(-t*19), .39)
t = clock(.65)
a = np.zeros_like(t)
for i in range(3):
    s = np.maximum(0, t - i*.15)
    a += (t >= i*.15)*np.exp(-s*50)*(np.sin(2*np.pi*(1300+i*220)*s)+.3*np.sin(2*np.pi*3700*s))
save('lens_reload', a, .26)
t = clock(.27)
save('optic_place', (np.sin(2*np.pi*930*t)*.35+np.sin(2*np.pi*2470*t)*.2+band(noise(len(t), 4203), 400, 3200)*.2)*np.exp(-t*22), .24)
t = clock(.17)
save('optic_turn', (band(noise(len(t), 4205), 650, 3000)+.25*np.sin(2*np.pi*1520*t))*(np.exp(-t*65)+.45*np.exp(-np.abs(t-.08)*100)), .21)
t = clock(.25)
save('optic_hit', (np.sin(2*np.pi*2180*t)+.35*np.sin(2*np.pi*3517*t)+.2*np.sin(2*np.pi*4641*t))*np.exp(-t*21), .25)
t = clock(.50)
a = band(noise(len(t), 4207), 1400, 8200)*np.exp(-t*24)
for i, f in enumerate([1871, 2419, 3281, 4703]):
    s = np.maximum(0, t-i*.034)
    a += .12*(t >= i*.034)*np.sin(2*np.pi*f*s)*np.exp(-s*23)
save('optic_break', a, .39)
