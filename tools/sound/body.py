"""Quiet breathy snores, a rounded swallow and a short optional burp."""
import numpy as np
from synth import RATE,clock,noise,band,save
for name,pitch,seed in [('snore_low',77,3101),('snore_high',123,3102)]:
    t=clock(1.05)
    env=np.sin(np.pi*t/1.05)**1.6
    f=pitch*(1+.11*np.sin(2*np.pi*t/.95))
    phase=2*np.pi*np.cumsum(f)/RATE
    rasp=band(noise(len(t),seed),250,1800)
    voice=np.sin(phase)+.25*np.sin(2*phase)
    flutter=.65+.35*np.sin(2*np.pi*22*t)**2
    save(name,(voice*.4+rasp*.6)*env*flutter,.19)
t=clock(.30)
phase=2*np.pi*(330*t-300*t*t)
voice=np.sin(phase)+.35*np.sin(phase*2)
env=np.exp(-((t-.10)/.041)**2)+.45*np.exp(-((t-.19)/.027)**2)
wet=band(noise(len(t),3103),550,3000)
save('gulp',(voice*.65+wet*.35)*env,.28)
t=clock(.34)
f=105-47*t/.34+9*np.sin(2*np.pi*24*t)
phase=2*np.pi*np.cumsum(f)/RATE
voice=np.tanh(1.6*(np.sin(phase)+.3*np.sin(phase*2)))
env=np.minimum(1,t/.02)*np.maximum(0,1-t/.34)**1.2
save('burp',(voice*.75+band(noise(len(t),3104),300,2000)*.25)*env,.23)
