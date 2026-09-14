"""Offline crank, gathering charge, optical report and a smashed lamp."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.42)
save('warden_turn',(band(noise(len(t),4501),350,2500)*.55+.13*np.sin(2*np.pi*570*t))*(.2+.8*np.sin(2*np.pi*9*t)**2)*np.sin(np.pi*t/.42),.27)
t=clock(.70)
save('warden_charge',(.24*np.sin(2*np.pi*(410*t+550*t*t))+.1*np.sin(2*np.pi*(1221*t+1200*t*t))+band(noise(len(t),4503),2400,4900)*.12)*np.sin(np.pi*t/.7),.34)
t=clock(.32)
save('warden_fire',(band(noise(len(t),4505),600,6500)*.4+.3*np.sin(2*np.pi*(1630*t-1800*t*t))+.15*np.sin(2*np.pi*3971*t))*np.exp(-t*17),.45)
t=clock(.50)
save('warden_death',(band(noise(len(t),4507),180,2500)*.6+.18*np.sin(2*np.pi*(210*t-130*t*t)))*np.exp(-t*12),.37)
t=clock(.48)
a=band(noise(len(t),4509),1400,7800)*np.exp(-t*22)
for i,f in enumerate([763,1759,2819]):
 s=np.maximum(0,t-.05*i); a+=(t>=.05*i)*.2*np.sin(2*np.pi*f*s)*np.exp(-s*20)
save('lamp_break',a,.44)
