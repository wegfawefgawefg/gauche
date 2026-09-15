"""Threaded steel collar, direction latch, unthread and fitting fracture."""
import numpy as np
from synth import clock, noise, band, save

for name,duration,seed,rate in [('nozzle_fit',.48,15601,23),('nozzle_remove',.38,15602,18)]:
    t=clock(duration)
    envelope=np.sin(np.pi*t/duration)**.7
    x=band(noise(len(t),seed),500,4500)*envelope*(.15+.85*np.sin(2*np.pi*rate*t)**10)
    age=np.maximum(0,t-duration*.8)
    x+=(np.sin(2*np.pi*291*age)+.3*np.sin(2*np.pi*803*age))*np.exp(-age*75)*(t>=duration*.8)*.5
    save(name,x,.23)
t=clock(.19)
x=band(noise(len(t),15603),500,6200)*np.exp(-t*60)
x+=.3*np.sin(2*np.pi*411*t)*np.exp(-t*35)
save('nozzle_turn',x,.20)
t=clock(.38)
x=band(noise(len(t),15604),250,5400)*np.exp(-t*18)
for f in (217,613,1087):
    x+=.17*np.sin(2*np.pi*f*t)*np.exp(-t*22)
save('nozzle_break',x,.3)
