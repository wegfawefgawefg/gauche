"""Offline drill motor, loaded bit, free spin and exhausted battery."""
import numpy as np
from scipy import signal
from synth import clock, noise, band, save

def motor(t,start,end):
 f=start+(end-start)*t/max(t[-1],.001)
 phase=2*np.pi*np.cumsum(f)/44100
 return .6*np.sin(phase)+.18*signal.sawtooth(phase*3,width=.65)

t=clock(.4);x=motor(t,55,173)*(1-np.exp(-t*30))
x+=.15*band(noise(len(t),15301),300,2800)*np.minimum(1,t*10)
save('pocket_drill_start',x,.23)
t=clock(.2);n=band(noise(len(t),15302),180,3500)
x=motor(t,139,147)*.7+n*(.3+.7*np.sin(2*np.pi*47*t)**8)
save('pocket_drill_work',x,.26)
t=clock(.2);x=motor(t,177,178)+.06*band(noise(len(t),15303),1100,4400)
save('pocket_drill_air',x,.13)
t=clock(.4);x=motor(t,155,24)*np.exp(-t*5)
x+=.15*band(noise(len(t),15304),350,1800)*np.exp(-t*15)
save('pocket_drill_empty',x,.20)
