"""Taut woody fibers, a snapping vine and a guardian's dry branch swing."""
import numpy as np
from synth import RATE, clock, noise, band, save

t = clock(.64)
pitch = 85 + 160*(t/.64)**2
phase = 2*np.pi*np.cumsum(pitch)/RATE
strain = (np.sin(phase)+.25*np.sin(phase*3))*.3
fibers = band(noise(len(t), 1103), 600, 2600)*(.3+.7*np.sin(2*np.pi*19*t)**8)
save('root_coil', (strain+fibers)*np.sin(np.pi*t/.64)**.8, .36)
t = clock(.27)
whip = band(noise(len(t), 3101), 800, 8200)*np.exp(-((t-.055)/.04)**2)
snap = np.sin(2*np.pi*190*t)*np.exp(-t*28)
save('root_lash', whip + snap*.65, .48)
t = clock(.36)
rustle = band(noise(len(t), 1251), 1100, 6500)
creak = np.sin(2*np.pi*(95*t+90*t*t))*.22
save('bramble_rustle', (rustle+creak)*np.sin(np.pi*t/.36)**1.4, .34)
t = clock(.23)
body = (np.sin(2*np.pi*135*t)+.45*np.sin(2*np.pi*337*t))*np.exp(-t*25)
crack = band(noise(len(t), 731), 700, 7000)*np.exp(-t*42)
save('bramble_strike', body+crack*.9, .43)
