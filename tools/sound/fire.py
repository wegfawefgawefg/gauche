"""Generate ember crunch, extinguishing hiss, and a short synthesized 'AAH'."""
import numpy as np
from synth import RATE, band, clock, noise, save


def panic():
    t = clock(0.52)
    pitch = 210 + 70 * np.sin(np.pi * t / 0.52) + 9 * np.sin(2 * np.pi * 27 * t)
    phase = np.cumsum(pitch) / RATE
    throat = np.zeros(len(t))
    # VOWEL: Harmonics cluster around an open mouth's three broad resonances.
    for harmonic in range(1, 25):
        frequency = harmonic * 255
        formants = sum(gain * np.exp(-((frequency - center) / width) ** 2)
                       for center, width, gain in [(790, 220, 1), (1200, 320, .7),
                                                    (2800, 480, .18)])
        throat += (0.06 / harmonic + formants / harmonic ** .55) * np.sin(
            2 * np.pi * harmonic * phase)
    breath = band(noise(len(t), 6723), 500, 4400)
    envelope = np.sin(np.pi * t / .52) ** .65
    return (throat + .12 * breath) * envelope * (1 + .12 * np.sin(2 * np.pi * 39 * t))


def trample():
    t = clock(.42)
    crunch = band(noise(len(t), 8812), 650, 7500)
    envelope = np.exp(-t * 13) * (.35 + .65 * np.sin(t * 180) ** 8)
    hiss = band(noise(len(t), 5290), 1700, 9500) * np.exp(-t * 9)
    thud = np.sin(2 * np.pi * (95 * t - 45 * t * t)) * np.exp(-t * 23)
    return crunch * envelope + .27 * hiss + .22 * thud


def extinguish():
    t = clock(.85)
    hiss = band(noise(len(t), 4511), 1500, 9500)
    envelope = (1 - np.exp(-t * 45)) * np.exp(-t * 5)
    return hiss * envelope * (.65 + .35 * np.sin(2 * np.pi * t * 17) ** 2)


if __name__ == "__main__":
    save("fire_panic", panic(), .52)
    save("fire_trample", trample(), .60)
    save("fire_out", extinguish(), .45)
