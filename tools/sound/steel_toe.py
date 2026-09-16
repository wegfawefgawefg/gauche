"""Offline leather kick, a punt's metal ring, wall knock and worn-cap rattle."""
import numpy as np
from synth import clock, noise, band, save
for name,span,seed,peak in [('toe_kick',.20,15711,.21),('toe_punt',.32,15712,.22),('toe_wall',.28,15713,.23),('toe_spent',.48,15714,.18)]:
 t=clock(span)
 x=.6*band(noise(len(t),seed),170,3200)*np.exp(-t*23)
 x+=np.sin(2*np.pi*(100*t-24*t*t))*np.exp(-t*31)
 if name!='toe_kick':
  for f,w,d in [(480,.25,16),(1131,.18,24),(2677,.11,30)]:x+=w*np.sin(2*np.pi*f*t)*np.exp(-t*d)
 if name=='toe_spent':
  for start in [.08,.17,.26]:
   q=np.maximum(0,t-start);x+=.25*np.sin(2*np.pi*701*q)*np.exp(-q*45)*(t>=start)
 save(name,x,peak)
