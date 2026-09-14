"""Offline crunchy herbs/root, splint wrap, juicy pepper and soft bread sounds."""
import numpy as np
from synth import clock, band, noise, save

def chews(name, seed, low, high, duration, peaks, body=.0):
    t=clock(duration)
    envelope=np.zeros(len(t))
    for start in peaks:
        age=np.maximum(t-start, 0)
        envelope+=(t>=start)*(1-np.exp(-age*350))*np.exp(-age*24)
    crunch=band(noise(len(t),seed),low,high)
    mouth=body*np.sin(2*np.pi*(170*t+22*np.sin(2*np.pi*3*t)))
    save(name,(crunch+mouth)*envelope,.28)

chews('herbs_chew',115,650,4700,.45,[.01,.14,.29],.05)
chews('root_chew',232,1100,6500,.48,[.01,.19,.32],.08)
chews('chili_bite',791,350,2900,.39,[.01,.15],.18)
chews('bread_munch',650,200,2200,.52,[.01,.18,.33],.11)
t=clock(.44)
wrap=band(noise(len(t),895),700,3600)*np.sin(np.pi*t/.44)**1.5
snap=band(noise(len(t),554),130,1700)*np.exp(-np.maximum(t-.32,0)*50)*(t>=.32)
save('splint_bind',wrap*.6+snap,.29)
