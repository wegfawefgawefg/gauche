"""Finite shutdown ratchet, evacuation horn, pressure rupture and terminal blast."""
import numpy as np
from synth import clock, noise, band, save

t=clock(1.4)
x=band(noise(len(t),15731),120,1800)*np.exp(-t*4)*.35
for start in [0,.16,.31,.44,.56,.67,.77]:
 q=np.maximum(0,t-start)
 x+=(np.sin(2*np.pi*174*q)+.35*np.sin(2*np.pi*711*q))*np.exp(-q*43)*(t>=start)*.4
x+=np.sin(2*np.pi*(74*t-12*t*t))*np.exp(-t*2.5)*.25
save('reactor_shutdown',x,.32)
t=clock(.8);envelope=np.minimum(t/.06,1)*np.minimum((.8-t)/.16,1)
freq=185+25*np.sin(2*np.pi*1.6*t)
phase=np.cumsum(freq)*2*np.pi/44100
x=(np.sin(phase)+.3*np.sin(3*phase)+.14*np.sin(5*phase))*envelope
save('reactor_alarm',x,.26)
t=clock(.55)
x=band(noise(len(t),15733),100,4500)*np.exp(-t*8)+np.sin(2*np.pi*67*t)*np.exp(-t*13)*.5
save('reactor_rupture',x,.22)
t=clock(2.8)
x=band(noise(len(t),15734),35,2000)*np.exp(-t*2)+.6*np.sin(2*np.pi*(45*t+14*(1-np.exp(-t*5))))*np.exp(-t*2.2)
x+=band(noise(len(t),15735),2000,7000)*np.exp(-t*24)*.5
save('reactor_blast',x,.48)
