"""Brief small-voice alarm and wooden mushroom-door slam, with mix headroom."""
import numpy as np
from synth import RATE, clock, noise, band, save
t=clock(.32)
pitch=720+260*np.sin(np.pi*t/.32)+45*np.sin(2*np.pi*28*t)
phase=2*np.pi*np.cumsum(pitch)/RATE
voice=(np.sin(phase)+.24*np.sin(phase*2)+.08*band(noise(len(t),91),1200,5000))*np.sin(np.pi*t/.32)**.7
save('gnome_squeal',voice,.35)
t=clock(.17)
save('gnome_door',(np.sin(2*np.pi*190*t)+.4*np.sin(2*np.pi*430*t))*np.exp(-42*t)+.3*band(noise(len(t),92),700,3800)*np.exp(-65*t),.38)
