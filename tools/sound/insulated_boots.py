"""Offline rubber seals, two soft squeaks and diverted electrical fizz."""
import numpy as np
from synth import clock, noise, band, save
for name,length,seed,base,peak in [('insulator_fit',.42,15671,270,.22),('insulator_step1',.16,15672,580,.10),('insulator_step2',.18,15673,490,.10),('insulator_release',.32,15674,220,.15),('insulator_spent',.46,15675,170,.21)]:
    t=clock(length);u=t/length
    env=np.sin(np.pi*u)**2
    squeak=np.sin(2*np.pi*(base*t+80*t*t/length))*env
    rub=band(noise(len(t),seed),180,2400)*env
    thud=np.sin(2*np.pi*92*t)*np.exp(-t*34)
    save(name,.45*squeak+.3*rub+.5*thud,peak)
t=clock(.23)
x=band(noise(len(t),15676),1800,7500)*np.exp(-t*21)
x+=.25*np.sin(2*np.pi*1850*t)*np.exp(-t*32)
save('insulator_arc',x,.20)
