"""Offline salt crunch and wet leaf chewing; no generic healing cue."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.57)
envelope = sum(np.exp(-((t-center)/width)**2)
               for center,width in [(.07,.014),(.20,.022),(.35,.018),(.48,.027)])
crunch = band(noise(len(t),5571),1800,6200) * envelope
wet = band(noise(len(t),5573),180,1500) * envelope
save('kelp_chew', .5*crunch + wet, .25)
