"""Offline bowed-glass charge, brittle electrical discharge and splintering crystal."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.75)
env=np.sin(np.pi*t/.75)**2
rub=band(noise(len(t),5791),850,5200)*env*.65
for frequency,amount in [(417,.25),(711,.15),(1149,.08)]:
    phase=2*np.pi*(frequency*t+65*t*t)
    rub+=np.sin(phase)*amount*env*(.6+.4*np.sin(t*37)**2)
save('shard_charge',rub,.26)
t=clock(.28)
a=band(noise(len(t),5793),350,6900)*(1-np.exp(-t*100))*np.exp(-t*19)
for f in [619,1093,1847]: a+=.12*np.sin(2*np.pi*f*t)*np.exp(-t*23)
save('shard_pulse',a,.32)
t=clock(.62)
a=np.zeros_like(t)
for at,f,seed in [(0,1801,5795),(.04,2903,5797),(.12,3419,5799),(.29,2237,5801)]:
    age=np.maximum(0,t-at)
    env=np.exp(-age*35)*(t>=at)
    a+=(band(noise(len(t),seed),1500,9000)+.2*np.sin(2*np.pi*f*age))*env
save('shard_break',a,.31)
