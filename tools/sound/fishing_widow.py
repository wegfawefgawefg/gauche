"""Offline breath, bending rod, wet hook bite, strained reel and fish chewing."""
import numpy as np
from synth import clock, noise, band, save

def hum(t,pitch):
    phase=2*np.pi*np.cumsum(pitch)/44100
    return np.sin(phase)+.28*np.sin(phase*2)+.15*np.sin(phase*3)

t=clock(.54)
a=hum(t,220+65*np.sin(np.pi*t/.54))*.3+band(noise(len(t),5301),600,2800)*.2
save('widow_warn',a*np.sin(np.pi*t/.54)**2,.31)
t=clock(.25)
a=band(noise(len(t),5303),600,5500)*np.exp(-((t-.08)/.045)**2)
save('widow_cast',a,.30)
t=clock(.21)
a=band(noise(len(t),5305),160,3200)*np.exp(-t/.04)+np.sin(2*np.pi*900*t)*np.exp(-t/.016)*.15
save('widow_latch',a,.34)
t=clock(.13)
a=hum(t,390+120*np.sin(np.pi*t/.13))*.2+band(noise(len(t),5307),400,2200)*.3
save('widow_reel',a*np.sin(np.pi*t/.13)**2,.21)
t=clock(.24)
a=hum(t,260+450*np.exp(-t*30))*np.exp(-t*22)*.4+band(noise(len(t),5309),700,5800)*np.exp(-t*35)
save('widow_snap',a,.30)
t=clock(.52)
a=band(noise(len(t),5311),300,2200)*(np.exp(-((t-.10)/.06)**2)+.7*np.exp(-((t-.32)/.09)**2))
save('widow_tangle',a,.22)
t=clock(.68)
a=hum(t,270-150*t)*.36+band(noise(len(t),5313),650,3300)*.2
save('widow_death',a*(1-np.exp(-t*80))*np.exp(-t*5),.35)
for name,seed,duration,peak in [('fish_eat',5315,.55,.26),('fish_nibble',5317,.42,.22)]:
    t=clock(duration);env=np.zeros_like(t)
    for center in [duration*.15,duration*.42,duration*.73]:
        env+=np.exp(-((t-center)/.035)**2)
    a=band(noise(len(t),seed),180,2800)*env
    a+=np.sin(2*np.pi*145*t)*env*.06
    save(name,a,peak)
