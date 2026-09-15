"""Three linked folding steel panels, hinge squeak and locking footfalls."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.63);a=np.zeros(len(t));rattle=band(noise(len(t),15101),450,4300)
for at,pitch in [(0,590),(.14,440),(.28,710)]:
 q=np.maximum(0,t-at);a+=(.25*np.sin(2*np.pi*pitch*q)+.2*rattle)*np.exp(-q*22)*(t>=at)
a+=.16*np.sin(2*np.pi*(900*t+180*t*t))*np.exp(-((t-.23)/.12)**2)
q=np.maximum(0,t-.44);a+=.2*np.sin(2*np.pi*120*q)*np.exp(-q*29)*(t>=.44)
save('barricade_unfold',a,.30)
