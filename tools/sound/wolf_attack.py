"""Short rasped warning and airy jaw snap; retain the original howl/bite assets."""
import numpy as np
from synth import RATE,clock,noise,band,save

t=clock(.29)
pitch=110-35*t/.29+8*np.sin(2*np.pi*24*t)
phase=2*np.pi*np.cumsum(pitch)/RATE
throat=np.tanh(2.1*(np.sin(phase)+.3*np.sin(phase*2)))
rasp=band(noise(len(t),721),400,2300)
env=np.minimum(1,t/.016)*np.maximum(0,1-t/.29)**.55
save('wolf_snarl',(throat*.55+rasp*.45)*env,.32)
t=clock(.19)
air=band(noise(len(t),817),1100,7200)
snap=np.exp(-np.maximum(0,t-.026)*95)*(t>=.026)
voice=np.sin(2*np.pi*(190*t-210*t*t))*np.exp(-t*30)
save('wolf_snap',air*(np.sin(np.pi*t/.19)**2*.45+snap*.8)+voice*.12,.36)
