"""Rubber boiler-bladder inflation, steam dash, rupture and safe death vent."""
import numpy as np
from synth import clock,noise,band,save,RATE

t=clock(.6);phase=2*np.pi*np.cumsum(310+530*(t/.6)**1.6)/RATE
v=(np.sin(phase)+.3*np.sin(phase*2.01))*(.25+.75*t/.6)
v+=.3*band(noise(len(t),14601),650,5400)
save('rat_inflate',v*np.minimum(1,t*35)*np.minimum(1,(.6-t)*70),.25)
t=clock(.3);v=band(noise(len(t),14602),450,7800)*np.exp(-t*9)
v+=.25*np.sin(2*np.pi*(460*t-430*t*t))*np.exp(-t*12)
save('rat_rush',v*np.minimum(1,t*180),.28)
t=clock(.55);v=band(noise(len(t),14603),160,8000)*np.exp(-t*9)
v+=.5*np.sin(2*np.pi*86*t)*np.exp(-t*30)
save('rat_burst',v*np.minimum(1,t*600),.34)
t=clock(.25);phase=2*np.pi*(900*t-1100*t*t)
v=.3*np.sin(phase)*np.exp(-t*20)+band(noise(len(t),14604),1400,6000)*np.exp(-t*17)
save('rat_death',v*np.minimum(1,t*300),.23)
