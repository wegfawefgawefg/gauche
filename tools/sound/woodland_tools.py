"""Offline soil scratches, tacky resin, seed rattle and small woody growth cues."""
import numpy as np
from synth import clock, band, noise, save

t=clock(.18)
scrape=band(noise(len(t),986),900,5700)
save('claw_scratch',scrape*np.sin(np.pi*t/.18)**.65,.30)
t=clock(.32)
sticky=np.sin(2*np.pi*(340*t-310*t*t))+.3*band(noise(len(t),437),300,2300)
save('resin_repair',sticky*np.exp(-t*12)+.3*np.sin(2*np.pi*700*t)*np.exp(-np.maximum(t-.16,0)*30)*(t>.16),.29)
t=clock(.25)
rattle=band(noise(len(t),238),1700,6700)*(.3+.7*np.sin(2*np.pi*22*t)**6)
save('seed_plant',(rattle+.4*np.sin(2*np.pi*110*t))*np.exp(-t*18),.28)
t=clock(.38)
rustle=band(noise(len(t),902),130,2700)
save('cover_grow',(rustle+.24*np.sin(2*np.pi*(120*t+140*t*t)))*np.sin(np.pi*t/.38)**.8,.33)
t=clock(.46)
tone=np.sin(2*np.pi*610*t)+.35*np.sin(2*np.pi*913*t)+.15*np.sin(2*np.pi*1500*t)
save('lantern_plant',tone*np.exp(-t*11)+.4*band(noise(len(t),788),500,3200)*np.exp(-t*36),.24)
