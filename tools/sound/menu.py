"""Dry wooden UI ticks with short pitched confirmation tails; offline assets."""
import numpy as np
from synth import clock, noise, band, save

for name, length, pitch, direction, seed in [
    ("move", .055, 720, -.2, 1801), ("activate", .14, 570, .7, 1803),
    ("back", .12, 530, -.6, 1805), ("change", .065, 820, .15, 1807)]:
    t = clock(length)
    tick = band(noise(len(t), seed), 800, 6800) * np.exp(-t*145)
    body = np.sin(2*np.pi*pitch*(t + direction*t*t/length)) * np.exp(-t*65)
    overtone = np.sin(2*np.pi*pitch*2.37*t) * np.exp(-t*95)
    save("menu_"+name, tick*.6 + body*.5 + overtone*.12, .23)
