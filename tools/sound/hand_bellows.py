"""Original leather compression, breathy nozzle and collapsing seam."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.46)
x=band(noise(len(t),15561),180,4200)*(1-np.exp(-t*40))*np.exp(-t*9)
x+=.25*np.sin(2*np.pi*(115*t-45*t*t))*np.exp(-t*18)
x+=band(noise(len(t),15562),90,400)*.3*np.exp(-t*11)
save('bellows_puff',x,.24)
t=clock(.54)
x=band(noise(len(t),15563),350,5800)*np.exp(-t*9)
x+=.25*np.sin(2*np.pi*(140*t-80*t*t))*np.exp(-t*15)
save('bellows_spent',x,.19)
