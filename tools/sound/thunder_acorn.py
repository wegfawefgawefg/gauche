"""Dry seed flick and short electrical cracks; no runtime synthesis."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.27)
swish=band(noise(len(t),1369),600,5700)*np.sin(np.pi*t/.27)**2
swish+=.2*np.sin(2*np.pi*(720*t+1600*t*t))*np.exp(-t*19)
save('thunder_throw',swish,.32)
t=clock(.36)
burst=np.zeros(len(t))
for start,gain in [(0,1),(.018,.65),(.053,.38),(.097,.16)]:
    age=np.maximum(0,t-start)
    burst+=(t>=start)*gain*band(noise(len(t),1403+round(start*10000)),650,9000)*np.exp(-age*75)
burst+=.23*np.sin(2*np.pi*(170*t-125*t*t))*np.exp(-t*23)
save('thunder_crack',burst,.53)
