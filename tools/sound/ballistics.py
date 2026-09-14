"""Offline gun reports and mechanical reloads; deliberately distinct from spell sounds."""
import numpy as np
from synth import clock, noise, band, save

for name, length, pitch, decay, seed in [
    ('pistol_shot', .26, 135, 24, 941), ('musket_shot', .56, 76, 10, 943),
    ('shotgun_shot', .40, 95, 15, 945), ('smg_shot', .13, 185, 42, 947)]:
    t = clock(length)
    crack = band(noise(len(t), seed), 500, 11000) * np.exp(-t * decay * 1.7)
    body = np.sin(2*np.pi*(pitch*t - pitch*.55*t*t)) * np.exp(-t*decay)
    tail = band(noise(len(t), seed+1), 100, 2200) * np.exp(-t*decay*.6)
    save(name, crack + body*.55 + tail*.35, .62)
t = clock(.46)
save('rocket_launch', band(noise(len(t), 949), 180, 6500) *
     (1-np.exp(-t*120))*np.exp(-t*7) + np.sin(2*np.pi*90*t)*np.exp(-t*24)*.4, .58)

for name, length, beats, seed in [
    ('pistol_reload', .55, [(0, .7), (.20, .55), (.40, 1)], 951),
    ('shell_reload', .75, [(0, .6), (.22, .7), (.45, .7), (.62, 1)], 953),
    ('powder_reload', 1.0, [(0, .35), (.15, .4), (.55, .65), (.82, 1)], 955),
    ('crossbow_reload', .85, [(0, .45), (.36, .4), (.70, 1)], 957),
    ('rocket_reload', .80, [(0, .7), (.25, .65), (.58, 1)], 959)]:
    t = clock(length); result = np.zeros(len(t))
    texture = band(noise(len(t), seed), 800, 7500)
    for when, strength in beats:
        age = np.maximum(0, t-when)
        envelope = np.where(t >= when, np.exp(-age*65), 0)
        result += strength*envelope*(texture + .3*np.sin(2*np.pi*1700*age))
    if name == 'powder_reload':
        result += band(noise(len(t), seed+1), 1000, 4500)*np.sin(np.pi*t/length)**2*.14
    if name == 'crossbow_reload':
        result += np.sin(2*np.pi*(95*t+220*t*t))*np.sin(np.pi*t/length)**2*.15
    save(name, result, .36)
