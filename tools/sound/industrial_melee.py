"""Offline hammer heft, iron impact, rubber knocks and a small valve release."""
import numpy as np
from synth import clock, noise, band, save
for name,length,seed,gain in [('press_heave',.48,9411,.25),('mallet_swing',.13,9413,.15)]:
 t=clock(length);n=band(noise(len(t),seed),150,2600)
 env=np.sin(np.pi*t/length)**2
 save(name,n*env*(1-.5*t/length),gain)
t=clock(.65)
x=band(noise(len(t),9421),70,5000)*np.exp(-32*t)
for f,g in [(92,.8),(217,.3),(613,.12)]:x+=g*np.sin(2*np.pi*f*t)*np.exp(-12*t)
save('press_impact',x,.46)
t=clock(.23)
x=np.sin(2*np.pi*(210*t-130*t*t))*np.exp(-30*t)
x+=.45*np.sin(2*np.pi*470*t)*np.exp(-50*t)+.25*band(noise(len(t),9423),200,1800)*np.exp(-65*t)
save('mallet_impact',x,.32)
t=clock(.26)
save('mallet_vent',band(noise(len(t),9425),1100,5500)*(1-np.exp(-110*t))*np.exp(-18*t),.18)
