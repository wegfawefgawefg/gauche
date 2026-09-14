"""Offline mechanical ratchet, escapement and double-bell alarm Foley."""
import numpy as np
from synth import clock, noise, band, save

def strike(t, start, frequency, decay):
    age = np.maximum(0, t-start)
    return (t >= start) * np.exp(-age/decay) * (
        np.sin(2*np.pi*frequency*age) + .32*np.sin(2*np.pi*frequency*2.71*age))

# WIND: Little spring teeth accelerate and settle into a wound ratchet.
t = clock(.48); a = np.zeros_like(t)
for i, start in enumerate([.03,.10,.16,.21,.25,.29,.34]):
    a += strike(t,start,700+70*i,.012)
a += band(noise(len(t),4911),400,3000)*np.sin(np.pi*t/.48)**2*.15
save('clock_wind',a,.25)
t = clock(.12)
save('clock_tick',strike(t,.015,1250,.009)+strike(t,.04,830,.007),.16)
# RING: Five playback pulses are separated by a second in deterministic gameplay.
t = clock(.72); a = np.zeros_like(t)
for i, start in enumerate(np.arange(.02,.5,.048)):
    a += strike(t,start,1680 if i%2 else 2120,.055)
save('clock_ring',a,.34)
t = clock(.24)
save('clock_stop',strike(t,.015,480,.035)+strike(t,.065,760,.025),.20)
t = clock(.48)
a = band(noise(len(t),4913),500,6500)*np.exp(-t/.045)
for i, start in enumerate([.015,.08,.16,.25]):
    a += .6**i*strike(t,start,1100+317*i,.04)
save('clock_break',a,.35)
