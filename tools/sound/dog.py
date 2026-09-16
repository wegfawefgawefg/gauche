"""A short canine warning bark/growl and a dry jaw snap, rendered offline."""
import numpy as np
from synth import RATE,clock,noise,band,save

t=clock(.32)
f=180-70*t/.32+7*np.sin(2*np.pi*27*t)
phase=2*np.pi*np.cumsum(f)/RATE
throat=np.tanh(1.7*(np.sin(phase)+.38*np.sin(phase*2)))
env=np.minimum(1,t/.014)*np.maximum(0,1-t/.32)**.8
save('dog_growl',(throat*.55+band(noise(len(t),1581),650,3300)*.3)*env,.28)
t=clock(.17)
air=band(noise(len(t),1582),1500,7500)
click=np.exp(-np.maximum(0,t-.032)*100)*(t>=.032)
voice=np.sin(2*np.pi*(220*t-240*t*t))*np.exp(-t*26)
save('dog_snap',air*(np.sin(np.pi*t/.17)**2*.3+click*.7)+voice*.15,.31)
