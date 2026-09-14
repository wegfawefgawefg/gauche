"""Two compact snow compressions and two restrained ice boot scrapes."""
import numpy as np
from synth import clock, noise, band, save

for variant in range(2):
    t = clock(.22 + variant * .025)
    envelope = (1 - np.exp(-t * 180)) * np.exp(-t * 22)
    crunch = band(noise(len(t), 2011 + variant), 320, 6200)
    grains = np.maximum(0, noise(len(t), 2027 + variant)) ** 4
    sound = crunch * envelope * (.65 + grains)
    sound += .24 * band(noise(len(t), 2039 + variant), 80, 360) * np.exp(-t * 32)
    save(f"snow_step{variant + 1}", sound, .19)
    t = clock(.18 + variant * .018)
    scrape = band(noise(len(t), 2053 + variant), 1100, 5100)
    scrape *= (1 - np.exp(-t * 220)) * np.exp(-t * 30)
    tap = .25 * np.sin(2 * np.pi * (210 + variant * 31) * t) * np.exp(-t * 70)
    save(f"ice_step{variant + 1}", scrape + tap, .17)
