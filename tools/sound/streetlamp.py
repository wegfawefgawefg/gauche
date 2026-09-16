"""Offline thin steel knocks, bending post and glass/lamp collapse."""
import numpy as np
from synth import clock, noise, band, save
for name,span,seed,peak in [('pole_hit',.24,15701,.20),('pole_creak',.92,15702,.25),('pole_fall',.72,15703,.30),('pole_break',.39,15704,.22)]:
 t=clock(span);u=t/span
 if name=='pole_creak':
  phase=2*np.pi*(190*t-55*t*t/span)
  x=(np.sin(phase)+.3*np.sin(phase*2.73))*np.sin(np.pi*u)**2
  x+=band(noise(len(t),seed),300,2200)*(.3+.7*np.sin(2*np.pi*12*t)**2)*np.sin(np.pi*u)**2
 else:
  x=np.sin(2*np.pi*135*t)*np.exp(-t*16)
  for f,w,d in [(560,.35,22),(927,.2,28),(1783,.12,35)]:x+=w*np.sin(2*np.pi*f*t)*np.exp(-t*d)
  x+=.6*band(noise(len(t),seed),2200,9500)*np.exp(-t*(9 if name=='pole_fall' else 25))
 save(name,x,peak)
