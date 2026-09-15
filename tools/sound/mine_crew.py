"""Offline whistles, work grunts, metal pick strikes and heavy boots."""
import numpy as np
from synth import clock,noise,band,save

def voice(t,pitch,seed):
    phase=2*np.pi*(pitch*t-22*t*t)
    return band(np.sin(phase)+.45*np.sin(2*phase)+.2*np.sin(3*phase),120,1600)+.12*band(noise(len(t),seed),300,1400)

t=clock(.48)
env=np.sin(np.pi*t/.48)**2
save('crew_whistle',np.sin(2*np.pi*(1750*t+60*np.sin(t*8)*t))*env,.22)
t=clock(.7);march=np.zeros_like(t)
for at,pitch in [(0,130),(.23,145),(.46,115)]:
    u=np.maximum(t-at,0);march+=voice(u,pitch,9101)*np.exp(-((u-.08)/.045)**2)*(t>=at)
save('crew_march',march,.25)
for name,seconds,pitch,seed in [('crew_heave',.3,105,9111),('crew_warn',.28,155,9113),('crew_complain',.4,135,9115),('crew_death',.65,110,9117)]:
    t=clock(seconds);save(name,voice(t,pitch,seed)*np.sin(np.pi*t/seconds)**2,.27)
for name,freq,seed in [('crew_pick',1420,9121),('crew_pick2',1680,9123)]:
    t=clock(.3)
    hit=band(noise(len(t),seed),700,6500)*np.exp(-45*t)
    hit+=.35*np.sin(2*np.pi*freq*t)*np.exp(-20*t)+.18*np.sin(2*np.pi*freq*1.43*t)*np.exp(-28*t)
    save(name,hit,.36)
t=clock(.18)
save('crew_swing',band(noise(len(t),9131),200,4200)*np.exp(-((t-.065)/.04)**2),.3)
t=clock(.64)
save('crew_alarm',np.sin(2*np.pi*(1900*t+80*t*t))*(np.exp(-((t-.12)/.065)**2)+np.exp(-((t-.43)/.09)**2)),.3)
for name,seed,freq in [('crew_step1',9141,110),('crew_step2',9143,95)]:
    t=clock(.14)
    save(name,band(noise(len(t),seed),100,1500)*np.exp(-45*t)+.6*np.sin(2*np.pi*freq*t)*np.exp(-35*t),.16)
