"""Soft listening whine, a doubled warning growl, snap and dying yelp; offline."""
import numpy as np
from synth import clock, noise, band, save

def voice(t, f):
    phase=2*np.pi*np.cumsum(f)/44100
    return np.sin(phase)+.3*np.sin(phase*2)+.13*np.sin(phase*3)

t=clock(.40)
a=voice(t,460+90*np.sin(np.pi*t/.40))*.24+band(noise(len(t),4701),450,2200)*.13
save('echo_listen',a*np.sin(np.pi*t/t[-1])**2,.20)
t=clock(.37)
a=voice(t,115+16*np.sin(2*np.pi*17*t))*.36+band(noise(len(t),4703),180,2300)*.35
env=np.exp(-((t-.07)/.047)**2)+.72*np.exp(-((t-.23)/.065)**2)
save('echo_warn',a*env,.37)
t=clock(.22)
a=voice(t,180+100*np.exp(-t*25))*.26+band(noise(len(t),4705),350,5600)*.5
save('echo_bite',a*np.exp(-t*27),.36)
t=clock(.55)
a=voice(t,420-280*t+.9*np.sin(2*np.pi*11*t))*.4+band(noise(len(t),4707),900,3700)*.14
save('echo_death',a*(1-np.exp(-t*90))*np.exp(-t*7),.36)
