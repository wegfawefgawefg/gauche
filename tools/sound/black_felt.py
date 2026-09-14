"""Dry cloth swishes, a short tear/burn, and wood with loose glass; offline only."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.34)
a = band(noise(len(t), 4601), 220, 2600)
save('felt_cover', a * np.sin(np.pi*t/t[-1])**2 * (1+.3*np.sin(2*np.pi*23*t)), .23)
t = clock(.27)
a = band(noise(len(t), 4603), 600, 4600)
save('felt_tear', a * np.exp(-t*12) * (.4+.6*np.sin(2*np.pi*37*t)**2), .28)
t = clock(.48)
a = band(noise(len(t), 4605), 1100, 7900)
save('felt_burn', a * (1-np.exp(-t*70))*np.exp(-t*10)*(.7+.3*np.sin(2*np.pi*61*t)), .26)
t = clock(.55)
a = band(noise(len(t), 4607), 140, 1800)*np.exp(-t*32)
a += .23*np.sin(2*np.pi*147*t)*np.exp(-t*26)
for i, f in enumerate([1871, 3197, 4133]):
    s = np.maximum(0, t-.027-i*.05)
    a += .16*(t >= .027+i*.05)*np.sin(2*np.pi*f*s)*np.exp(-s*25)
save('lens_case_break', a, .38)
