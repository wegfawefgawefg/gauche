"""Offline match scrape, breath, dying wick, wax fracture and linen winding."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.44)
a=band(noise(len(t),5711),1900,7200)*np.exp(-t*37)
a+=band(noise(len(t),5713),170,1100)*np.exp(-((t-.10)/.075)**2)*.7
save('candle_light',a,.26)
t=clock(.31)
a=band(noise(len(t),5715),280,2100)*np.sin(np.pi*t/.31)**2*np.exp(-t*8)
save('candle_out',a,.23)
t=clock(.4)
a=band(noise(len(t),5717),2300,6500)*np.exp(-t*14)*( .3+.7*np.sin(2*np.pi*31*t)**2)
save('candle_spent',a,.16)
t=clock(.18)
a=band(noise(len(t),5719),500,4400)*np.exp(-t*55)
a+=.4*np.sin(2*np.pi*260*t)*np.exp(-t*70)
save('candle_break',a,.28)
t=clock(.57)
a=band(noise(len(t),5721),650,4200)*np.sin(np.pi*t/.57)**2*(.3+.7*np.sin(2*np.pi*9*t)**8)
save('wick_feed',a,.22)
t=clock(.22)
a=band(noise(len(t),5723),800,4500)*np.exp(-t*45)
a+=.3*np.sin(2*np.pi*530*t)*np.exp(-t*34)
save('wick_empty',a,.24)
