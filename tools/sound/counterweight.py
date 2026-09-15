"""Original geared travel, strained chain, stone impact and winch rewind."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.48)
ratchet = (.5 + .5*np.sin(2*np.pi*31*t))**10
x = band(noise(len(t),15571),350,3000)*ratchet*np.sin(np.pi*t/.48)
x += .2*np.sin(2*np.pi*96*t)*np.sin(np.pi*t/.48)
save('weight_travel',x,.20)
t = clock(.8)
x = band(noise(len(t),15572),500,4100)*(.3+.7*t/.8)
x *= (.5+.5*np.sin(2*np.pi*(11*t+8*t*t)))**8
for f in (327,731,1297):
    x += .15*np.sin(2*np.pi*f*t)*np.exp(-t*7)
save('weight_warn',x,.27)
t = clock(.8)
x = .8*np.sin(2*np.pi*(49*t+3*(1-np.exp(-t*20))))*np.exp(-t*10)
x += band(noise(len(t),15573),80,900)*np.exp(-t*12)
x += .6*band(noise(len(t),15574),700,4600)*np.exp(-t*24)
save('weight_slam',x,.40)
t = clock(1.8)
ratchet = (.5+.5*np.sin(2*np.pi*(15*t+3*t*t)))**10
x = band(noise(len(t),15575),250,2400)*ratchet*np.sin(np.pi*t/1.8)
x += .18*np.sin(2*np.pi*(84*t+4*t*t))*np.sin(np.pi*t/1.8)
save('weight_reel',x,.17)
t = clock(.85)
x = band(noise(len(t),15576),140,2900)*np.exp(-t*9)
for start,f in ((0,249),(.08,513),(.17,883),(.28,377)):
    age=np.maximum(t-start,0)
    x += .3*np.sin(2*np.pi*f*age)*np.exp(-age*17)*(t>=start)
save('weight_break',x,.29)
