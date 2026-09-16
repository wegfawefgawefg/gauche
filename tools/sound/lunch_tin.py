"""Offline lunch lid/chewing and hollow tin sounds; no healing chime."""
import numpy as np
from synth import clock, noise, band, save
for name,span,seed,peak in [('lunch_eat',.75,15721,.18),('lunch_throw',.22,15722,.15),('lunch_clatter',.61,15723,.26),('lunch_spill',.44,15724,.18)]:
 t=clock(span);x=np.zeros(len(t))
 hits=[0,.22,.43] if name=='lunch_eat' else [0,.13,.25] if name=='lunch_clatter' else [0,.12]
 for i,start in enumerate(hits):
  q=np.maximum(0,t-start);gate=(t>=start)
  if name=='lunch_eat' and i>0:
   x+=band(noise(len(t),seed+i),250,1900)*np.exp(-q*23)*gate*.6
  else:
   x+=(.35*np.sin(2*np.pi*390*q)+.22*np.sin(2*np.pi*1037*q)+.12*np.sin(2*np.pi*2411*q))*np.exp(-q*32)*gate/(i+1)
 x+=band(noise(len(t),seed),600,4800)*np.exp(-t*22)*.22
 save(name,x,peak)
