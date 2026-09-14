"""Offline plate movement, sword commitment and ringing reflected impacts."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.3)
save('knight_raise', (band(noise(len(t),4301),300,2400)*.6+np.sin(2*np.pi*370*t)*.15+np.sin(2*np.pi*812*t)*.08)*np.sin(np.pi*t/.3)**2, .27)
t=clock(.28)
save('knight_ready', (np.sin(2*np.pi*820*t)*.5+np.sin(2*np.pi*1903*t)*.25)*np.exp(-t*17), .25)
t=clock(.37)
save('knight_warn', (band(noise(len(t),4303),200,1800)*.55 + .22*np.sin(2*np.pi*(165*t-30*t*t)))*np.sin(np.pi*t/.37)**2, .32)
t=clock(.19)
save('knight_slash', (band(noise(len(t),4305),800,7000)*.65 + .14*np.sin(2*np.pi*(1100*t-1500*t*t)))*np.sin(np.pi*t/.19)**2, .43)
t=clock(.32)
save('knight_reflect', (np.sin(2*np.pi*1387*t)*.45 + np.sin(2*np.pi*2873*t)*.24 + band(noise(len(t),4307),1700,6000)*.13)*np.exp(-t*17), .37)
t=clock(.55)
a=band(noise(len(t),4309),130,3800)*np.exp(-t*14)
for start,f in [(.09,370),(.18,612),(.31,1107)]:
 s=np.maximum(0,t-start); a += (t>=start)*np.exp(-s*27)*np.sin(2*np.pi*f*s)*.22
save('knight_death', a, .45)
