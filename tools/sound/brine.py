"""Offline liquid slosh, saltwater splash, dented metal and a springing tin lid."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.24)
env=np.sin(np.pi*t/.24)**2
slosh=band(noise(len(t),5591),180,1200)*env
save('brine_throw',slosh+.08*np.sin(2*np.pi*(180*t+130*t*t))*env,.24)
t=clock(.52)
a=band(noise(len(t),5593),250,5000)*(1-np.exp(-t*180))*np.exp(-t*11)
for at,pitch in [(.03,1700),(.08,2300),(.14,3300)]:
    age=np.maximum(0,t-at)
    a+=.05*np.sin(2*np.pi*pitch*age)*np.exp(-age*50)*(t>=at)
save('brine_splash',a,.35)
for name,duration,seed,peak in [('tin_hit',.24,5595,.27),('tin_open',.58,5597,.32)]:
    t=clock(duration)
    a=band(noise(len(t),seed),500,4800)*np.exp(-t*70)*.5
    for pitch,weight in [(460,.35),(823,.25),(1273,.12)]:
        a+=weight*np.sin(2*np.pi*pitch*t)*np.exp(-t*17)
    if name=='tin_open':
        age=np.maximum(0,t-.09)
        a+=.4*np.sin(2*np.pi*(510*age+190*age*age))*np.exp(-age*10)*(t>=.09)
        a+=band(noise(len(t),seed+2),1800,8000)*np.exp(-((t-.16)/.04)**2)*.4
    save(name,a,peak)
