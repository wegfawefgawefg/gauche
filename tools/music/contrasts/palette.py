"""Distinct excitation/oscillator families for pass 03; no shared pipe bed.

These are authored synthetic instruments, not sampled imitations of an orchestra.
The legacy breath resonator is reserved for the one Ice composition.
"""
import numpy as np
from numba import njit
from scipy import signal

from materials import RATE, envelope, filtered, level


@njit(cache=True)
def string_loop(initial, count, loss, brightness):
    ring = initial.copy()
    result = np.zeros(count)
    size = len(ring)
    previous = ring[-1]
    for i in range(count):
        index = i % size
        value = ring[index]
        result[i] = value
        # A lossy bridge reflection progressively removes upper string modes.
        ring[index] = loss * ((1-brightness)*value + brightness*previous)
        previous = value
    return result


def prepared(seconds,hz,rng,pin=.15,felt=.35):
    count = round(seconds*RATE)
    period = max(8,round(RATE/hz-.5))
    excitation = rng.normal(size=period)
    # Plucking position suppresses selected modes rather than making a sawtooth.
    excitation -= np.roll(excitation,max(1,round(period*.21))) * .8
    excitation = np.convolve(excitation,[.2,.6,.2],mode="same")
    excitation -= excitation.mean()
    loss = np.exp(-6.9/(hz*(4.8-felt*2.6)))
    dry = string_loop(excitation,count,loss,.22+felt*.45)
    t = np.arange(count)/RATE
    # A loose bridge pin contributes an inharmonic buzz mostly during the attack.
    buzz = np.sin(2*np.pi*hz*2.713*t) * np.exp(-t*5.2)
    dry = filtered(dry,35,4900-felt*3200)
    dry += pin * buzz * max(.01,np.max(np.abs(dry)))
    b,a = signal.iirpeak(214,3.5,fs=RATE)
    body = signal.lfilter(b,a,dry)
    dry = .8*dry+.27*body
    dry *= envelope(seconds,.003+felt*.009,min(.6,seconds*.25))
    return level(dry)


def tape_shards(seconds,hz,rng,reverse=.35):
    """Time-windowed scraps of our own prepared-string voice, no external sample."""
    source = prepared(5,hz,rng,pin=.23,felt=.65)
    result = np.zeros(round(seconds*RATE))
    at = .05
    while at < seconds-.4:
        duration = rng.uniform(.35,1.2)
        rate = rng.choice([.5,1,1,1.5]) * rng.uniform(.994,1.006)
        n = round(duration*RATE)
        position = rng.uniform(.05,2.3)*RATE
        indices = position + np.arange(n)*rate
        scrap = np.interp(indices,np.arange(len(source)),source,left=0,right=0)
        if rng.random() < reverse:
            scrap = scrap[::-1]
        scrap = scrap * np.hanning(n) * rng.uniform(.35,.9)
        start = round(at*RATE)
        take = min(n,len(result)-start)
        result[start:start+take] += scrap[:take]
        at += rng.uniform(.7,1.8)
    result *= envelope(seconds,.03,min(1,seconds*.2))
    return level(filtered(result,85,3200))


def rubbed_glass(seconds,hz,rng,rough=.04):
    """Coherent held overtones: intentionally unlike the broad pipe resonator."""
    t = np.arange(round(seconds*RATE))/RATE
    pressure = envelope(seconds,min(3,seconds*.3),min(4,seconds*.4))
    result = np.zeros(len(t))
    for k,(ratio,gain) in enumerate([(1,1),(2.318,.17),(4.247,.065),(6.633,.017)]):
        frequency = hz*ratio
        if frequency > 10500:
            continue
        drift = .06*np.sin(t*(.39+k*.037)+rng.uniform(0,6))
        beating = .82+.12*np.sin(t*(.7+k*.33)+k)
        result += np.sin(2*np.pi*frequency*t+drift)*gain*beating
    result += filtered(rng.normal(size=len(t)),1200,6000)*rough
    return level(result*pressure)


def bass(seconds,hz,rng,grit=.4):
    t = np.arange(round(seconds*RATE))/RATE
    # Low saw / triangle hybrid. Higher harmonics close faster than the fundamental.
    phase = 2*np.pi*hz*t + .025*np.sin(t*5.7)
    result = np.zeros(len(t))
    for k in range(1,25):
        weight = (.9 if k%2 else .35)/k**1.4
        result += weight*np.sin(phase*k)*np.exp(-t*(.8+k*.9))
    result = np.tanh(result*(1+grit*3))
    result = filtered(result,28,2800)
    result *= envelope(seconds,.008,min(.35,seconds*.3))
    return level(result)


def relay(seconds,hz,rng,bend=-.22):
    t = np.arange(round(seconds*RATE))/RATE
    phase = 2*np.pi*hz*(t+bend*(1-np.exp(-t*5))/5)
    # Inharmonic phase modulation, with its metallic sidebands fading independently.
    result = np.sin(phase + 2.1*np.exp(-t*6)*np.sin(phase*1.414))
    result *= np.exp(-t*5)
    result += filtered(rng.normal(size=len(t)),900,6200)*np.exp(-t*85)*.09
    return level(result*envelope(seconds,.004,.08))


def rattle(seconds,hz,rng,density=18):
    count = round(seconds*RATE)
    t = np.arange(count)/RATE
    noise = filtered(rng.normal(size=count),max(100,hz),min(9500,hz*5))
    hits = np.zeros(count)
    for at in np.cumsum(rng.uniform(.6/density,1.4/density,round(seconds*density*2))):
        if at >= seconds:
            break
        hits[round(at*RATE)] = rng.uniform(.15,1)
    decay = np.exp(-np.arange(round(.025*RATE))/(RATE*.004))
    contact = signal.fftconvolve(hits,decay)[:count]
    return level(noise*contact*envelope(seconds,.04,min(.17,seconds*.3)))


def scrape(seconds,hz,rng,teeth=23):
    t = np.arange(round(seconds*RATE))/RATE
    noise = filtered(rng.normal(size=len(t)),hz, min(11000,hz*9))
    speed = teeth*t + .8*np.sin(t*1.3)
    tooth = np.maximum(0,np.sin(2*np.pi*speed))**6
    # Dragging across a rack: irregular tooth impacts, not a smooth sustained pad.
    data = noise*(.12+tooth)*envelope(seconds,.1,min(.5,seconds*.3))
    return level(data)


def radio(seconds,hz,rng):
    t = np.arange(round(seconds*RATE))/RATE
    noise = filtered(rng.normal(size=len(t)),650,2400)
    packet = (np.sin(t*13.71)*np.sin(t*5.37) > .18).astype(float)
    packet = signal.sosfilt(signal.butter(1,30,fs=RATE,output="sos"),packet)
    ring = np.sin(2*np.pi*hz*t)*(.18+.12*np.sin(t*71))
    data = (noise*.3+ring)*packet*envelope(seconds,.12,min(.4,seconds*.3))
    return level(data)


VOICES = {"prepared":prepared,"tape":tape_shards,"glass":rubbed_glass,
          "bass":bass,"relay":relay,"rattle":rattle,"scrape":scrape,"radio":radio}
