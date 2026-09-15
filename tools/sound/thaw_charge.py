"""Offline latch, hot wire ticks, brittle fracture and scalding vapor."""
import numpy as np
from synth import clock,noise,band,save
for name,length,seed in [('thaw_place',.22,2101),('thaw_fuse',.13,2111),('thaw_burst',.58,2113),('thaw_steam',.32,2129)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='thaw_place':
  s=band(n,450,3300)*np.exp(-t*34)+np.sin(2*np.pi*190*t)*np.exp(-t*40)*.5
 elif name=='thaw_fuse':
  s=band(n,1300,5100)*np.exp(-t*35)*(1-np.exp(-t*300))
 elif name=='thaw_burst':
  s=band(n,100,5400)*np.exp(-t*7)
  for f in [281,467,731]:s+=.12*np.sin(2*np.pi*f*t)*np.exp(-t*20)
  s*=1-np.exp(-t*350)
 else:s=band(n,800,3700)*np.sin(np.pi*t/length)**2
 save(name,s,{'thaw_place':.25,'thaw_fuse':.13,'thaw_burst':.38,'thaw_steam':.07}[name])
