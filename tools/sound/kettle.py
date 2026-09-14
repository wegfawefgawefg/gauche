"""Offline gurgling fill, a brief lid whistle, pour, and soft scattered steam."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.55)
a=band(noise(len(t),5931),200,2900)*(1-np.exp(-t*30))*np.exp(-t*5)
for start,f in [(0.06,390),(.17,520),(.29,315)]:
    age=np.maximum(0,t-start)
    a+=.35*np.sin(2*np.pi*(f*age-170*age*age))*np.exp(-age*26)*(t>=start)
save('kettle_fill',a,.3)
t=clock(.65)
a=.5*np.sin(2*np.pi*(1020*t+170*t*t))*np.sin(np.pi*t/.65)**2
b=band(noise(len(t),5933),1400,4800)*(1-np.exp(-t*24))*np.exp(-t*6)
save('kettle_ready',a+b*.5,.24)
t=clock(.25)
a=band(noise(len(t),5935),350,3100)*np.sin(np.pi*t/.25)**2
save('kettle_pour',a,.16)
for name,seed,high in [('kettle_scald',5937,6700),('kettle_splash',5939,3300)]:
    t=clock(.4)
    a=band(noise(len(t),seed),650,high)*(1-np.exp(-t*45))*np.exp(-t*10)
    save(name,a,.085)
t=clock(.18)
a=.2*np.sin(2*np.pi*660*t)*np.exp(-t*40)+band(noise(len(t),5941),800,2600)*np.exp(-t*28)
save('kettle_cool',a,.09)
