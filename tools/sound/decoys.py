"""Offline straw stake placement and tearing cloth; no electronic ward beeps."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.40)
x = .45*np.sin(2*np.pi*135*t)*np.exp(-t*28)
x += band(noise(len(t), 210), 400, 4700)*np.exp(-t*24)
age = np.maximum(0, t-.09)
x += (t >= .09)*.4*band(noise(len(t), 431), 900, 6200)*np.exp(-age*14)
save('scarecrow_plant', x, .34)
t = clock(.53)
x = band(noise(len(t), 702), 340, 5100)*np.exp(-t*9)*(.35+.65*np.sin(2*np.pi*47*t)**2)
x += .4*np.sin(2*np.pi*(150*t-35*t*t))*np.exp(-t*30)
save('scarecrow_tear', x, .43)

t = clock(.28)
x = band(noise(len(t), 225), 180, 3400)*np.exp(-t*21)
x += .35*np.sin(2*np.pi*118*t)*np.exp(-t*20)
save('decoy_plant', x, .30)
t = clock(.23)
x = .48*np.sin(2*np.pi*(120*t-60*t*t))*np.exp(-t*32)
x += band(noise(len(t), 666), 350, 4400)*np.exp(-t*23)
save('decoy_hit', x, .34)
t = clock(.55)
x = band(noise(len(t), 593), 700, 6200)*np.exp(-t*10)*(.45+.55*np.sin(2*np.pi*32*t)**2)
x += .35*np.sin(2*np.pi*95*t)*np.exp(-t*25)
save('decoy_tear', x, .44)
