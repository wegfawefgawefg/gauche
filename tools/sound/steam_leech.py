"""Wet suction, bubbling pressure and a steam release, synthesized offline."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.20)
slurp = band(noise(len(t), 2801), 350, 2000)*np.exp(-t*20)
slurp += .24*np.sin(2*np.pi*(150*t+950*t*t))*np.exp(-t*28)
save('leech_latch', slurp, .25)
t = clock(.43)
bubble = sum(np.sin(2*np.pi*(f*t-180*t*t))*np.exp(-((t-delay)*40)**2)
             for f, delay in [(360,.04),(270,.16),(180,.29)])
save('leech_sip', bubble, .21)
t = clock(.60)
pressure = band(noise(len(t), 2803), 900, 3900)*(.2+.8*t/.6)*np.sin(np.pi*t/.6)**.6
pressure += .13*np.sin(2*np.pi*(290*t+210*t*t))*np.sin(np.pi*t/.6)
save('leech_warning', pressure, .32)
t = clock(.44)
steam = band(noise(len(t), 2819), 1200, 7400)*(1-np.exp(-t*160))*np.exp(-t*10)
steam += .12*np.sin(2*np.pi*115*t)*np.exp(-t*27)
save('leech_burst', steam, .44)
t = clock(.18)
pop = .4*np.sin(2*np.pi*(180*t-220*t*t))*np.exp(-t*36)
pop += band(noise(len(t), 2833), 300, 2400)*np.exp(-t*40)
save('leech_detach', pop, .22)
t = clock(.33)
wet = band(noise(len(t), 2837), 160, 1900)*np.exp(-t*17)
wet += .18*np.sin(2*np.pi*(145*t-130*t*t))*np.exp(-t*20)
save('leech_death', wet, .27)
