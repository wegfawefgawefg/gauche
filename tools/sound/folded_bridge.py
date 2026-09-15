"""Offline wooden hinge clacks, rope snap, splash and hollow footfall."""
import numpy as np
from synth import clock,noise,band,save
for name,length,seed in [('bridge_unfold',.6,2203),('bridge_break',.4,2207),('bridge_splash',.5,2213),('bridge_step',.13,2221)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='bridge_unfold':
  s=np.zeros(len(t))
  for start in [0,.13,.28]:
   age=np.maximum(0,t-start);env=np.exp(-age*35)*(t>=start)
   s+=(band(n,450,2500)*.5+np.sin(2*np.pi*180*age)*.25)*env
 elif name=='bridge_break':s=band(n,300,5200)*np.exp(-t*15)+np.sin(2*np.pi*(180*t-80*t*t))*np.exp(-t*12)*.2
 elif name=='bridge_splash':s=band(n,240,3200)*np.exp(-t*9)*(1-np.exp(-t*70))
 else:s=(np.sin(2*np.pi*135*t)*.5+band(n,300,2100)*.35)*np.exp(-t*40)
 save(name,s,{'bridge_unfold':.29,'bridge_break':.32,'bridge_splash':.25,'bridge_step':.10}[name])
