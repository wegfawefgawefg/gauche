"""Original physical / feedback timbres. No soundfont, MIDI or recorded samples."""
import numpy as np
from numba import njit
from scipy import signal

RATE = 32000


def filtered(data, low, high):
    return signal.sosfilt(signal.butter(2, [low, high], fs=RATE,
                                      btype="bandpass", output="sos"), data)


def envelope(seconds, attack, release):
    t = np.arange(round(seconds * RATE)) / RATE
    return (np.sin(np.minimum(t / attack, 1) * np.pi / 2) ** 2 *
            np.sin(np.minimum((seconds - t) / release, 1) * np.pi / 2) ** 2)


def level(data):
    # Instrument peak calibration, not per-passage compression or limiting.
    return data / max(1e-9, np.max(np.abs(data)))


@njit(cache=True)
def feedback_body(excitation, lengths, decay, damping, drive, wander):
    """Four coupled waveguides, energy-preserving scattering, damped nonlinear return.

    Excitation keeps re-entering the body. Fractional delay modulation and friction
    saturation change the resonant modes, instead of layering preset string notes.
    """
    size = int(np.max(lengths)) + 8
    buffers = np.zeros((4, size))
    output = np.zeros(len(excitation))
    low = np.zeros(4)
    reads = np.zeros(4)
    gains = 10 ** (-3 * lengths / (RATE * decay))
    for i in range(len(excitation)):
        head = i % size
        for k in range(4):
            delay = lengths[k] + wander * np.sin(i / RATE * (.41 + k * .127) + k)
            integer = int(delay)
            fraction = delay - integer
            value = ((1 - fraction) * buffers[k, (head - integer) % size] +
                     fraction * buffers[k, (head - integer - 1) % size])
            low[k] += damping * (value - low[k])
            reads[k] = low[k]
        total = .5 * np.sum(reads)
        for k in range(4):
            # Householder scattering: four chambers exchange energy each round trip.
            returning = (total - reads[k]) * gains[k]
            saturated = np.tanh(returning * drive) / drive
            buffers[k, head] = saturated + excitation[i] * (1 if k % 2 == 0 else -.73)
        output[i] = .43 * reads[0] + .31 * reads[1] - .19 * reads[2] + .27 * reads[3]
    return output


def breath(seconds, frequency, rng, character="root"):
    t = np.arange(round(seconds * RATE)) / RATE
    # A breath has several pressure changes inside one held gesture; no vibrato duet.
    pressure = envelope(seconds, min(4.5, seconds * .3), min(6, seconds * .4))
    pressure *= .7 + .15 * np.sin(t * .71) + .09 * np.sin(t * 1.313 + 1.7)
    if character == "root":
        ratios, decay, damping, drive = [1, .751, .498, .337], 2.8, .16, 2.5
        noise = filtered(rng.normal(size=len(t)), 75, 1550)
        excitation = noise * .011 + np.sin(2*np.pi*frequency*t + .06*np.sin(t*.57)) * .006
    elif character == "wire":
        ratios, decay, damping, drive = [1, .713, .419, .287], 5.6, .45, 1.8
        noise = filtered(rng.normal(size=len(t)), 240, 3500)
        # Rough contact changes pressure without producing a regular rhythmic gate.
        excitation = noise * .016 * (.65 + .35 * np.sin(t * 2.67) ** 2)
    elif character == "glass":
        ratios, decay, damping, drive = [1, .819, .447, .312], 7.5, .62, 1.3
        noise = filtered(rng.normal(size=len(t)), 500, 5000)
        excitation = noise * .009
    else:  # A large ventilated pipe, driven by air rather than a sampled horn.
        ratios, decay, damping, drive = [1, .503, .251, .168], 3.6, .12, 3.4
        noise = filtered(rng.normal(size=len(t)), 50, 950)
        excitation = noise * .014 + np.sin(2*np.pi*frequency*t) * .004
    body = feedback_body(excitation * pressure, RATE / frequency * np.array(ratios),
                         decay, damping, drive, .6 if character == "glass" else 1.7)
    body = filtered(body, 32, 4500 if character == "glass" else 2600)
    body *= envelope(seconds, .08, min(3, seconds * .2))
    return level(body)


