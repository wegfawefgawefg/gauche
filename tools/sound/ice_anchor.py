"""Offline steel screw knocks and a short rope ratchet; no runtime synthesis."""
import numpy as np
from synth import clock,noise,band,save
for name,length,peak,seed in [('anchor_set',.34,.2,2651),('anchor_recover',.3,.15,2659),('anchor_blocked',.18,.12,2663),('anchor_tension',.36,.15,2671),('anchor_reel',.1,.08,2677),('anchor_break',.48,.22,2683)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='anchor_reel':
  s=band(n,700,3900)*np.exp(-t*42)+.15*np.sin(2*np.pi*870*t)*np.exp(-t*65)
 elif name=='anchor_tension':
  s=band(n,140,1300)*np.sin(np.pi*t/length)**2*(.7+.3*np.sin(2*np.pi*38*t))
 elif name=='anchor_break':
  s=band(n,200,4600)*np.exp(-t*16)+.3*np.sin(2*np.pi*1320*t)*np.exp(-t*13)
 elif name=='anchor_blocked':
  s=band(n,120,820)*np.exp(-t*35)+.2*np.sin(2*np.pi*160*t)*np.exp(-t*27)
 else:
  s=band(n,350,2600)*np.exp(-t*18)
  for f in [640,1067,1893]:s+=.17*np.sin(2*np.pi*f*t)*np.exp(-t*24)
 save(name,s,peak)
