"""Dry bow tension/twang, arrow splinter and thrown bomb fuse cues."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.24)
creak = band(noise(len(t), 705), 450, 4200)
save('bow_draw', creak*.25*np.sin(np.pi*t/.24) + np.sin(2*np.pi*(160*t+250*t*t))*.25*np.sin(np.pi*t/.24), .34)
t = clock(.26)
save('bow_release', (np.sin(2*np.pi*290*t)+.45*np.sin(2*np.pi*580*t))*np.exp(-t*25) + band(noise(len(t), 707), 1500, 6500)*np.exp(-t*45)*.4, .44)
t = clock(.18)
save('arrow_impact', band(noise(len(t), 708), 750, 8000)*np.exp(-t*36) + .25*np.sin(2*np.pi*450*t)*np.exp(-t*45), .38)
t = clock(.18)
save('bomb_throw', band(noise(len(t), 713), 300, 3000)*np.sin(np.pi*t/.18), .27)
t = clock(.18)
save('bomb_land', np.sin(2*np.pi*130*t)*np.exp(-t*32) + band(noise(len(t), 714), 600, 6500)*np.exp(-t*60)*.35, .34)
t = clock(.49)
save('bomb_fuse', band(noise(len(t), 716), 2400, 10500)*(.6+.4*np.maximum(0, np.sin(2*np.pi*31*t))**8)*np.sin(np.pi*t/.49)**.25, .21)
