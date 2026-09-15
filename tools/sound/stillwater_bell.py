"""Offline damped bowl resonance and a quiet current splash."""
import numpy as np
from synth import clock,noise,band,save
for name,length,peak,seed in [('stillwater_ring',1.3,.22,2503),('stillwater_spent',.5,.13,2511),('current_drift',.24,.07,2521)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='current_drift':s=band(n,300,2700)*np.exp(-t*18)*(1-np.exp(-t*90))
 else:
  base=294 if name=='stillwater_ring' else 196
  s=sum(np.sin(2*np.pi*base*ratio*t)*amp*np.exp(-t*decay) for ratio,amp,decay in [(1,.65,3),(2.76,.18,7),(5.4,.10,15)])
  s+=band(n,600,4200)*np.exp(-t*65)*.12
 save(name,s,peak)
