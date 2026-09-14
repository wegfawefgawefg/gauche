"""Short muted coin clinks; comfortable when several piles are collected together."""
import numpy as np
from synth import clock, save


if __name__ == "__main__":
    t = clock(.22)
    signal = np.zeros(len(t))
    for delay, frequency in [(0, 2300), (.03, 3100), (.065, 2700)]:
        age = np.maximum(0, t - delay)
        signal += (np.sin(age * 2 * np.pi * frequency) + .3 * np.sin(age * 2 * np.pi * frequency * 1.71)) * np.exp(-age * 65) * (t >= delay)
    save("coin_pickup", signal, .30)
