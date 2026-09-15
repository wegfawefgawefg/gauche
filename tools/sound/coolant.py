"""Quiet finite canister pours, hollow empty-can knock, slick foot skitter."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.65);v=np.zeros(len(t));n=band(noise(len(t),14701),250,2800)
for start,pitch in [(0,210),(.12,175),(.29,245),(.43,160)]:
 q=np.maximum(0,t-start)
 v+=(np.sin(2*np.pi*(pitch*q-100*q*q))+.25*n)*np.exp(-q*22)*(t>=start)
v+=.2*n*np.sin(np.pi*t/.65)**2
save('coolant_pour',v,.26)
t=clock(.34);n=band(noise(len(t),14702),1200,4800)
v=(.6*np.sin(2*np.pi*360*t)+.25*np.sin(2*np.pi*807*t)+.15*n)*np.exp(-t*20)
save('coolant_empty',v*np.minimum(1,t*600),.22)
t=clock(.19);n=band(noise(len(t),14703),1500,6200)
v=n*np.sin(np.pi*t/.19)**2+.25*np.sin(2*np.pi*(630*t-900*t*t))*np.exp(-t*17)
save('coolant_slip',v,.20)
