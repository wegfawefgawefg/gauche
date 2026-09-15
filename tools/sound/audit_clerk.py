"""Offline paper, rubber stamp, clerk gasp/bell, and the pay cage's actual metal."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.4);x=band(noise(len(t),15481),1200,6000)*np.exp(-t*7)*(.2+.8*np.sin(t*35)**4)
save('clerk_paper',x,.14)
t=clock(.25);x=np.sin(2*np.pi*143*t)*np.exp(-t*32)+band(noise(len(t),15482),270,2300)*np.exp(-t*50)
save('clerk_stamp',x,.25)
t=clock(.36);x=band(noise(len(t),15483),350,1900)*(1-np.exp(-t*35))*np.exp(-t*10)
x+=.2*np.sin(2*np.pi*(170*t+130*t*t))*np.exp(-t*6)
save('clerk_gasp',x,.24)
t=clock(.7);x=np.zeros(len(t))
for start in [0,.17,.34]:
 u=np.maximum(0,t-start)
 x+=(t>=start)*(np.sin(2*np.pi*947*u)+.3*np.sin(2*np.pi*2339*u))*np.exp(-u*13)
save('clerk_alarm',x,.25)
t=clock(.45);x=band(noise(len(t),15484),180,2200)*np.exp(-t*11)+.3*np.sin(2*np.pi*(230*t-135*t*t))*np.exp(-t*7)
save('clerk_death',x,.22)
t=clock(.34);x=np.sin(2*np.pi*377*t)*np.exp(-t*17)+.25*np.sin(2*np.pi*881*t)*np.exp(-t*26)
x+=band(noise(len(t),15485),500,3400)*np.exp(-t*26)
save('pay_rattle',x,.24)
t=clock(.45);x=np.zeros(len(t))
for i,start in enumerate([0,.04,.09,.16,.25]):
 u=np.maximum(0,t-start);x+=(t>=start)*np.sin(2*np.pi*(1231+i*171)*u)*np.exp(-u*40)
save('pay_deposit',x,.20)
t=clock(.65);x=band(noise(len(t),15486),180,3700)*np.exp(-t*12)
x+=.5*np.sin(2*np.pi*131*t)*np.exp(-t*6)+.3*np.sin(2*np.pi*569*t)*np.exp(-t*14)
save('pay_break',x,.32)
