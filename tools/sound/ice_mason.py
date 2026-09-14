"""Offline ice cutting and worker grunts. Short tells precede cover and melee."""
import numpy as np
from synth import clock, noise, band, save

def tap(t, delay, frequency):
    at=np.maximum(0,t-delay)
    return (t>=delay)*np.sin(2*np.pi*frequency*at)*np.exp(-at*50)

t=clock(.83)
cut=band(noise(len(t),2901),1300,6200)
env=sum(np.exp(-((t-at)/.02)**2) for at in [.02,.26,.51,.75])
cut=cut*env + sum(.17*tap(t,at,1850) for at in [.02,.26,.51,.75])
save('mason_cut',cut,.3)
t=clock(.32)
grunt=np.sin(2*np.pi*(95*t-40*t*t))+.35*np.sin(2*np.pi*195*t)
save('mason_lift',band(grunt,110,900)*np.sin(np.pi*t/.32)**2,.23)
t=clock(.44)
save('mason_set',band(noise(len(t),2903),250,1800)*np.sin(np.pi*t/.44)**2,.20)
t=clock(.22)
save('mason_place',.7*tap(t,0,135)+.3*tap(t,.005,780)+band(noise(len(t),2905),650,2800)*np.exp(-35*t),.35)
t=clock(.23)
save('mason_warn',(np.sin(2*np.pi*(150*t+70*t*t))+.35*band(noise(len(t),2907),350,1400))*np.sin(np.pi*t/.23)**2,.25)
t=clock(.16)
save('mason_jab',band(noise(len(t),2909),550,4100)*np.exp(-((t-.07)/.035)**2)+.3*tap(t,.08,430),.30)
t=clock(.53)
save('mason_death',(np.sin(2*np.pi*(160*t-90*t*t))+.3*band(noise(len(t),2911),130,900))*np.sin(np.pi*t/.53)**2,.3)
t=clock(.17)
save('ice_block_hit',tap(t,0,1670)+.45*tap(t,.009,2570)+band(noise(len(t),2913),1600,6500)*np.exp(-40*t),.3)
t=clock(.42)
crack=band(noise(len(t),2917),900,7100)*np.exp(-17*t)
crack+=sum(.2*tap(t,at,fr) for at,fr in [(0,2100),(.05,3100),(.09,1400),(.15,2700)])
save('ice_block_break',crack,.42)
