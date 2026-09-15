"""Offline ratcheting hydraulic gates and a wedged metal stop, without UI beeps."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.22)
save('wedge_fit',band(noise(len(t),1601),420,4700)*np.exp(-t*31)+
     .3*np.sin(2*np.pi*430*t)*np.exp(-t*21),.29)
t=clock(.26)
save('wedge_lift',band(noise(len(t),1607),800,3900)*np.sin(np.pi*t/.26)**2*.6+
     .3*np.sin(2*np.pi*620*t)*np.exp(-t*32),.22)
t=clock(.22)
ring=sum(np.sin(2*np.pi*f*t)*np.exp(-t*r)/a for f,r,a in [(670,22,1),(1381,35,2),(2410,48,4)])
save('wedge_hit',ring+band(noise(len(t),1609),900,5900)*np.exp(-t*49),.28)
t=clock(.34)
save('wedge_break',band(noise(len(t),1613),600,6600)*np.exp(-t*19)+
     .3*np.sin(2*np.pi*1220*t)*np.exp(-t*20),.38)
t=clock(.31)
save('wedge_strain',band(noise(len(t),1619),500,3100)*np.sin(np.pi*t/.31)**2+
     .15*np.sin(2*np.pi*(190*t+16*t*t)),.17)
for name,seed,closing in [('sluice_open',1621,False),('sluice_close',1627,True)]:
 t=clock(.57)
 rasp=band(noise(len(t),seed),180,3700)*np.sin(np.pi*t/.57)**2
 chatter=.35+.65*(.5+.5*np.sin(2*np.pi*38*t))**5
 end=np.exp(-np.maximum(t-.44,0)*35)*(t>=.44)
 save(name,rasp*chatter+.5*band(noise(len(t),seed+2),110,1300)*end,.38 if closing else .32)
t=clock(.33)
env=sum(np.exp(-np.maximum(t-s,0)*50)*(t>=s) for s in [0,.14])
save('sluice_warning',band(noise(len(t),1637),750,3300)*env+
     .4*np.sin(2*np.pi*410*t)*env,.23)
