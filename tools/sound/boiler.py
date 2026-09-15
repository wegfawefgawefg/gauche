"""Offline steam pressure, weighted wheels, work grunts and valve hardware."""
import numpy as np
from synth import clock, noise, band, save
for name,seed,duration,peak in [('boiler_warn',6121,.75,.35),('boiler_vent',6123,.65,.32),('boiler_scald',6125,.3,.075),('boiler_leak',6127,.28,.12)]:
 t=clock(duration);a=band(noise(len(t),seed),850,6200)
 if name=='boiler_warn':
  a*=np.linspace(.07,.9,len(t))
  a+=.35*np.sin(2*np.pi*(570*t+270*t*t))*np.sin(np.pi*t/duration)**2
 else: a*=(1-np.exp(-t*70))*np.exp(-t*(8 if name=='boiler_vent' else 13))
 save(name,a,peak)
for name,seed,duration in [('boiler_roll',6131,.35),('boiler_break',6133,.8),('valve_fit',6135,.42),('valve_remove',6137,.35),('locker_open',6139,.55)]:
 t=clock(duration);a=band(noise(len(t),seed),200,4700)*np.exp(-t*15)
 for f,w in [(163,.34),(271,.25),(433,.15)]: a+=w*np.sin(2*np.pi*f*t)*np.exp(-t*12)
 if name=='boiler_roll': a*=.3+.7*np.sin(t*38)**4
 if name=='boiler_break': a+=band(noise(len(t),seed+1),800,6500)*np.exp(-t*6)
 save(name,a,.4 if name=='boiler_break' else .27)
for name,seed,duration in [('porter_heave',6141,.29),('porter_warn',6143,.4),('porter_hit',6145,.19),('porter_death',6147,.62)]:
 t=clock(duration);phase=2*np.pi*(130*t-30*t*t)
 voice=sum(np.sin(i*phase)/i for i in range(1,9))
 a=voice*.17+band(noise(len(t),seed),280,2600)*.6
 a*=np.sin(np.pi*t/duration)**2
 if name=='porter_hit': a+=band(noise(len(t),seed+1),600,5000)*np.exp(-t*25)
 save(name,a,.29)
t=clock(.48)
a=band(noise(len(t),6151),350,2900)*np.sin(np.pi*t/.48)**2*(.5+.5*np.sin(t*47)**2)
save('sealant_patch',a,.25)
t=clock(.2);a=band(noise(len(t),6153),1000,5000)*np.exp(-t*24)
save('sealant_empty',a,.19)
