"""Offline animal voices, shell impacts and living wood; no runtime synthesizer."""
import numpy as np
from synth import RATE, clock, noise, band, save


def voice(name, seconds, low, high, pulse, seed, peak=.44):
    t = clock(seconds)
    pitch = low + (high - low) * np.sin(np.pi * t / seconds) + 4 * np.sin(2*np.pi*7*t)
    phase = 2*np.pi*np.cumsum(pitch) / RATE
    tone = np.sin(phase) + .35*np.sin(2*phase) + .18*np.sin(3*phase)
    breath = band(noise(len(t), seed), 250, 2200)
    envelope = np.sin(np.pi*t/seconds)**.6
    envelope *= .65 + .35*np.sin(2*np.pi*pulse*t)**2
    save(name, (.8*tone + .4*breath)*envelope, peak)


def impact(name, seconds, frequency, seed, woody=False):
    t = clock(seconds)
    thud = np.sin(2*np.pi*(frequency*t + 20*t*np.exp(-t*40))) * np.exp(-t*24)
    grit = band(noise(len(t), seed), 300 if woody else 900, 6500) * np.exp(-t*40)
    save(name, thud + grit*.7, .40)


if __name__ == "__main__":
    voice("boar_snort", .46, 65, 125, 13, 312)
    impact("boar_hit", .23, 85, 143, True)
    impact("shell_knock", .20, 830, 222)
    t = clock(.26)
    save("shell_scrape", band(noise(len(t), 92), 500, 4500)*np.sin(np.pi*t/.26)**2, .28)
    voice("toad_croak", .58, 105, 160, 23, 142)
    t = clock(.36)
    save("toad_puff", band(noise(len(t), 97), 180, 1500)*np.sin(np.pi*t/.36)**1.4, .36)
    t = clock(.35)
    save("moth_flutter", band(noise(len(t), 512), 1100, 6800)*np.sin(2*np.pi*19*t)**6*np.sin(np.pi*t/.35), .23)
    save("moth_dust", band(noise(len(t), 751), 1800, 8500)*np.sin(np.pi*t/.35)**2, .22)
    voice("mimic_wake", .43, 75, 180, 17, 325)
    impact("mimic_bite", .20, 165, 402, True)
    voice("wolf_howl", 1.20, 210, 520, 4, 92, .40)
    impact("wolf_bite", .15, 155, 92)
    voice("bear_rear", .64, 48, 95, 12, 883, .50)
    impact("bear_slam", .36, 50, 34, True)
    voice("bat_squeak", .25, 1800, 3300, 30, 65, .26)
    impact("bat_bite", .12, 650, 315)
