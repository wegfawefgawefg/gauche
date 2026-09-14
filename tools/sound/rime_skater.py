"""Blade scraping, ice impact, coat collapse, and dry grit without electronic beeps."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.42)
env = (1 - np.exp(-t * 38)) * np.exp(-t * 6)
scrape = band(noise(len(t), 2111), 1600, 6700) * env
scrape += .16 * np.sin(2 * np.pi * (1200 * t + 210 * t*t)) * env
save('skater_push', scrape, .28)
t = clock(.28)
scrape = band(noise(len(t), 2137), 600, 5800) * np.exp(-t * 18)
scrape *= .65 + .35 * np.sin(2 * np.pi * 65 * t) ** 2
save('skater_brake', scrape, .27)
t = clock(.20)
hit = band(noise(len(t), 2141), 170, 4200) * np.exp(-t * 38)
hit += .28 * np.sin(2 * np.pi * 510 * t) * np.exp(-t * 26)
save('skater_hit', hit, .36)
t = clock(.40)
fall = band(noise(len(t), 2153), 160, 2100) * np.exp(-t * 12)
for at, frequency in [(.02, 1710), (.08, 2240), (.15, 1190)]:
    age = np.maximum(0, t - at)
    fall += .17 * np.sin(2 * np.pi * frequency * age) * np.exp(-age * 34) * (t >= at)
save('skater_break', fall, .34)
t = clock(.28)
grit = band(noise(len(t), 2161), 1400, 7200)
grit *= (1 - np.exp(-t * 90)) * np.exp(-t * 17)
grit *= .3 + .7 * np.maximum(0, noise(len(t), 2179))
save('grit_scatter', grit, .24)
t = clock(.18)
save('grit_empty', band(noise(len(t), 2203), 350, 3000) * np.exp(-t * 24), .12)
t = clock(.24)
env = np.sin(np.pi * t / .24) ** 2
save('ice_slip', band(noise(len(t), 2207), 800, 4800) * env, .17)
