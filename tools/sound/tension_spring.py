"""Offline loaded steel ratchet, latch, spring release and brittle coil break."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.45);x=np.zeros(len(t))
for i,start in enumerate([0,.07,.14,.22,.30]):
 u=np.maximum(0,t-start)
 x+=(t>=start)*(.7*np.sin(2*np.pi*(270+i*43)*u)+band(noise(len(t),15471+i),700,4200)*.25)*np.exp(-u*65)
save('tension_set',x,.24)
t=clock(.15);x=np.sin(2*np.pi*713*t)*np.exp(-t*47)+.4*band(noise(len(t),15476),900,4300)*np.exp(-t*75)
save('tension_ready',x,.18)
t=clock(.65);phase=2*np.pi*(410*t-235*t*t)
x=(np.sin(phase)+.32*np.sin(phase*2.71))*(.5+.5*np.cos(2*np.pi*22*t))*np.exp(-t*6)
x+=band(noise(len(t),15477),130,3000)*np.exp(-t*24)*.4
save('tension_launch',x,.32)
t=clock(.4);x=band(noise(len(t),15478),400,5400)*np.exp(-t*24)
x+=.4*np.sin(2*np.pi*1273*t)*np.exp(-t*17)
save('tension_break',x,.25)
