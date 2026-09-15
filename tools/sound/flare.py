"""Small pyrotechnic launch, dry burning hiss and wet extinguishing."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.35);n=noise(len(t),1301)
pop=band(n,180,5400)*np.exp(-t*45)
whoosh=band(n,900,7000)*np.sin(np.pi*np.minimum(t/.28,1))**2*.6
save('flare_launch',pop+whoosh+.2*np.sin(2*np.pi*94*t)*np.exp(-t*25),.46)
t=clock(.25)
save('flare_land',band(noise(len(t),1303),500,4000)*np.exp(-t*31)+.2*np.sin(2*np.pi*460*t)*np.exp(-t*25),.27)
t=clock(1.2);n=band(noise(len(t),1307),1700,8500)
save('flare_hiss',n*np.sin(np.pi*t/1.2)**.7*(.7+.3*np.sin(t*31)**8),.14)
t=clock(.8);n=band(noise(len(t),1319),1000,7400)
save('flare_out',n*np.exp(-t*5)*(1-np.exp(-t*100)),.3)
t=clock(.45)
save('flare_spent',band(noise(len(t),1321),700,5200)*np.exp(-t*10),.17)
