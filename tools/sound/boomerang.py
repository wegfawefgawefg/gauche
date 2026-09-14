"""A fluttering throw, woody impact, soft catch and dirt landing; generated offline."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.38)
flutter = .35 + .65 * np.sin(2*np.pi*(18*t-11*t*t))**2
save('boomerang_throw', band(noise(len(t),341),400,4300) * flutter * np.sin(np.pi*t/.38)**2, .32)
t = clock(.16)
wood = np.sin(2*np.pi*340*t)*np.exp(-t*35) + .35*np.sin(2*np.pi*870*t)*np.exp(-t*55)
save('boomerang_hit', wood + band(noise(len(t),957),600,5700)*np.exp(-t*48), .4)
t = clock(.19)
save('boomerang_catch', (band(noise(len(t),224),180,1500)*.7 + np.sin(2*np.pi*180*t)*.2)*np.exp(-t*33), .26)
t = clock(.28)
scrape = band(noise(len(t),866),700,3700)*np.exp(-t*13)
save('boomerang_land', scrape + np.sin(2*np.pi*260*t)*np.exp(-t*55)*.25, .25)
