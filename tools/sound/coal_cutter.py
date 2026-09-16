"""Short offline cutter strokes and stop cues; leave room for world sound."""
import numpy as np
from synth import clock, noise, band, save
t=clock(.4)
x=band(noise(len(t),19571),160,2200)*np.sin(np.pi*t/.4)**2
x*=.5+.5*np.sin(2*np.pi*36*t)**2
x+=.18*np.sin(2*np.pi*130*t)*np.exp(-12*t)
save('cutter_cut',x,.20)
t=clock(.24)
x=band(noise(len(t),19573),220,3400)*np.exp(-28*t)
x+=.5*np.sin(2*np.pi*(400*t-350*t*t))*np.exp(-17*t)
save('cutter_jam',x,.22)
t=clock(.55)
x=band(noise(len(t),19579),150,1200)*np.exp(-8*t)*(.5+.5*np.cos(2*np.pi*(35*t-26*t*t)))
x+=.15*np.sin(2*np.pi*(160*t-110*t*t))*np.exp(-9*t)
save('cutter_empty',x,.17)
