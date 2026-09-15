"""A short-short-long borrowed work call, acknowledgement, and cracked last note."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.68)
env = sum(np.exp(-((t-at)/width)**4) for at, width in
          [(.075, .043), (.22, .043), (.46, .13)])
phase = 2*np.pi*(1680*t + 1.6*np.sin(2*np.pi*19*t))
save('work_call', (np.sin(phase)+.09*band(noise(len(t), 9171), 1700, 4000))*env, .24)
t = clock(.3)
phase = 2*np.pi*(125*t+35*t*t)
voice = band(np.sin(phase)+.4*np.sin(2*phase)+.2*np.sin(3*phase), 110, 1500)
save('work_answer', voice*np.exp(-((t-.13)/.07)**2), .23)
t = clock(.24)
save('work_spent', (.4*np.sin(2*np.pi*(1600*t-1000*t*t)) +
                   band(noise(len(t), 9173), 900, 4500))*np.exp(-20*t), .2)
