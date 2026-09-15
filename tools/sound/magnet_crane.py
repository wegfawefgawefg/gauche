"""Gantry motor, rising coil, heavy grab, metal collapse; offline mono SFX."""
import numpy as np
from synth import clock,noise,band,save,RATE

t=clock(.5);phase=2*np.pi*np.cumsum(62+25*np.sin(np.pi*t/.5))/RATE
motor=band(np.tanh(np.sin(phase)*2.5)+.2*noise(len(t),14501),100,2400)
save('crane_slew',motor*np.sin(np.pi*t/.5)**2,.27)
t=clock(.75);phase=2*np.pi*np.cumsum(95+140*t*t)/RATE
coil=(np.sin(phase)+.4*np.sin(phase*2.03)+.1*np.sin(phase*5.07))*(.15+.7*t/.75)
save('crane_coil',coil*np.minimum(1,t*50)*np.minimum(1,(.75-t)*80),.25)
t=clock(.28);a=.4*np.sin(2*np.pi*76*t)*np.exp(-t*19)
a+=band(noise(len(t),14509),400,4300)*np.exp(-t*31)
save('crane_tug',a,.32)
t=clock(.9);a=np.zeros(len(t));n=band(noise(len(t),14511),140,3500)
for at,pitch in [(0,110),(.17,340),(.31,180),(.48,590)]:
    q=np.maximum(0,t-at);a+=(.25*np.sin(2*np.pi*pitch*q)+.2*n)*np.exp(-q*13)*(t>=at)
save('crane_death',a,.36)
