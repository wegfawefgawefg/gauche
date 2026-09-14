"""Mansion warning bell, iron gate, shifting grave and release chord."""
import numpy as np
from synth import band, clock, noise, save


def bell(duration, fundamental):
    t = clock(duration)
    signal = np.zeros(len(t))
    for i, ratio in enumerate([1, 2.71, 4.08, 5.42]):
        signal += np.sin(t * 2 * np.pi * fundamental * ratio) * np.exp(-t * (2.5 + i * 1.4)) / (i + 1)
    return signal


if __name__ == "__main__":
    save("encounter_bell", bell(1.1, 190), .56)
    t = clock(.7)
    gate = band(noise(len(t), 916), 350, 9000) * np.exp(-t * 17)
    for frequency in [92, 177, 410]:
        gate += .4 * np.sin(t * 2 * np.pi * frequency) * np.exp(-t * 9)
    save("gate_close", gate, .70)
    t = clock(.48)
    earth = band(noise(len(t), 497), 80, 1400) * np.sin(np.pi * t / .48) ** 2
    earth += .2 * np.sin(2 * np.pi * (70 * t + 45 * t * t)) * np.exp(-t * 6)
    save("grave_rise", earth, .45)
    save("encounter_clear", bell(1.8, 260) + bell(1.8, 325) * .6 + bell(1.8, 390) * .4, .58)
