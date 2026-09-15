"""Original offline transformer start, restrained arc crackle and steel cutting."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.30)
rise=np.sin(np.pi*t/.6)**2
hum=np.sin(2*np.pi*(96*t+230*t*t))+.2*np.sin(2*np.pi*640*t)
click=band(noise(len(t),12301),600,6000)*np.exp(-t*60)
save('arc_prime',hum*.35*rise+click,.24)
for name,seed,metal in [('arc_pulse',12303,False),('arc_cut',12305,True)]:
    t=clock(.17)
    sparks=band(noise(len(t),seed),350,6800)*(.28+.72*np.maximum(0,np.sin(t*2*np.pi*83))**3)
    a=(sparks+.18*np.sin(2*np.pi*120*t))*(1-np.exp(-t*500))*np.exp(-t*14)
    if metal:a+=.18*np.sin(2*np.pi*1771*t)*np.exp(-t*20)
    save(name,a,.24 if metal else .20)
t=clock(.26)
a=band(noise(len(t),12307),100,1100)*np.exp(-t*25)
a+=.2*np.sin(2*np.pi*(190*t-220*t*t))*np.sin(np.pi*t/.26)
save('arc_empty',a,.18)
