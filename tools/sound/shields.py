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

t = clock(.18)
x = band(noise(len(t), 692), 700, 3100)*np.sin(np.pi*t/.18)**2
x += .22*np.sin(2*np.pi*680*t)*np.exp(-t*30)
save('pan_raise', x, .25)
t = clock(.70)
x = band(noise(len(t), 536), 180, 5600)*np.exp(-t*65)
for freq, gain, decay in [(422, .70, 9), (1097, .42, 12), (1814, .20, 17), (2699, .12, 23)]:
    x += gain*np.sin(2*np.pi*freq*t)*np.exp(-t*decay)
save('pan_reflect', x, .64)
t = clock(.44)
x = band(noise(len(t), 299), 300, 4700)*np.exp(-t*27)
x += .45*np.sin(2*np.pi*(460*t-230*t*t))*np.exp(-t*12)
x += .20*np.sin(2*np.pi*1460*t)*np.exp(-t*18)
save('pan_break', x, .51)
