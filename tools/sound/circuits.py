"""Unspooling, metal seating, electrical grounding and a cooling stake. Offline only."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.4); n = noise(len(t), 1331)
rasp = band(n, 1000, 5800) * np.exp(-t*9) * (.3+.7*np.sin(t*72)**2)
save('wire_lay', rasp + .13*np.sin(2*np.pi*680*t)*np.exp(-t*32), .28)
t = clock(.3)
save('wire_empty', band(noise(len(t), 1333), 400, 3100)*np.exp(-t*21) +
     .2*np.sin(2*np.pi*230*t)*np.exp(-t*17), .22)
t = clock(.24)
save('wire_cut', band(noise(len(t), 1337), 1400, 8000)*np.exp(-t*47) +
     .14*np.sin(2*np.pi*1340*t)*np.exp(-t*30), .31)
t = clock(.32)
strike = sum(np.sin(2*np.pi*f*t)*np.exp(-t*decay)*gain for f, decay, gain in
             [(290, 25, .5), (1130, 17, .22), (2317, 40, .1)])
save('spike_plant', strike + band(noise(len(t), 1341), 600, 4500)*np.exp(-t*35), .36)
t = clock(.7)
buzz = np.sin(2*np.pi*(150*t+540*t*t))*np.sin(2*np.pi*71*t)
save('spike_sink', buzz*np.exp(-t*6)*.5 + band(noise(len(t), 1343), 1200, 8400)*
     np.exp(-t*8)*(1-np.exp(-t*230)), .55)
t = clock(.6)
ping = (np.sin(2*np.pi*1630*t)+.24*np.sin(2*np.pi*3017*t))*np.exp(-t*15)
save('spike_cool', ping*.16+band(noise(len(t), 1345), 2700, 9200)*np.exp(-t*11), .23)
t = clock(.33)
save('spike_lift', band(noise(len(t), 1347), 500, 3400)*np.sin(np.pi*t/.33)**2 +
     .12*np.sin(2*np.pi*910*t)*np.exp(-t*12), .26)
t = clock(.45)
save('spike_break', band(noise(len(t), 1349), 350, 7600)*np.exp(-t*19) +
     sum(.14*np.sin(2*np.pi*f*t)*np.exp(-t*d) for f, d in [(530, 16), (1817, 9)]), .39)
