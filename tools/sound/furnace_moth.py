"""Offline papery wings, a hot breath and a small collapsing ash insect."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.50)
x=band(noise(len(t),15501),950,4600)*(1-np.exp(-t*25))*np.exp(-t*5)
x*=.3+.7*np.sin(2*np.pi*27*t)**4
save('furnace_moth_sip',x,.17)
t=clock(.36)
x=band(noise(len(t),15502),250,1900)*np.exp(-t*10)
x+=.25*np.sin(2*np.pi*(260*t+260*t*t))*np.exp(-t*11)
save('furnace_moth_feed',x,.22)
t=clock(.75)
x=band(noise(len(t),15503),1200,6200)*(.2+.8*np.sin(2*np.pi*(17*t+11*t*t))**4)
x*=np.sin(np.pi*t/.75)**.7
x+=.12*np.sin(2*np.pi*(690*t+320*t*t))*np.sin(np.pi*t/.75)
save('furnace_moth_warn',x,.25)
t=clock(.3)
x=band(noise(len(t),15504),550,5400)*np.sin(np.pi*t/.3)**2
save('furnace_moth_dive',x,.25)
t=clock(.42)
x=band(noise(len(t),15505),160,4200)*np.exp(-t*12)
x+=.3*np.sin(2*np.pi*137*t)*np.exp(-t*26)
save('furnace_moth_flare',x,.29)
t=clock(.45)
x=band(noise(len(t),15506),500,3200)*np.exp(-t*15)*(.3+.7*np.sin(2*np.pi*33*t)**2)
save('furnace_moth_death',x,.20)
