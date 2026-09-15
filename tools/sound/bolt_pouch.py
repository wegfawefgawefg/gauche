"""Original cloth handful and small steel fasteners scattering on stone."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.27)
x=band(noise(len(t),15511),700,4600)*np.sin(np.pi*t/.27)**2*.35
for start,freq in [(0.025,930),(0.065,1371),(0.10,2120),(0.12,1640)]:
    u=np.maximum(0,t-start);env=(t>=start)*np.exp(-u*55)
    x+=env*(np.sin(2*np.pi*freq*u)+.21*np.sin(2*np.pi*freq*2.73*u))*.12
save('bolts_throw',x,.24)
t=clock(.34)
x=np.zeros_like(t)
for start,freq,gain in [(0,1710,1),(.055,2290,.58),(.12,1440,.29),(.20,2860,.12)]:
    u=np.maximum(0,t-start);env=(t>=start)*np.exp(-u*65)
    x+=gain*env*(np.sin(2*np.pi*freq*u)+.3*np.sin(2*np.pi*freq*2.41*u))
x+=band(noise(len(t),15512),1600,6900)*np.exp(-t*32)*.22
save('bolts_clatter',x,.105)
