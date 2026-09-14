"""Two breathy whistle notes and a short rising canine answer; offline only."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.67)
whistle = np.zeros(len(t))
for start, length, pitch in [(0, .17, 1670), (.24, .32, 1960)]:
    age = np.clip(t-start, 0, length)
    env = ((t >= start) & (t < start+length))*np.maximum(0, np.sin(np.pi*age/length))**.65
    phase = 2*np.pi*(pitch*age + 13*np.sin(2*np.pi*6*age)/(2*np.pi*6))
    whistle += env*(np.sin(phase)+.07*np.sin(phase*2))
whistle += .12*band(noise(len(t), 719), 1000, 4200)*np.sin(np.pi*t/.67)**2
save('wolf_whistle', whistle, .35)
t = clock(.52)
freq = 290 + 230*np.sin(np.pi*t/.52)**.6
phase = 2*np.pi*np.cumsum(freq)/44100
voice = sum(np.sin(phase*n)/n**1.3 for n in range(1, 7))
voice += .13*band(noise(len(t), 821), 450, 2700)
save('wolf_answer', voice*np.sin(np.pi*t/.52)**1.4, .38)
