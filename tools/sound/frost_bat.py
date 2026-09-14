"""Cold breath, papery wings and brittle dispersal, synthesized offline."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.50)
rise = np.sin(np.pi * t / 1.0) ** 2
breath = band(noise(len(t), 2309), 1200, 6500) * rise
breath += .12 * np.sin(2*np.pi*(1600*t + 550*t*t)) * rise
save('frost_inhale', breath, .26)
t = clock(.32)
env = (1 - np.exp(-t*100)) * np.exp(-t*12)
save('frost_exhale', band(noise(len(t), 2311), 550, 5600) * env, .30)
t = clock(.42)
flutter = band(noise(len(t), 2333), 220, 2600)
flutter *= np.maximum(0, np.sin(2*np.pi*13*t)) ** 2 * np.exp(-t*4)
save('frost_flutter', flutter, .18)
t = clock(.34)
cry = .4*np.sin(2*np.pi*(1900*t - 1400*t*t)) * np.exp(-t*13)
cry += band(noise(len(t), 2339), 500, 4500)*np.exp(-t*16)
save('frost_death', cry, .28)
t = clock(.23)
burst = band(noise(len(t), 2341), 2200, 8200) * np.exp(-t*26)
for frequency in [3211, 4339, 5801]:
    burst += .12*np.sin(2*np.pi*frequency*t)*np.exp(-t*42)
save('frost_puff_hit', burst, .23)
t = clock(.27)
hiss = band(noise(len(t), 2347), 1700, 6000) * np.exp(-t*16)
hiss += .11*np.sin(2*np.pi*(950*t-850*t*t)) * np.exp(-t*32)
save('ice_melt', hiss, .17)
t = clock(.18)
whip = band(noise(len(t), 2351), 1500, 7500)*np.sin(np.pi*t/.18)**2
save('ice_needle_throw', whip, .22)
t = clock(.19)
chip = band(noise(len(t), 2357), 1900, 7600)*np.exp(-t*40)
chip += .18*np.sin(2*np.pi*2900*t)*np.exp(-t*45)
save('ice_needle_hit', chip, .23)
