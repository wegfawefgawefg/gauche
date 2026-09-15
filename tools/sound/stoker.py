"""Offline granular coal handling, breathy furnace spitting and laboring grunts."""
import numpy as np
from synth import clock, noise, band, save
for name,seconds,pitch,seed in [('stoker_cough',.32,100,9211),('stoker_heave',.34,125,9213),('stoker_death',.65,90,9215)]:
 t=clock(seconds);env=np.sin(np.pi*t/seconds)**2
 phase=2*np.pi*(pitch*t-20*t*t)
 grain=band(noise(len(t),seed),160,2000)
 save(name,(.55*grain+.5*np.sin(phase)+.23*np.sin(phase*2))*env,.28)
t=clock(.52);r=band(noise(len(t),9221),700,5300)
env=np.exp(-((t-.13)/.09)**2)+.6*np.exp(-((t-.34)/.08)**2)
save('stoker_scoop',r*env+.25*np.sin(2*np.pi*1130*t)*np.exp(-30*t),.24)
t=clock(.24);r=band(noise(len(t),9223),180,6200)
save('stoker_spit',r*np.exp(-17*t)+.32*np.sin(2*np.pi*(180*t-180*t*t))*np.exp(-20*t),.36)
t=clock(.21)
save('stoker_impact',band(noise(len(t),9225),450,6500)*np.exp(-30*t)+.45*np.sin(2*np.pi*155*t)*np.exp(-35*t),.3)
t=clock(.25)
save('stoker_swing',band(noise(len(t),9227),160,2500)*np.exp(-((t-.10)/.06)**2),.3)
