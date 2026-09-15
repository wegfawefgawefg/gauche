"""Offline furnace feet, straining hinge, short breath, feed grind and collapse."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.42)
x=.8*np.sin(2*np.pi*(59*t-17*t*t))*np.exp(-t*15)
x+=band(noise(len(t),15461),300,2700)*np.exp(-t*25)*.5
save('kiln_step',x,.3)
t=clock(.9)
x=.25*np.sin(2*np.pi*(145*t+18*np.sin(t*9)))*(1-np.exp(-t*30))*np.exp(-t*3)
x+=band(noise(len(t),15462),450,2100)*(.3+.7*np.sin(t*19)**4)*np.exp(-t*4)
x+=.3*np.sin(2*np.pi*313*t)*np.exp(-t*15)
save('kiln_door',x,.28)
t=clock(.38)
x=band(noise(len(t),15463),95,4200)*(1-np.exp(-t*40))*np.exp(-t*8)
x+=.12*np.sin(2*np.pi*67*t)*np.exp(-t*10)
save('kiln_breath',x,.30)
t=clock(.9)
x=band(noise(len(t),15464),170,2600)*(.15+.85*np.sin(t*29)**12)*np.exp(-t*3)
x+=.2*np.sin(2*np.pi*101*t)*np.exp(-t*6)
save('kiln_feed',x,.23)
t=clock(1.1)
x=band(noise(len(t),15465),100,5300)*np.exp(-t*6)
for i,f in enumerate([233,381,659,1043]):
 start=i*.07;u=np.maximum(0,t-start)
 x+=.3*(t>=start)*np.sin(2*np.pi*f*u)*np.exp(-u*(12+i*3))
save('kiln_death',x,.36)
