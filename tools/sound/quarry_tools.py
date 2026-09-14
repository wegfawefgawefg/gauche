"""Offline chisel swish/fracture and a weighty prepared brick throw."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.10)
save('ice_brick_ready',(np.sin(2*np.pi*840*t)+.35*np.sin(2*np.pi*1330*t))*np.exp(-t*50),.18)
t=clock(.25)
save('ice_brick_throw',band(noise(len(t),2941),170,1700)*np.sin(np.pi*t/.25)**2,.28)
t=clock(.09)
save('chisel_jab',band(noise(len(t),2943),1200,6500)*np.sin(np.pi*t/.09)**2,.21)
t=clock(.35)
snap=band(noise(len(t),2947),1800,6700)*np.exp(-t*32)
snap+=.23*np.sin(2*np.pi*2180*t)*np.exp(-t*18)
snap+=.18*np.sin(2*np.pi*2940*t)*np.exp(-t*23)
save('chisel_break',snap,.33)
