"""Thick liquid splash, a flask swing, cooling crust and reheated bubbles."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.24)
save('tar_throw',band(noise(len(t),15631),250,3300)*np.sin(np.pi*t/.24)**1.5,.19)
t=clock(.54)
x=band(noise(len(t),15632),120,1700)*(1-np.exp(-t*100))*np.exp(-t*10)
x+=.45*np.sin(2*np.pi*(180*t-100*t*t))*np.exp(-t*17)
x+=.18*band(noise(len(t),15633),1600,6200)*np.exp(-t*42)
save('tar_splash',x,.30)
t=clock(.38)
x=band(noise(len(t),15634),800,5900)*np.exp(-t*16)*(.2+.8*np.sin(2*np.pi*29*t)**10)
x+=.2*np.sin(2*np.pi*751*t)*np.exp(-t*28)
save('tar_clot',x,.20)
t=clock(.4)
x=band(noise(len(t),15635),110,1200)*np.sin(np.pi*t/.4)*(.25+.75*np.sin(2*np.pi*11*t)**4)
save('tar_soften',x,.19)
