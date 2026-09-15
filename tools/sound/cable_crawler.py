"""Cable-machine relay buildup, electric snap, claw steps and broken spool."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.8);x=np.zeros(len(t));n=band(noise(len(t),15201),1100,4100)
for start in [.02,.23,.41,.55,.65,.73]:
 a=np.maximum(0,t-start);x+=(t>=start)*np.exp(-a*90)*(.3*n+np.sin(2*np.pi*731*a))
x+=.15*band(noise(len(t),15202),180,1100)*(t/.8)**2
save('crawler_charge',x,.32)
t=clock(.23);n=band(noise(len(t),15203),700,6600)
x=n*np.exp(-t*22)+.45*np.sin(2*np.pi*93*t)*np.exp(-t*20)
save('crawler_pulse',x,.34)
t=clock(.2);x=np.zeros(len(t));n=band(noise(len(t),15204),600,3200)
for start in [0,.061,.125]:
 a=np.maximum(0,t-start);x+=(t>=start)*np.exp(-a*95)*(.3*n+np.sin(2*np.pi*357*a))
save('crawler_step',x,.16)
t=clock(.85);n=band(noise(len(t),15205),170,3600)
x=.6*n*np.exp(-t*10)+np.sin(2*np.pi*(210*t-65*t*t))*np.exp(-t*7)
for start in [.24,.43,.68]:
 a=np.maximum(0,t-start);x+=(t>=start)*np.sin(2*np.pi*827*a)*np.exp(-a*45)*.4
save('crawler_death',x,.34)
