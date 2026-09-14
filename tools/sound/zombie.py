"""A descending comic groan followed by uneven cloth/body thumps."""
import numpy as np
from synth import band, clock, noise, save


if __name__ == "__main__":
    t = clock(1.05)
    pitch = 160 * np.exp(-t * 1.8) + 38
    phase = np.cumsum(pitch) * 2 * np.pi / 44100
    throat = np.sin(phase) + .28 * np.sin(phase * 2) + .16 * np.sin(phase * 5)
    signal = throat * np.exp(-t * 6) * np.minimum(1, t * 70)
    for i, delay in enumerate([.18, .34, .51, .72]):
        age = np.maximum(0, t - delay)
        thud = np.sin(2 * np.pi * (85 - i * 8) * age) * np.exp(-age * 26)
        cloth = band(noise(len(t), 195 + i), 400, 3500) * np.exp(-age * 35)
        signal += (.7 * thud + .25 * cloth) * (t >= delay)
    save("zombie_topple", signal, .65)
