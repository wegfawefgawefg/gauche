"""Three gurgling chest voices plus separate wet cough/recovery/death gestures."""
import numpy as np
from synth import clock, noise, band, save

for voice, freq, length, seed in [('low',92,.78,15641),('mid',109.4,.98,15642),('high',130.1,1.18,15643)]:
    t=clock(length)
    # Uneven bubbles modulate a dark chest tone: thirds without a sung melody.
    phase=2*np.pi*freq*t+.6*np.sin(2*np.pi*7.3*t)
    pulse=.48+.52*np.sin(2*np.pi*(5*t+2*t*t))**4
    x=(np.sin(phase)+.25*np.sin(2.03*phase))*.42*pulse
    x+=band(noise(len(t),seed),180,1500)*pulse*.32
    x*=np.sin(np.pi*t/length)**.75
    save('choir_'+voice,x,.16)
t=clock(.33)
x=band(noise(len(t),15644),170,2400)*(1-np.exp(-t*95))*np.exp(-t*16)
x+=.4*np.sin(2*np.pi*(180*t-150*t*t))*np.exp(-t*21)
save('choir_cough',x,.30)
t=clock(.44)
x=.5*np.sin(2*np.pi*(120*t-58*t*t))*np.exp(-t*8)
x+=band(noise(len(t),15645),280,2100)*np.exp(-t*15)*np.sin(2*np.pi*13*t)**4
save('choir_break',x,.22)
t=clock(.65)
x=band(noise(len(t),15646),110,1800)*np.exp(-t*7)
x+=.6*np.sin(2*np.pi*(90*t-40*t*t))*np.exp(-t*9)
x+=.16*band(noise(len(t),15647),1700,4200)*np.exp(-t*35)
save('choir_death',x,.30)
