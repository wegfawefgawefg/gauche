"""Offline ice strain, ragged breath, sleeve swing and frostbound death."""
import numpy as np
from synth import clock, noise, band, save

def voice(t, pitch):
    phase=2*np.pi*np.cumsum(pitch)/44100
    return np.sin(phase)+.35*np.sin(2*phase)+.18*np.sin(3*phase)

def ice(name, length, seed, bright, peak):
    t=clock(length); a=np.zeros_like(t)
    for i, start in enumerate(np.linspace(.03,length*.72,5)):
        age=np.maximum(0,t-start)
        a+=(t>=start)*np.exp(-age/.025)*np.sin(2*np.pi*(bright+190*i)*age)*(.9**i)
    a+=band(noise(len(t),seed),700,4800)*np.sin(np.pi*t/length)**2*.15
    save(name,a,peak)

ice('pilgrim_thaw',.72,5101,1250,.26)
ice('pilgrim_freeze',.52,5103,2200,.24)
ice('pilgrim_shell_hit',.16,5105,1500,.30)
t=clock(.5)
a=voice(t,140+90*np.sin(np.pi*t/.5))*.35+band(noise(len(t),5107),200,2300)*.3
save('pilgrim_wake',a*np.sin(np.pi*t/.5)**2,.34)
t=clock(.26)
a=voice(t,150+35*np.sin(2*np.pi*15*t))*.3+band(noise(len(t),5109),400,2500)*.4
save('pilgrim_warn',a*np.sin(np.pi*t/.26)**2,.32)
t=clock(.25)
a=band(noise(len(t),5111),400,5300)*np.exp(-((t-.095)/.047)**2)
save('pilgrim_slash',a,.35)
t=clock(.64)
a=voice(t,180-120*t)*.3+band(noise(len(t),5113),600,4000)*.18
save('pilgrim_death',a*(1-np.exp(-t*60))*np.exp(-t*6),.34)
