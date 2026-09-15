"""Offline small metal tools and a short, deep directional rock charge."""
import numpy as np
from synth import clock, noise, band, save
for name,seed in [('quarry_place',9311),('fuse_snip',9313),('scissors_spent',9315)]:
 t=clock(.2);n=band(noise(len(t),seed),600,6000)
 x=n*np.exp(-45*t)+.3*np.sin(2*np.pi*1400*t)*np.exp(-35*t)
 if name=='scissors_spent':x+=.3*np.sin(2*np.pi*650*t)*np.exp(-18*t)
 save(name,x,.24)
t=clock(.18)
save('quarry_fuse',band(noise(len(t),9321),1800,7500)*(np.exp(-30*t)+.25*np.exp(-((t-.11)/.02)**2)),.16)
t=clock(.55)
save('fuse_douse',band(noise(len(t),9323),600,7000)*np.exp(-9*t),.25)
t=clock(.8);n=band(noise(len(t),9325),80,4200)
x=n*np.exp(-7*t)+.7*np.sin(2*np.pi*(80*t-22*t*t))*np.exp(-8*t)
x+=.4*band(noise(len(t),9327),600,4500)*np.exp(-((t-.28)/.14)**2)
save('quarry_burst',x,.47)
