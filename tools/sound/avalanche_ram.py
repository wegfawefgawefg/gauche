"""Offline rasping bleat, hoof shove, blunt horn impact and crumbling snow."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.55)
phase=2*np.pi*np.cumsum(165+30*np.sin(t*19))/44100
env=np.sin(np.pi*t/.55)**2
a=(np.sin(phase)+.3*np.sin(phase*3))*.25*env
for at in [.06,.29]:a+=band(noise(len(t),5631),300,2600)*np.exp(-((t-at)/.03)**2)
save('ram_paw',a,.32)
t=clock(.2)
a=band(noise(len(t),5633),120,1800)*np.sin(np.pi*t/.2)**2
save('ram_rush',a,.29)
for name,seed,pitch in [('ram_hit',5635,110),('ram_bonk',5637,170)]:
    t=clock(.30);phase=2*np.pi*(pitch*t+45*(1-np.exp(-t*30))/30)
    a=np.sin(phase)*np.exp(-t*20)*.5+band(noise(len(t),seed),200,2800)*np.exp(-t*35)
    if name=='ram_bonk':a+=.14*np.sin(2*np.pi*490*t)*np.exp(-t*14)
    save(name,a,.39)
t=clock(.32)
a=band(noise(len(t),5639),650,5200)*(1-np.exp(-t*80))*np.exp(-t*15)
save('ram_plough',a,.25)
t=clock(.71)
phase=2*np.pi*np.cumsum(170-100*t+15*np.sin(t*23))/44100
save('ram_death',(np.sin(phase)+.35*np.sin(phase*3))*(1-np.exp(-t*55))*np.exp(-t*6),.34)
