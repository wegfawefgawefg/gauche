"""Offline runner scrapes, bent wood knocks and dry splitting/crackle."""
import numpy as np
from synth import clock,noise,band,save
for name,length,peak,seed in [('sled_deploy',.4,.18,2801),('sled_board',.25,.14,2803),('sled_slide',.09,.055,2819),('sled_brake',.3,.13,2833),('sled_break',.55,.23,2837),('sled_burn',.65,.10,2843)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='sled_slide':
  s=band(n,300,3800)*np.sin(np.pi*t/length)**2
 elif name=='sled_brake':
  s=band(n,240,3300)*np.sin(np.pi*t/length)**2*(1-t/length)
 elif name=='sled_burn':
  s=band(n,1100,7000)*.15
  for offset in [.025,.17,.22,.39,.51]:
   dt=np.maximum(0,t-offset);s+=np.where(t>=offset,band(n,700,6500)*np.exp(-dt*130),0)
 elif name=='sled_break':
  s=band(n,200,5600)*np.exp(-t*16)
  for offset in [.08,.19,.29]:
   dt=np.maximum(0,t-offset);s+=np.where(t>=offset,band(n,400,3900)*np.exp(-dt*38),0)
 else:
  s=band(n,140,1200)*np.exp(-t*20)
  s+=.3*np.sin(2*np.pi*(180*t-85*t*t))*np.exp(-t*25)
  if name=='sled_deploy':s+=.2*np.sin(2*np.pi*870*t)*np.exp(-t*30)
 save(name,s,peak)
