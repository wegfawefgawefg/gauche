"""Cord tension, a wound spring, and a dry bursting shell, synthesized offline."""
import numpy as np
from synth import clock, noise, band, save
for name,length,seed,peak in [('snare_place',.24,140,.23),('snare_catch',.2,366,.36),('snare_release',.29,972,.26)]:
 t=clock(length); e=np.sin(np.pi*t/length)**2
 fiber=band(noise(len(t),seed),600,5400)
 if name=='snare_catch': e=np.exp(-t*23); fiber+=.24*np.sin(2*np.pi*(270*t-240*t*t))
 save(name,fiber*e,peak)
t=clock(.38)
ratchet=band(noise(len(t),893),600,4000)*np.maximum(0,np.sin(2*np.pi*26*t))**8
save('spring_set',(ratchet+.12*np.sin(2*np.pi*320*t))*np.sin(np.pi*t/.38)**2,.3)
t=clock(.42)
phase=2*np.pi*(180*t+70*np.sin(2*np.pi*8*t)*t)
save('spring_launch',(np.sin(phase)*.35+band(noise(len(t),816),450,3900)*np.exp(-t*30))*np.exp(-t*9),.42)
t=clock(.23)
save('spring_break',(band(noise(len(t),208),300,4800)+.22*np.sin(2*np.pi*420*t))*np.exp(-t*24),.3)
t=clock(.16)
save('acorn_set',(band(noise(len(t),73),240,2200)+.2*np.sin(2*np.pi*240*t))*np.exp(-t*28),.23)
t=clock(.35)
cracks=band(noise(len(t),610),600,7500)*np.exp(-t*20)
for delay in [.045,.082,.12]: cracks+=band(noise(len(t),int(delay*10000)),1100,5800)*np.where(t>=delay,np.exp(-np.maximum(0,t-delay)*60),0)*.25
save('acorn_burst',cracks+.24*np.sin(2*np.pi*110*t)*np.exp(-t*28),.5)
