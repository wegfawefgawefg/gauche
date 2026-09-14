"""Offline earthy rumble, throat opening and short fibrous separation sounds."""
import numpy as np
from synth import RATE, clock, band, noise, save

t=clock(.40)
phase=2*np.pi*(100*t+100*t*t)
save('worm_warning',(np.sin(phase)+.25*np.sin(phase*3)+band(noise(len(t),821),250,1800)*.65)*np.sin(np.pi*t/.4)**.7,.34)
t=clock(.17)
save('worm_bite',(np.sin(2*np.pi*140*t)*.7+band(noise(len(t),817),700,5900))*np.exp(-t*25),.4)
t=clock(.6)
rumble=band(noise(len(t),611),70,1800)*(.45+.55*np.sin(2*np.pi*13*t)**2)
save('worm_burrow',(rumble+np.sin(2*np.pi*72*t)*.3)*np.sin(np.pi*t/.6)**.6,.37)
t=clock(.28)
save('worm_split',(band(noise(len(t),239),300,3300)+.25*np.sin(2*np.pi*(150*t-90*t*t)))*np.exp(-t*16),.38)
