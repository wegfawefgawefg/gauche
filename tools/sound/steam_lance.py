"""Steam lance: a small boiler, wooden brace, valve and short pressure discharge."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.58)
x=band(noise(len(t),15621),240,3400)*np.sin(np.pi*t/.58)**1.5
x*=.35+.65*np.sin(2*np.pi*(9*t+5*t*t))**2
save('lance_fill',x,.22)
t=clock(.32)
x=band(noise(len(t),15622),700,5500)*np.exp(-t*10)*.3
for f in (473,1231,2017):x+=.25*np.sin(2*np.pi*f*t)*np.exp(-t*18)
save('lance_ready',x,.20)
t=clock(.45)
x=band(noise(len(t),15623),350,3900)*np.exp(-t*12)
save('lance_cool',x,.15)
t=clock(.45)
x=band(noise(len(t),15624),170,1500)*np.exp(-t*38)
x+=band(noise(len(t),15625),1400,5300)*(.1+t)*np.sin(np.pi*t/.45)
x+=.3*np.sin(2*np.pi*197*t)*np.exp(-t*25)
save('lance_windup',x,.23)
t=clock(.6)
env=(1-np.exp(-t*130))*np.exp(-t*9)
x=band(noise(len(t),15626),380,7200)*env
x+=.25*band(noise(len(t),15627),70,330)*env
save('lance_jet',x,.34)
t=clock(.4)
x=band(noise(len(t),15628),240,4200)*np.sin(np.pi*t/.4)**1.2
x*=.5+.5*np.sin(2*np.pi*17*t)**2
save('lance_pour',x,.22)
t=clock(.28)
save('lance_scald',band(noise(len(t),15629),1400,6900)*np.exp(-t*16),.16)
t=clock(.3)
x=band(noise(len(t),15630),600,3500)*np.exp(-t*20)
x+=.15*np.sin(2*np.pi*(600*t-430*t*t))*np.exp(-t*14)
save('lance_splash',x,.16)
