"""Original small hand-pump suction and wet discharge cues."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.4)
pull=np.sin(np.pi*t/.4)**2
x=band(noise(len(t),15581),280,1700)*pull*(.3+.7*np.sin(2*np.pi*17*t)**2)
x+=.28*np.sin(2*np.pi*(180*t-100*t*t))*pull
x+=.2*band(noise(len(t),15582),1700,4200)*np.exp(-t*70)
save('pump_draw',x,.22)
t=clock(.42)
x=band(noise(len(t),15583),350,4100)*(1-np.exp(-t*75))*np.exp(-t*7)
for offset,f in ((.04,173),(.12,251),(.21,131)):
    age=np.maximum(t-offset,0)
    x+=.2*np.sin(2*np.pi*(f*age+120*age*age))*np.exp(-age*30)*(t>=offset)
save('pump_pour',x,.23)
