"""Offline metal grille clangs, hollow bins and contained ore rubble."""
import numpy as np
from synth import clock,noise,band,save
for name,seed,broken in [('grate_hit',9611,False),('grate_break',9613,True),('scrap_hit',9615,False),('scrap_break',9617,True)]:
 t=clock(.65 if broken else .28);n=band(noise(len(t),seed),400,6200)
 x=.45*n*np.exp(-28*t)
 for f,g in [(190,.5),(511,.35),(1337,.12)]:x+=g*np.sin(2*np.pi*f*t)*np.exp(-(8 if broken else 17)*t)
 if broken:x+=.65*n*np.exp(-((t-.18)/.085)**2)
 save(name,x,.36 if broken else .23)
for name,seed,broken in [('ore_hit',9621,False),('ore_break',9623,True)]:
 t=clock(.6 if broken else .2);n=band(noise(len(t),seed),150,4500)
 env=np.exp(-26*t)
 if broken:env+=.45*np.exp(-((t-.17)/.06)**2)+.2*np.exp(-((t-.34)/.08)**2)
 save(name,n*env+.25*np.sin(2*np.pi*105*t)*np.exp(-25*t),.32 if broken else .2)
