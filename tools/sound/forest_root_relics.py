"""Woody auger rasp and paired-seed spatial chirps, synthesized offline."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.58)
phase=2*np.pi*(80*t+100*t*t)
wood=band(noise(len(t),386),250,2700)
e=np.minimum(1,t/.06)*np.minimum(1,(.58-t)/.13)
save('drill_start',(wood*.6+np.sin(phase)*.2)*(0.6+.4*np.sin(2*np.pi*26*t)**2)*e,.39)
t=clock(.14)
save('drill_bite',band(noise(len(t),931),500,4800)*(0.4+.6*np.sin(2*np.pi*40*t)**2)*np.exp(-t*23),.29)
t=clock(.3)
save('drill_stop',(band(noise(len(t),207),180,1800)*.7+np.sin(2*np.pi*(130*t-110*t*t))*.2)*np.exp(-t*17),.26)
for name,duration,start,change,peak in [('swap_cast',.22,420,1500,.3),('swap_fold',.38,880,-1400,.36),('swap_fade',.2,540,-600,.2)]:
    t=clock(duration)
    phase=2*np.pi*(start*t+change*t*t)
    tone=np.sin(phase)+.3*np.sin(phase*1.51)
    air=band(noise(len(t),int(start)),1100,6800)*.1
    save(name,(tone*.5+air)*np.sin(np.pi*t/duration)**2,peak)
