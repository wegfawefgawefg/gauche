"""Territorial chest rumble, clothy arm sweep and grounded body landings."""
import numpy as np
from synth import clock, noise, band, save, RATE

def throat(seconds,seed,fall=False):
    t=clock(seconds)
    pitch=91+9*np.sin(t*31)-(t*35 if fall else 0)
    phase=2*np.pi*np.cumsum(pitch)/RATE
    voice=np.tanh(2.5*np.sin(phase)+.6*np.sin(phase*2))
    voice=band(voice,100,850)+.35*band(noise(len(t),seed),150,900)
    envelope=(1-np.exp(-t*45))*np.exp(-t*(4 if fall else 1.5))
    return t,voice*envelope

t,a=throat(.58,12111)
a+=band(noise(len(t),12113),180,2400)*np.exp(-((t-.08)/.035)**2)*.6
save('yeti_warn',a,.33)
t,a=throat(.72,12115,True)
save('yeti_death',a,.32)
for name,seconds,seed in [('yeti_throw',.3,12117),('yeti_miss',.23,12119)]:
    t=clock(seconds)
    air=band(noise(len(t),seed),180,2100)*np.sin(np.pi*t/seconds)**1.4
    save(name,air,.28 if name=='yeti_throw' else .20)
for name,seed,peak in [('toss_land',12121,.27),('toss_impact',12123,.37)]:
    t=clock(.29)
    thump=np.sin(2*np.pi*(75*t+1.3*(1-np.exp(-t*27))))*np.exp(-t*20)
    crack=band(noise(len(t),seed),250,3100)*np.exp(-t*31)
    save(name,thump*.7+crack*(1.2 if name=='toss_impact' else .35),peak)
