"""Offline pipe taps, crack, escaping water and a dry-fire warning/failure."""
import numpy as np
from synth import clock,noise,band,save
t=clock(.22)
x=np.sin(2*np.pi*310*t)*np.exp(-26*t)+.4*band(noise(len(t),19591),600,5000)*np.exp(-60*t)
save('pipe_hit',x,.23)
t=clock(.38)
x=band(noise(len(t),19593),250,5400)*np.exp(-20*t)+.5*np.sin(2*np.pi*(370*t-220*t*t))*np.exp(-16*t)
save('pipe_break',x,.27)
t=clock(.65)
x=band(noise(len(t),19597),900,6400)*np.sin(np.pi*t/.65)**2
x+=.12*np.sin(2*np.pi*(1800*t+120*t*t))*np.exp(-9*t)
save('pipe_leak',x,.13)
t=clock(1.1)
x=band(noise(len(t),19601),1500,5800)*np.sin(np.pi*t/1.1)**2*.3
x+=np.sin(2*np.pi*(550*t+350*t*t))*(.6+.4*np.sin(2*np.pi*7*t))*np.sin(np.pi*t/1.1)**2
save('boiler_dry_warn',x,.24)
t=clock(.9)
x=band(noise(len(t),19603),60,2100)*np.exp(-6*t)+.4*np.sin(2*np.pi*(110*t-34*t*t))*np.exp(-8*t)
save('boiler_dry_fire',x,.32)
