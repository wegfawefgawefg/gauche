"""Offline dry lumps, a stove door's low clang, ignition and cooling iron."""
import numpy as np
from synth import clock,noise,band,save
for name,duration,seed,peak in [('coal_feed',.38,5821,.26),('coal_land',.21,5823,.24),('stove_hit',.3,5825,.31),('stove_break',.65,5827,.36)]:
 t=clock(duration)
 a=band(noise(len(t),seed),300,4200)*np.exp(-t*38)
 if name.startswith('stove') or name=='coal_feed':
  for f,w in [(181,.25),(307,.18),(523,.11)]: a+=w*np.sin(2*np.pi*f*t)*np.exp(-t*14)
 if name=='stove_break':
  age=np.maximum(0,t-.17)
  a+=band(noise(len(t),seed+3),800,5100)*np.exp(-age*23)*(t>=.17)
 save(name,a,peak)
t=clock(.2)
save('coal_throw',band(noise(len(t),5831),180,1700)*np.sin(np.pi*t/.2)**2,.19)
t=clock(.63)
a=band(noise(len(t),5833),180,2900)*(1-np.exp(-t*28))*np.exp(-t*6)
save('stove_light',a,.28)
t=clock(.58)
a=band(noise(len(t),5835),1300,7200)*(1-np.exp(-t*90))*np.exp(-t*9)
save('stove_out',a,.24)
t=clock(.45)
a=band(noise(len(t),5837),900,3700)*np.exp(-t*16)
a+=.12*np.sin(2*np.pi*703*t)*np.exp(-t*36)
save('stove_spent',a,.16)
