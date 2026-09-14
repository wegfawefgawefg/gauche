"""Offline cage rattle, blunt shield impact and glass/metal failure."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.24)
x = band(noise(len(t), 534), 450, 2500)*np.exp(-t*20)
x += .35*np.sin(2*np.pi*760*t)*np.exp(-t*28)
x += .16*np.sin(2*np.pi*1790*t)*np.exp(-t*18)
save('lantern_raise', x, .24)
t = clock(.38)
x = .7*np.sin(2*np.pi*(180*t-65*t*t))*np.exp(-t*29)
x += band(noise(len(t), 537), 180, 2900)*np.exp(-t*43)
for freq, gain in [(618, .3), (1517, .12), (2493, .07)]:
    x += gain*np.sin(2*np.pi*freq*t)*np.exp(-t*14)
save('lantern_block', x, .50)
t = clock(.60)
x = band(noise(len(t), 941), 750, 7500)*np.exp(-t*28)
for i, start in enumerate([.035, .09, .17, .27]):
    age = np.maximum(0, t-start)
    x += (t >= start)*(.3-i*.05)*np.sin(2*np.pi*(2200+631*i)*age)*np.exp(-age*38)
x += .28*np.sin(2*np.pi*360*t)*np.exp(-t*18)
save('lantern_break', x, .49)
