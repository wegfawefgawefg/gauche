"""Low animal breaths and woody impacts; offline synthesis with headroom."""
import numpy as np
from synth import RATE, clock, noise, band, save

for name,length,pitch,peak in [('growth_roar',.8,78,.48),('growth_death',1.15,62,.48)]:
    t=clock(length)
    phase=2*np.pi*np.cumsum(pitch*(1-.35*t/length)+8*np.sin(2*np.pi*17*t))/RATE
    breath=band(noise(len(t),71),160,2200)
    voice=(np.sin(phase)+.25*np.sin(phase*2)+.42*breath)*np.sin(np.pi*t/length)**.7
    save(name,voice,peak)
for name,length,seed,pitch in [('growth_maul',.3,73,95),('growth_crash',.46,75,66)]:
    t=clock(length)
    sound=(np.sin(2*np.pi*pitch*t)*np.exp(-16*t)+band(noise(len(t),seed),180,3100)*np.exp(-22*t))
    save(name,sound,.53)
t=clock(.5)
save('growth_rush',band(noise(len(t),77),240,2800)*np.sin(np.pi*t/.5)**1.2,.36)
