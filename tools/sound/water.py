"""Low splash/plop footsteps and a short extinguishing hiss."""
import numpy as np
from synth import clock, noise, band, save

for variant in range(2):
    t = clock(.27)
    phase = 2*np.pi*(390*t - 260*t*t)
    plop = np.sin(phase)*np.exp(-t*24)
    splash = band(noise(len(t), 381+variant), 550, 6200)*np.exp(-t*17)
    save(f"water_step{variant+1}", .5*plop + splash*.65, .38)
t = clock(.65)
hiss = band(noise(len(t), 393), 1800, 10500)
save("water_douse", hiss*np.minimum(1, t*60)*np.exp(-t*6), .36)
