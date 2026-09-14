"""Breathy hunting horn and rope cast/latch/reeling sounds, generated offline."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.72)
phase=2*np.pi*(148*t + 2*np.sin(2*np.pi*4*t)/25)
horn=sum(np.sin(phase*n)*(.65**(n-1)) for n in range(1,7))
envelope=np.minimum(t/.055,1)*np.minimum((.72-t)/.2,1)
breath=band(noise(len(t),963),250,1900)*.13
save('horn_blast',(horn+breath)*envelope,.48)
t=clock(.29)
save('hook_cast',band(noise(len(t),711),450,4000)*np.sin(np.pi*t/.29)**2,.31)
t=clock(.24)
metal=sum(np.sin(2*np.pi*t*f)*np.exp(-t*k) for f,k in [(730,32),(1611,45),(2763,54)])
save('hook_latch',metal+band(noise(len(t),929),1200,6000)*np.exp(-t*70),.38)
t=clock(.095)
save('hook_reel',band(noise(len(t),434),230,2100)*np.sin(np.pi*t/.095),.17)
t=clock(.21)
save('hook_release',band(noise(len(t),856),1200,4600)*np.exp(-t*35),.23)
