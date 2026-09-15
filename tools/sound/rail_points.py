"""Offline points lever: iron scrape, double latch, and exhausted socket."""
import numpy as np
from synth import clock, noise, band, save
for name,length,seed,base,peak in [('points_turn',.48,15661,175,.24),('points_break',.46,15662,98,.29),('switch_key_spent',.28,15663,410,.19)]:
    t=clock(length)
    x=band(noise(len(t),seed),250,3100)*np.sin(np.pi*t/length)*.18
    for onset,amp in [(0,.7),(length*.58,1)]:
        u=np.maximum(0,t-onset)
        strike=np.sin(2*np.pi*base*u)*np.exp(-u*28)+.32*np.sin(2*np.pi*base*2.73*u)*np.exp(-u*39)
        x+=(t>=onset)*amp*strike
    save(name,x,peak)
