"""Offline inharmonic metal chimes, a tiny transmitted tone and glass fracture."""
import numpy as np
from synth import clock,noise,band,save
for name,seconds,peak,seed in [('fork_windup',.5,.13,2401),('fork_ring',.8,.21,2407),('fork_travel',.22,.025,2411),('fork_spent',.4,.16,2417),('crystal_break',.38,.23,2423)]:
 t=clock(seconds);n=noise(len(t),seed)
 if name=='crystal_break':
  s=band(n,1500,8500)*np.exp(-t*23)
  for hz in [1871,2733,4319]:s+=.13*np.sin(2*np.pi*hz*t)*np.exp(-t*15)
 elif name=='fork_windup':
  s=band(n,700,3800)*np.exp(-t*60)*.4+np.sin(2*np.pi*523*t)*np.exp(-t*8)*.3
 elif name=='fork_spent':
  s=(np.sin(2*np.pi*(450*t-220*t*t))*.4+band(n,500,2200)*.2)*np.exp(-t*14)
 else:
  s=sum(np.sin(2*np.pi*hz*t)*amp*np.exp(-t*decay) for hz,amp,decay in [(523,.6,5),(1048,.2,9),(2761,.12,20)])
  s*=1-np.exp(-t*150)
 save(name,s,peak)
