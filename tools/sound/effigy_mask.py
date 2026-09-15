"""Offline dry wooden scrape and breath-like filtered noise, no voice recording."""
import numpy as np
from synth import clock,noise,band,save
for name,length,peak,seed in [('mask_raise',.32,.15,2609),('mask_lower',.21,.09,2617),('mask_whisper',.38,.035,2621),('mask_spent',.5,.19,2633)]:
 t=clock(length);n=noise(len(t),seed)
 if name=='mask_whisper':s=band(n,800,2600)*np.sin(np.pi*t/length)**2
 elif name=='mask_spent':s=band(n,500,4200)*np.exp(-t*18)+.2*np.sin(2*np.pi*145*t)*np.exp(-t*10)
 else:
  s=band(n,300,2100)*np.exp(-t*15)*(.6+.4*np.sin(2*np.pi*23*t))
  s+=.2*np.sin(2*np.pi*210*t)*np.exp(-t*25)
 save(name,s,peak)
