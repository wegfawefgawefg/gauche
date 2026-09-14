"""Offline flames, jars, sparks, pouring and sticky placement."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.38); save('fire_catch',band(noise(len(t),920),300,7000)*np.sin(np.pi*t/.38)**.7,.34)
t=clock(.20);save('torch_swing',band(noise(len(t),922),250,4900)*np.sin(np.pi*t/.20),.31)
t=clock(.13);save('lighter_spark',band(noise(len(t),924),1600,10000)*(np.exp(-t*95)+.65*np.where(t>.05,np.exp(-np.maximum(0,t-.05)*65),0)),.30)
t=clock(.2);save('bottle_throw',band(noise(len(t),926),450,4000)*np.sin(np.pi*t/.2),.24)
t=clock(.32);glass=sum(np.sin(2*np.pi*f*t)*np.exp(-t*d) for f,d in [(2300,35),(3727,28),(4981,22)])
save('bottle_break',glass*.3+band(noise(len(t),928),800,10000)*np.exp(-t*23),.42)
t=clock(.54);save('water_pour',band(noise(len(t),930),400,6000)*np.sin(np.pi*t/.54)**.8*(.7+.3*np.sin(2*np.pi*19*t)),.34)
t=clock(.23);save('honey_place',np.sin(2*np.pi*(240*t-320*t*t))*np.exp(-t*20)+band(noise(len(t),931),100,1000)*np.exp(-t*30),.28)
