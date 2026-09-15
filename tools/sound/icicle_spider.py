"""Offline fine silk scrapes, snapping strands, wet fangs and brittle death."""
import numpy as np
from synth import clock, noise, band, save
for name,seed,duration in [('spider_weave',6021,.72),('spider_set',6023,.26),('spider_cut',6025,.18),('spider_catch',6027,.32)]:
    t=clock(duration); a=band(noise(len(t),seed),1600,6500)
    if name=='spider_weave': a*=np.sin(t*30)**8*np.sin(np.pi*t/duration)**2
    else: a*=np.exp(-t*25)
    a+=.18*np.sin(2*np.pi*(780*t+90*t*t))*np.exp(-t*18)
    save(name,a,.2 if name=='spider_weave' else .29)
t=clock(.27)
a=band(noise(len(t),6029),2100,7500)*(1-np.exp(-t*65))*np.exp(-t*13)
save('spider_burn',a,.23)
t=clock(.35)
a=band(noise(len(t),6031),800,4200)*np.sin(t*45)**6*np.sin(np.pi*t/.35)**2
save('spider_warn',a,.32)
t=clock(.19)
a=band(noise(len(t),6033),350,3700)*np.exp(-t*29)+.3*np.sin(2*np.pi*260*t)*np.exp(-t*35)
save('spider_bite',a,.34)
t=clock(.52)
a=band(noise(len(t),6035),1000,7300)*np.exp(-t*11)
a+=.14*np.sin(2*np.pi*(370*t-120*t*t))*np.exp(-t*10)
save('spider_death',a,.35)
