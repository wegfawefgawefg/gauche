"""Thrown cloth, stretched resin and quick retreat/slip sounds; offline only."""
import numpy as np
from synth import clock, noise, band, save
for name,seconds,seed,peak in [('net_throw',.32,782,.32),('net_catch',.26,433,.35),('net_fall',.22,617,.22)]:
 t=clock(seconds); cloth=band(noise(len(t),seed),350,4600)
 envelope=np.sin(np.pi*t/seconds)**2 if name=='net_throw' else np.exp(-t*20)
 save(name,cloth*envelope*(.6+.4*np.sin(2*np.pi*24*t)**2),peak)
t=clock(.32)
resin=np.sin(2*np.pi*(110*t+250*t*t))*.28+band(noise(len(t),853),160,1900)
save('boots_stick',resin*np.exp(-t*16),.31)
t=clock(.27)
save('boots_release',(np.sin(2*np.pi*(180*t+360*t*t))*.18+band(noise(len(t),588),500,3200))*np.sin(np.pi*t/.27)**2,.26)
t=clock(.22)
save('rabbit_escape',band(noise(len(t),751),250,3800)*np.sin(np.pi*t/.22)**2*(.55+.45*np.cos(2*np.pi*19*t)**2),.36)
t=clock(.2)
save('oil_slip',(band(noise(len(t),204),160,1400)+.16*np.sin(2*np.pi*(260*t-380*t*t)))*np.sin(np.pi*t/.2)**2,.28)
