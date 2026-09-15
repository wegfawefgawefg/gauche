"""Offline cork release, warm glass overtones and a quiet closing sigh."""
import numpy as np
from synth import clock, noise, band, save
for name,duration,seed,falling in [('summer_open',.72,2011,False),('summer_fade',.46,2017,True)]:
 t=clock(duration)
 breath=band(noise(len(t),seed),420,2900)*np.sin(np.pi*t/duration)**2
 tone=np.zeros(len(t))
 for pitch,gain in [(392,.45),(588,.23),(784,.10)]:
  tone+=gain*np.sin(2*np.pi*pitch*(t+(-.06 if falling else .04)*t*t))*np.exp(-t*7)
 cork=np.sin(2*np.pi*140*t)*np.exp(-t*70)
 save(name,.3*breath+tone+(.6*cork if not falling else 0),.30 if not falling else .18)
