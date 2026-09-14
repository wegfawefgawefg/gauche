"""Three small moist crunches rather than the bandage's fabric rip."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.66)
crunch = band(noise(len(t), 602), 650, 6500)
wet = band(noise(len(t), 604), 120, 850)
envelope = sum(np.where(t >= start, np.exp(-np.maximum(0, t-start)*34), 0)
               for start in (.02, .22, .43))
save("meat_munch", (crunch*.4 + wet)*envelope, .42)
t = clock(.09)
peck = np.sin(2*np.pi*750*t)*np.exp(-t*80)
save("chicken_peck", peck + band(noise(len(t), 615), 600, 4200)*np.exp(-t*55), .32)
