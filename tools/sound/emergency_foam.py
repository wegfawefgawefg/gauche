"""Original offline pressurized foam hiss, wet expansion and soft tearing."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.26)
x=band(noise(len(t),15491),800,4800)*np.sin(np.pi*t/.26)**2
x+=.25*np.sin(2*np.pi*331*t)*np.exp(-t*35)
save('foam_throw',x,.22)
t=clock(1.0)
x=band(noise(len(t),15492),1600,6500)*(1-np.exp(-t*28))*np.exp(-t*2)
x+=.35*np.sin(2*np.pi*217*t)*np.exp(-t*36)
save('foam_land',x,.19)
t=clock(.65)
x=band(noise(len(t),15493),220,2200)*(1-np.exp(-t*30))*np.exp(-t*7)
x*=.65+.35*np.sin(2*np.pi*(21*t+18*t*t))**2
x+=.25*np.sin(2*np.pi*(160*t-65*t*t))*np.exp(-t*12)
save('foam_expand',x,.30)
t=clock(.45)
x=band(noise(len(t),15494),1900,7800)*(1-np.exp(-t*55))*np.exp(-t*9)
save('foam_douse',x,.17)
t=clock(.2)
x=band(noise(len(t),15495),300,2900)*np.exp(-t*24)
x*=.35+.65*np.sin(2*np.pi*47*t)**2
save('foam_tear',x,.24)
t=clock(.48)
x=band(noise(len(t),15496),140,1700)*np.exp(-t*10)
x+=.22*np.sin(2*np.pi*(103*t-65*t*t))*np.exp(-t*14)
save('foam_collapse',x,.23)
