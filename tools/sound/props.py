"""Dry foliage, snapped wood, spores and resonant ceramic fragments."""
import numpy as np
from synth import band, clock, noise, save


def crunch(seed, duration, low, high, decay):
    t = clock(duration)
    texture = band(noise(len(t), seed), low, high)
    return texture * np.exp(-t * decay) * (.15 + np.sin(t * 115 + 1) ** 8)


if __name__ == "__main__":
    save("leaves_crunch", crunch(451, .32, 1200, 8000, 9), .40)
    save("twig_snap", crunch(637, .16, 500, 6000, 28), .58)
    save("plant_cut", crunch(1261, .24, 900, 4800, 11), .38)
    t = clock(.45)
    puff = band(noise(len(t), 9981), 350, 3500) * (1 - np.exp(-t*45)) * np.exp(-t*12)
    save("spore_puff", puff, .38)
    t = clock(.38)
    wood = crunch(299, .38, 200, 5000, 16)
    wood += .4 * (np.sin(t*2*np.pi*185) + .3*np.sin(t*2*np.pi*410)) * np.exp(-t*32)
    save("wood_crack", wood, .6)
    t = clock(.7)
    pottery = crunch(751, .7, 1800, 12000, 25)
    for i, frequency in enumerate([1430, 2170, 3280, 4830]):
        offset = i * .025
        age = np.maximum(0, t-offset)
        pottery += .12 * np.sin(age*2*np.pi*frequency) * np.exp(-age*22) * (t >= offset)
    save("pot_break", pottery, .52)
