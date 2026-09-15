"""Offline cork pop, worried chatter, claw scrape and quick work boots."""
import numpy as np
from synth import clock, noise, band, save
for name,seconds,pitch,seed in [('powder_panic',.45,260,9411),('powder_warn',.2,210,9413),('powder_death',.5,190,9415)]:
 t=clock(seconds);phase=2*np.pi*(pitch*t+3*np.sin(2*np.pi*10*t))
 env=np.sin(np.pi*t/seconds)**2*(.45+.55*np.sin(2*np.pi*9*t)**2)
 save(name,(np.sin(phase)+.3*np.sin(phase*2)+.15*band(noise(len(t),seed),500,3500))*env,.27)
t=clock(.16)
save('powder_cork',np.sin(2*np.pi*(650*t-1700*t*t))*np.exp(-40*t)+.35*band(noise(len(t),9421),400,5000)*np.exp(-50*t),.27)
t=clock(.16)
save('powder_scratch',band(noise(len(t),9423),800,5500)*np.sin(np.pi*t/.16)**2,.24)
for name,seed in [('powder_step1',9425),('powder_step2',9427)]:
 t=clock(.08)
 save(name,band(noise(len(t),seed),150,2200)*np.exp(-65*t)+.35*np.sin(2*np.pi*180*t)*np.exp(-70*t),.13)
