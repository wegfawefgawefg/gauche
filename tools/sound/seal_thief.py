"""Offline throaty seal calls, wet snatches and woven-basket impacts."""
import numpy as np
from synth import clock, noise, band, save


def throat(t, start, end):
    phase = 2*np.pi*np.cumsum(start+(end-start)*t/t[-1])/44100
    return np.sin(phase)+.4*np.sin(2*phase)+.25*np.sin(3*phase)


t = clock(.22)
a = band(noise(len(t),5401),240,3600)*np.exp(-((t-.07)/.04)**2)
a += throat(t,300,100)*.12*np.exp(-t*22)
save('seal_snatch',a,.26)
t = clock(.48)
a = throat(t,150,85)*.24+band(noise(len(t),5403),350,1800)*.12
save('seal_settle',a*np.sin(np.pi*t/.48)**2,.20)
t = clock(.46)
envelope = np.exp(-((t-.1)/.07)**2)+.7*np.exp(-((t-.31)/.06)**2)
a = throat(t,210,140)*.5+band(noise(len(t),5405),500,3500)*.27
save('seal_bark',a*envelope,.36)
t = clock(.20)
a = throat(t,220,95)*.2*np.exp(-t*20)+band(noise(len(t),5407),180,3300)*np.exp(-t*32)
save('seal_bite',a,.30)
t = clock(.58)
env = sum(np.exp(-((t-center)/.04)**2) for center in [.1,.28,.46])
a = band(noise(len(t),5409),180,2500)*env
save('seal_eat',a,.25)
t = clock(.66)
a = throat(t,230,60)*.45+band(noise(len(t),5411),300,2100)*.2
save('seal_death',a*(1-np.exp(-t*80))*np.exp(-t*5),.32)
for name,duration,seed,peak in [('creel_hit',.19,5413,.25),('creel_break',.47,5415,.32)]:
    t = clock(duration)
    a = band(noise(len(t),seed),650,6500)*np.exp(-t*15)
    a += np.sin(2*np.pi*180*t)*np.exp(-t*35)*.13
    if name == 'creel_break':
        for center in [.08,.14,.25]:
            a += band(noise(len(t),seed+int(center*100)),700,4200)*np.exp(-((t-center)/.014)**2)*.35
    save(name,a,peak)
