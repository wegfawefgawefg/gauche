"""A foil snap and a short fizz, synthesized offline without healing-bandage sounds."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.16)
foil = band(noise(len(t), 2707), 1800, 7600)*np.exp(-t*48)
foil += .15*np.sin(2*np.pi*380*t)*np.exp(-t*65)
save('heat_crack', foil, .28)
t = clock(.76)
fizz = band(noise(len(t), 2711), 700, 3800)*(1-np.exp(-t*34))*np.exp(-t*5)
fizz *= .75 + .25*np.sin(2*np.pi*47*t)**2
save('heat_hiss', fizz, .16)
