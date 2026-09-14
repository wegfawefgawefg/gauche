"""Offline tools, bowstring, powder report and mechanical handling."""
import numpy as np
from synth import RATE, clock, noise, band, save
from forest_enemies import impact


def swish(name, duration, seed, low, peak):
    t = clock(duration)
    save(name, band(noise(len(t), seed), low, 6500)*np.sin(np.pi*t/duration)**3, peak)


if __name__ == "__main__":
    swish("rock_throw", .18, 501, 900, .25)
    impact("rock_impact", .20, 410, 612)
    swish("axe_swing", .28, 633, 500, .37)
    swish("spear_thrust", .20, 511, 1200, .30)
    swish("maul_swing", .38, 642, 180, .40)
    swish("rake_sweep", .27, 843, 1900, .26)
    swish("knife_stab", .12, 71, 1400, .25)
    t = clock(.28)
    pitch = 310 + 220*np.exp(-t*25)
    twang = np.sin(2*np.pi*np.cumsum(pitch)/RATE)*np.exp(-t*24)
    save("crossbow_shot", twang + .5*band(noise(len(t), 183), 1200, 8000)*np.exp(-t*65), .42)
    t = clock(.52)
    powder = band(noise(len(t), 623), 80, 9000)*np.exp(-t*16)
    low = np.sin(2*np.pi*(70*t + 5*(1-np.exp(-t*15))))*np.exp(-t*13)
    save("blunder_shot", powder + .5*low, .70)
    t = clock(.30)
    clicks = np.zeros(len(t))
    for delay, hz in [(0, 750), (.08, 1100), (.18, 630)]:
        age = np.maximum(t-delay, 0)
        clicks += np.sin(2*np.pi*hz*age)*np.exp(-age*90)*(t>=delay)
    save("weapon_reload", clicks, .29)
    impact("weapon_empty", .09, 780, 52)
