"""Original short plank swish, set-down, nail contact and split sounds."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.22)
x=band(noise(len(t),15541),350,3200)*np.sin(np.pi*t/.22)**2
x+=.1*np.sin(2*np.pi*(150*t+90*t*t))*np.exp(-t*20)
save('nail_swing',x,.17)
t=clock(.26)
x=band(noise(len(t),15542),180,1700)*np.exp(-t*35)
x+=.5*np.sin(2*np.pi*180*t)*np.exp(-t*27)+.16*np.sin(2*np.pi*1170*t)*np.exp(-t*43)
save('nail_set',x,.20)
t=clock(.18)
x=band(noise(len(t),15543),600,3800)*np.exp(-t*56)
x+=.7*np.sin(2*np.pi*125*t)*np.exp(-t*31)+.13*np.sin(2*np.pi*2100*t)*np.exp(-t*32)
save('nail_step',x,.26)
t=clock(.42)
x=band(noise(len(t),15544),170,4800)*np.exp(-t*29)
for start,freq in [(.025,240),(.09,1370),(.18,970),(.27,2050)]:
 u=np.maximum(t-start,0);x+=(t>=start)*.20*np.sin(2*np.pi*freq*u)*np.exp(-u*44)
save('nail_break',x,.23)
