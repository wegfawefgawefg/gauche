"""Dry grain scattering, little beak taps and brittle thorns, synthesized offline."""
import numpy as np
from synth import clock, noise, band, save

def scatter(name,seed,duration,peaks,low,high):
    t=clock(duration)
    e=np.zeros(len(t))
    for start in peaks:
        age=np.maximum(0,t-start)
        e+=(t>=start)*(1-np.exp(-age*1800))*np.exp(-age*80)
    save(name,band(noise(len(t),seed),low,high)*e,.26)

scatter('seed_scatter',511,.4,[.01,.05,.09,.14,.19,.25,.31],1300,6800)
scatter('seed_peck',680,.16,[.01,.065],600,4300)
scatter('thorn_scatter',945,.34,[.01,.1,.18,.24],900,7200)
t=clock(.17)
e=(1-np.exp(-t*1300))*np.exp(-t*32)
prick=band(noise(len(t),170),2100,8800)*.7+np.sin(2*np.pi*(510*t-270*t*t))*.2
save('thorn_prick',prick*e,.35)
