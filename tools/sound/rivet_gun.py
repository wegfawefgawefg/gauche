"""Offline compressed-air pops, spent-rivet taps and a mechanical feed reload."""
import numpy as np
from synth import clock, noise, band, save
t=clock(.19)
x=band(noise(len(t),9801),700,6300)*(1-np.exp(-1600*t))*np.exp(-45*t)
x+=.55*np.sin(2*np.pi*(240*t-210*t*t))*np.exp(-55*t)
x+=.28*np.sin(2*np.pi*1370*t)*np.exp(-90*t)
save('rivet_fire',x,.35)
t=clock(.27)
x=.55*band(noise(len(t),9803),900,7000)*np.exp(-120*t)
for f,g in [(1880,.3),(3230,.2),(720,.1)]:x+=g*np.sin(2*np.pi*f*t)*np.exp(-28*t)
save('rivet_impact',x,.24)
t=clock(1.45);x=np.zeros(len(t))
for at,f in [(0,290),(.18,830),(.68,480),(1.16,220),(1.30,1100)]:
 u=np.maximum(t-at,0);e=(t>=at)*np.exp(-55*u)
 x+=(.65*np.sin(2*np.pi*f*u)+.3*band(noise(len(t),9807+int(at*100)),500,5200))*e
u=np.maximum(t-.36,0)
x+=.16*band(noise(len(t),9811),1300,4800)*(t>=.36)*np.exp(-12*u)
save('rivet_reload',x,.24)
t=clock(.37)
x=band(noise(len(t),9813),250,2300)*np.sin(np.pi*t/.37)**2*.4
x+=.35*np.sin(2*np.pi*185*t)*np.exp(-25*t)
save('gunner_brace',x,.22)
t=clock(.58)
x=band(noise(len(t),9817),180,1300)*np.sin(np.pi*t/.58)**2
x*=.5+.5*np.sin(2*np.pi*(95*t-35*t*t))
save('gunner_death',x,.29)
