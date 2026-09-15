"""Offline cloth pack and compacted-snow crunch; melting ends in water drops."""
import numpy as np
from synth import clock,noise,band,save
for name,length,peak,seed in [('shelter_pack',.55,.19,2701),('shelter_hit',.18,.13,2707),('shelter_break',.46,.22,2711),('shelter_melt',.55,.12,2713),('shelter_empty',.22,.08,2719)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='shelter_pack':
  env=np.exp(-t*19)+np.where(t>.2,np.exp(-np.maximum(0,t-.2)*18),0)
  s=band(n,180,2800)*env
 elif name=='shelter_melt':
  s=band(n,600,3800)*np.exp(-t*6)*.5
  for offset,freq in [(.08,720),(.23,980),(.39,640)]:
   dt=np.maximum(0,t-offset);s+=np.where(t>=offset,.16*np.sin(2*np.pi*(freq*dt-350*dt*dt))*np.exp(-dt*45),0)
 elif name=='shelter_break':s=band(n,140,4600)*np.exp(-t*9)*(.7+.3*np.sin(2*np.pi*27*t))
 elif name=='shelter_empty':s=band(n,350,1900)*np.sin(np.pi*t/length)**2
 else:s=band(n,140,2100)*np.exp(-t*28)+.1*np.sin(2*np.pi*125*t)*np.exp(-t*36)
 save(name,s,peak)
