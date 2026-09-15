"""Offline heavy boot, pneumatic bellow, shield and press-hammer cues."""
import numpy as np
from synth import clock,noise,band,save
for name,seed,length,gain in [('breaker_brace',9511,.3,.25),('breaker_block',9513,.22,.29),('breaker_shove',9515,.3,.3),('breaker_shield_break',9517,.65,.4)]:
 t=clock(length);n=band(noise(len(t),seed),250,5500)
 x=n*np.exp(-24*t)+.3*np.sin(2*np.pi*310*t)*np.exp(-17*t)
 if name=='breaker_shield_break':x+=.5*n*np.exp(-((t-.2)/.1)**2)+.25*np.sin(2*np.pi*1120*t)*np.exp(-7*t)
 save(name,x,gain)
for name,f,length,seed in [('breaker_heave',95,.62,9521),('breaker_alarm',140,.7,9523),('breaker_death',80,.75,9525)]:
 t=clock(length);env=np.sin(np.pi*t/length)**2*np.exp(-2*t)
 phase=2*np.pi*(f*t-20*t*t)
 x=sum(np.sin(h*phase)/h for h in range(1,7))
 x=band(x,220,1900)+.35*band(noise(len(t),seed),180,2400)
 save(name,x*env,.3)
t=clock(.8);x=band(noise(len(t),9531),60,6000)*np.exp(-22*t)
x+=.85*np.sin(2*np.pi*(85*t-14*t*t))*np.exp(-10*t)+.18*np.sin(2*np.pi*870*t)*np.exp(-9*t)
save('breaker_slam',x,.46)
for name,seed in [('breaker_step1',9533),('breaker_step2',9535)]:
 t=clock(.21);x=band(noise(len(t),seed),90,3000)*np.exp(-29*t)+.4*np.sin(2*np.pi*95*t)*np.exp(-26*t)
 save(name,x,.2)
