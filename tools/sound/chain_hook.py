"""Original iron chain links, catch and short ratcheting haul. No music changes."""
import numpy as np
from synth import clock, noise, band, save

for name,length,seed,events,peak in [
 ('chain_cast',.36,15531,[(0,930),(.06,1700),(.13,1300),(.22,2100)],.18),
 ('chain_latch',.22,15532,[(0,620),(.018,1430)],.23),
 ('chain_haul',.20,15533,[(0,510),(.045,980),(.09,1530)],.15),
 ('chain_release',.30,15534,[(0,1900),(.08,1200),(.16,750)],.14),
 ('chain_spent',.40,15535,[(0,740),(.12,460),(.22,310)],.18)]:
 t=clock(length);x=np.zeros(len(t));n=band(noise(len(t),seed),500,6000)
 for start,freq in events:
  u=np.maximum(t-start,0);gate=(t>=start)
  ring=sum(np.sin(2*np.pi*freq*ratio*u)*gain for ratio,gain in [(1,.42),(1.47,.23),(2.13,.16)])
  x+=gate*(ring*np.exp(-u*42)+n*.3*np.exp(-u*100))
 save(name,x,peak)
