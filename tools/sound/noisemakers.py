"""Inharmonic brass strikes, paper toss and fuse/bang; all synthesis is offline."""
import numpy as np
from synth import clock, noise, band, save

t = clock(1.2)
bell = np.zeros(len(t))
for start, strength in [(0, 1), (.19, .72)]:
    age = np.maximum(0, t-start)
    for ratio, gain, decay in [(1, .8, 4), (2.71, .32, 7), (4.09, .13, 10), (5.43, .06, 15)]:
        bell += (t >= start)*strength*gain*np.sin(2*np.pi*590*ratio*age)*np.exp(-age*decay)
save('hand_bell', bell, .42)
t = clock(.23)
save('cracker_throw', band(noise(len(t), 253), 500, 5100)*np.sin(np.pi*t/.23)**2, .26)
t = clock(.30)
fizz = band(noise(len(t), 298), 1600, 7200)
save('cracker_fuse', fizz*(.45+.55*np.sin(2*np.pi*43*t)**6)*np.sin(np.pi*t/.3)**2, .16)
t = clock(.17)
save('cracker_land', (band(noise(len(t), 864), 450, 3900)+.2*np.sin(2*np.pi*145*t))*np.exp(-t*32), .26)
t = clock(.42)
pop = band(noise(len(t), 437), 170, 7000)*np.exp(-t*48)
pop += .45*np.sin(2*np.pi*(135*t-100*t*t))*np.exp(-t*28)
# Two faint reflections make a paper crack rather than another bass explosion.
for start in [.047, .103]:
    age = np.maximum(0, t-start)
    pop += .10*(t >= start)*band(noise(len(t), round(start*10000)), 800, 4200)*np.exp(-age*28)
save('cracker_bang', pop, .63)
