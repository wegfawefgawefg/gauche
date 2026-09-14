"""Short exertion and air movement, with no in-game synthesizer."""
import numpy as np
from scipy import signal
from synth import clock, noise, band, save, RATE

t = clock(.13)
source = signal.sawtooth(2*np.pi*(110*t - 80*t*t), .4)
grunt = band(source, 320, 850)*.5 + band(source, 1100, 1700)*.18
breath = band(noise(len(t), 808), 600, 5000)*.2
save('fist_windup', (grunt + breath)*np.sin(np.pi*t/.13)**.6, .30)
for name, length, low, high, seed in [('stick_windup', .16, 280, 4500, 810), ('pickaxe_windup', .23, 170, 3000, 812)]:
    t = clock(length)
    air = band(noise(len(t), seed), low, high)
    save(name, air*np.sin(np.pi*t/length)**1.5, .34)
