"""Offline ceramic-memory-stone throw, landing, blank voice and crumble."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.19)
save('pebble_throw',band(noise(len(t),1409),700,3900)*np.sin(np.pi*t/.19)**2,.18)
t=clock(.25)
save('pebble_land',band(noise(len(t),1423),600,5100)*np.exp(-t*44)+
     .3*np.sin(2*np.pi*760*t)*np.exp(-t*25),.24)
t=clock(.55)
tone=sum(np.sin(2*np.pi*f*t)*np.exp(-t*d)/a for f,d,a in [(620,13,1),(1138,19,2),(1817,27,4)])
save('pebble_tone',tone+band(noise(len(t),1427),1200,5000)*np.exp(-t*65),.28)
t=clock(.37)
crumb=band(noise(len(t),1429),1300,6600)*(np.exp(-t*20)+.6*np.exp(-np.maximum(t-.10,0)*38)*(t>.10))
save('pebble_spent',crumb,.19)