# Ratios, amplitude decay seconds, contact bandwidth. Inharmonic material modes.
BODIES = {
    "wood": ([1, 2.71, 5.17, 7.31], [.23, .15, .08, .05], 2600),
    "coconut": ([1, 1.83, 3.07, 4.91], [.34, .19, .12, .07], 3700),
    "stone": ([1, 1.48, 2.13, 3.41, 5.08], [.42, .27, .18, .12, .06], 6200),
    "steel": ([1, 1.391, 1.947, 2.619, 4.083, 6.327], [3.9, 3.1, 2.6, 1.8, 1.1, .7], 7600),
    "bell": ([1, 2.017, 2.77, 4.09, 5.43], [4.2, 3.1, 2.3, 1.4, .9], 6500),
    "ice": ([1, 2.319, 4.247, 6.633], [3.7, 2.6, 1.7, .9], 8200),
    "hide": ([1, 1.594, 2.136, 2.653, 3.155], [.78, .48, .31, .17, .11], 1800),
}


def strike(seconds, frequency, rng, material="wood", softness=.5):
    t = np.arange(round(seconds * RATE)) / RATE
    ratios, decays, contact_high = BODIES[material]
    data = np.zeros(len(t))
    for k, (ratio, decay) in enumerate(zip(ratios, decays)):
        hz = frequency * ratio * rng.uniform(.997, 1.003)
        if hz > RATE * .43:
            continue
        # Membrane / hollow-body attack relaxes in pitch; metal modes beat unevenly.
        fall = (5 if material in ("hide", "coconut") else .06) * (1 - np.exp(-t*35))
        phase = 2*np.pi*(hz*t + fall)
        beating = 1 + (.11 if material in ("steel", "ice") else .015) * np.sin(t*(k+1)*1.1)
        data += np.sin(phase) * np.exp(-t / decay) * beating / (1+k)**(1.1+softness)
    contact = filtered(rng.normal(size=len(t)), 150, contact_high)
    data += contact * np.exp(-t/(.006+softness*.014)) * (.15+.3*(1-softness))
    data *= envelope(seconds, .001 + softness*.003, min(.3, seconds*.15))
    return level(data)


def friction(seconds, frequency, rng, material="wood"):
    # Bow / rub a modal body with discontinuous, correlated contact noise.
    t = np.arange(round(seconds * RATE)) / RATE
    noise = filtered(rng.normal(size=len(t)), 90, 7000)
    contact = (.3 + .7*np.sin(t*1.137+.6)**4) * envelope(seconds, seconds*.3, seconds*.4)
    data = np.zeros(len(t))
    ratios, _, _ = BODIES[material]
    for k, ratio in enumerate(ratios[:4]):
        hz = frequency * ratio
        if hz > 11000:
            continue
        b, a = signal.iirpeak(hz, 22 + k*17, fs=RATE)
        data += signal.lfilter(b, a, noise * contact) / (1+k)**1.3
    data *= envelope(seconds, .04, min(1, seconds*.2))
    return level(data)


@njit(cache=True)
def chamber(mono, lengths, seconds, damping):
    """Stereo late reflections: separate larger four-line feedback delay network."""
    size = int(np.max(lengths)) + 1
    buffers = np.zeros((4, size))
    low = np.zeros(4)
    wet = np.zeros((len(mono), 2))
    gains = 10 ** (-3 * lengths / (RATE * seconds))
    for i in range(len(mono)):
        head = i % size
        for k in range(4):
            value = buffers[k, (head - lengths[k]) % size]
            low[k] += damping * (value - low[k])
        total = .5 * np.sum(low)
        for k in range(4):
            buffers[k, head] = (total - low[k])*gains[k] + mono[i]*.38
        wet[i, 0] = low[0] - low[1] + low[2]*.3
        wet[i, 1] = low[2] - low[3] + low[0]*.3
    return wet
