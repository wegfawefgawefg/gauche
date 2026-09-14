"""Offline charged glass grenade: mechanical release, fizzing light and a crisp burst."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.22)
save('prism_throw', (band(noise(len(t),4401),400,3800)*.6+.15*np.sin(2*np.pi*420*t))*np.sin(np.pi*t/.22)**2,.28)
t=clock(.20)
save('prism_fuse', (band(noise(len(t),4403),2300,7300)*.5+.11*np.sin(2*np.pi*2711*t)+.08*np.sin(2*np.pi*3397*t))*np.sin(np.pi*t/.2)**2,.18)
t=clock(.25)
save('prism_land', (band(noise(len(t),4405),250,2100)*.5+.25*np.sin(2*np.pi*730*t)+.12*np.sin(2*np.pi*2117*t))*np.exp(-t*25),.26)
t=clock(.48)
a=band(noise(len(t),4407),300,7300)*np.exp(-t*24)
a+=.23*np.sin(2*np.pi*(210*t-110*t*t))*np.exp(-t*16)
for f in [937,1493,2503,3701]: a+=.11*np.sin(2*np.pi*f*t)*np.exp(-t*12)
save('prism_burst',a,.49)
