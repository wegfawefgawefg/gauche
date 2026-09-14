"""Folded wood, resonant linked thresholds, airy passage and a muted refusal."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.32)
wood=band(noise(len(t),1853),140,2200)*np.exp(-t*28)
wood+=.4*np.sin(2*np.pi*186*t)*np.exp(-t*23)
save('pocket_place',wood,.32)
t=clock(.68)
link=np.zeros(len(t))
for frequency,gain in [(430,.7),(645,.25),(1009,.12)]:
    link+=gain*np.sin(2*np.pi*frequency*t)*np.exp(-t*6)
link+=.14*band(noise(len(t),1913),1700,5300)*np.sin(np.pi*t/.68)**2
save('pocket_link',link,.32)
t=clock(.38)
air=band(noise(len(t),1919),330,4200)*np.sin(np.pi*t/.38)**2
phase=2*np.pi*(780*t-650*t*t)
air+=.27*np.sin(phase)*np.sin(np.pi*t/.38)**2
save('pocket_travel',air,.36)
t=clock(.16)
knock=band(noise(len(t),1931),120,1300)*np.exp(-t*38)
knock+=.4*np.sin(2*np.pi*108*t)*np.exp(-t*28)
save('pocket_blocked',knock,.23)
