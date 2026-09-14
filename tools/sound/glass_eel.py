"""Offline electric charge, wet discharge and soft fish flops. Deterministic LFSR sources."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.80)
phase = 2*np.pi*(170*t + 220*t*t)
charge = (.5*np.sin(phase) + .2*np.sin(phase*2.01)) * (.2+.8*t/.8)
charge += band(noise(len(t), 3571), 1300, 6000) * (.05+.14*t/.8)
save('eel_charge', charge * (0.6+0.4*np.sin(2*np.pi*(4*t+5*t*t))**2), .28)

t = clock(.38)
zap = band(noise(len(t), 3573), 600, 6800)*np.exp(-t*16)
zap += .24*np.sin(2*np.pi*(950*t-800*t*t))*np.exp(-t*18)
zap += band(noise(len(t), 3577), 110, 1500)*np.exp(-t*9)*.5
save('eel_discharge', zap, .43)

t = clock(.18)
flop = band(noise(len(t), 3579), 90, 1100)*np.exp(-t*25)
flop += .15*np.sin(2*np.pi*(140*t-130*t*t))*np.exp(-t*30)
save('eel_flop', flop, .16)

t = clock(.45)
death = band(noise(len(t), 3581), 150, 3200)*np.exp(-t*13)
death += .2*np.sin(2*np.pi*(320*t-270*t*t))*np.exp(-t*10)
save('eel_death', death, .29)

t = clock(.20)
save('battery_zap', (band(noise(len(t), 3583), 800, 6600)+.3*np.sin(2*np.pi*870*t))*np.exp(-t*23), .35)
t = clock(.30)
save('battery_empty', (.3*np.sin(2*np.pi*(460*t-620*t*t))+band(noise(len(t), 3587), 200, 1300))*np.exp(-t*16), .21)
