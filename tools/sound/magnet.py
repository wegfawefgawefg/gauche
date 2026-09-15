"""Short strained metallic pull and spent ferrite crumble. Offline synthesis."""
import numpy as np
from synth import clock,noise,band,save,RATE

t=clock(.19)
phase=2*np.pi*np.cumsum(105+170*t)/RATE
body=(np.sin(phase)+.35*np.sin(phase*2.73))*np.sin(np.pi*t/.19)**2
rasp=band(noise(len(t),13301),550,2700)*np.exp(-((t-.12)/.025)**2)
save('magnet_pull',.7*body+.22*rasp,.22)
t=clock(.43)
a=band(noise(len(t),13309),220,1600)*np.exp(-t*11)
for pitch,delay in [(680,.02),(410,.08),(270,.14)]:
    q=np.maximum(0,t-delay);a+=.18*np.sin(2*np.pi*pitch*q)*np.exp(-q*25)*(t>=delay)
save('magnet_spent',a,.24)
