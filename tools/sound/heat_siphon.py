"""Offline vacuum gulp, pressure release and quiet flame texture."""
import numpy as np
from synth import clock,noise,band,save
for name,length,seed in [('siphon_draw',.42,2041),('siphon_fire',.38,2053),('siphon_flame',.18,2069)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='siphon_draw':
  env=np.sin(np.pi*t/length)**.8
  hiss=band(n,260,1800)*env
  tone=np.sin(2*np.pi*(170*t+290*t*t))*env*.17
  sound=hiss+tone+np.sin(2*np.pi*95*t)*np.exp(-t*70)*.4
 elif name=='siphon_fire':
  sound=band(n,100,4100)*np.exp(-t*9)*(1-np.exp(-t*150))
  sound+=np.sin(2*np.pi*72*t)*np.exp(-t*18)*.4
 else:sound=band(n,550,3600)*np.sin(np.pi*t/length)**2
 save(name,sound,.34 if name=='siphon_fire' else .23 if name=='siphon_draw' else .07)
