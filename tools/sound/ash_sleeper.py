"""Original dry inhalation, grit swirls and settling ash, not musical notes."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.5)
x=band(noise(len(t),15551),220,3600)*(1-np.exp(-t*10))*np.exp(-t*3)
x*=.6+.4*np.sin(2*np.pi*(15*t+30*t*t))**2
save('ash_rise',x,.19)
t=clock(.48)
x=band(noise(len(t),15552),370,2600)*np.sin(np.pi*t/.48)**2
x+=band(noise(len(t),15553),80,300)*.3*np.sin(np.pi*t/.48)**2
save('ash_inhale',x,.24)
t=clock(.26)
x=band(noise(len(t),15554),500,4200)*np.exp(-t*18)*(1-np.exp(-t*100))
x+=.25*np.sin(2*np.pi*78*t)*np.exp(-t*30)
save('ash_swipe',x,.25)
t=clock(.5)
x=band(noise(len(t),15555),950,5000)*np.exp(-t*8)*(.5+.5*np.sin(2*np.pi*37*t)**2)
save('ash_settle',x,.15)
t=clock(.6)
x=band(noise(len(t),15556),160,4200)*np.exp(-t*9)
x*=.4+.6*np.sin(2*np.pi*(45*t-20*t*t))**2
save('ash_death',x,.23)
