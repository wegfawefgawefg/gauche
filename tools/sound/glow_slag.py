"""Offline vitrified slag clinks, soft cooling ticks and furnace intake."""
import numpy as np
from synth import clock, noise, band, save

for name,length,seed,base,peak in [
    ('glow_throw',.20,15691,790,.16),
    ('glow_land',.42,15692,650,.23),
    ('glow_heat',.72,15693,370,.18),
    ('glow_cool',.54,15694,990,.14),
]:
    t=clock(length)
    x=np.zeros(len(t))
    for ratio,weight,decay in [(1,.6,22),(1.73,.3,30),(2.41,.2,40)]:
        x+=weight*np.sin(2*np.pi*base*ratio*t)*np.exp(-t*decay)
    if name=='glow_heat':
        x+=band(noise(len(t),seed),150,1800)*np.sin(np.pi*t/length)**2
    elif name=='glow_cool':
        x+=.3*band(noise(len(t),seed),2200,7000)*np.exp(-t*9)
    else:
        x+=.25*band(noise(len(t),seed),500,4700)*np.exp(-t*25)
    save(name,x,peak)
