"""Heavy visor hinge and a muffled worker grunt; arcs share the player's torch sounds."""
import numpy as np
from synth import clock,noise,band,save,RATE

t=clock(.52)
a=band(noise(len(t),12501),240,1600)*np.exp(-((t-.09)/.065)**2)
for at,pitch in [(.025,820),(.27,330)]:
    q=np.maximum(0,t-at);a+=.23*np.sin(2*np.pi*pitch*q)*np.exp(-q*32)*(t>=at)
a+=.12*np.sin(2*np.pi*96*t)*np.sin(np.pi*t/.52)**2
save('welder_mask',a,.29)
t=clock(.54)
phase=2*np.pi*np.cumsum(105-30*t+7*np.sin(t*38))/RATE
voice=band(np.tanh(np.sin(phase)*3)+.25*noise(len(t),12503),160,1150)
a=voice*np.sin(np.pi*t/.54)**2
save('welder_death',a,.30)
