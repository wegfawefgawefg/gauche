"""Offline breathy mutters, matchwork, committed flame casts and cabinet wood."""
import numpy as np
from synth import clock, noise, band, save

# VOICE: Short coarse breath, with separate envelopes for scolding and warning.
for name,seconds,seed,pitch,peak in [
    ('keeper_scold',.56,5741,180,.3),('keeper_warn',.48,5743,130,.28),
    ('keeper_death',.65,5745,95,.3)]:
    t=clock(seconds)
    env=np.sin(np.pi*t/seconds)**2
    phase=2*np.pi*np.cumsum(pitch+18*np.sin(t*23))/44100
    voice=(np.sin(phase)+.2*np.sin(phase*3))*env
    voice+=band(noise(len(t),seed),600,3200)*env*.7
    if name=='keeper_scold': voice*=.4+.6*np.sin(t*18)**2
    save(name,voice,peak)
t=clock(.3)
a=band(noise(len(t),5747),1700,6500)*np.exp(-t*24)
a+=band(noise(len(t),5749),250,1400)*np.exp(-((t-.1)/.08)**2)*.6
save('keeper_tend',a,.2)
t=clock(.4)
a=band(noise(len(t),5751),140,3400)*(1-np.exp(-t*50))*np.exp(-t*12)
a+=.15*np.sin(2*np.pi*(95*t-45*t*t))*np.exp(-t*16)
save('keeper_cast',a,.36)
t=clock(.26)
a=band(noise(len(t),5753),950,5300)*np.exp(-t*18)
save('keeper_flame',a,.17)
t=clock(.46)
a=band(noise(len(t),5755),1100,6200)*(1-np.exp(-t*80))*np.exp(-t*10)
save('keeper_douse',a,.23)
t=clock(.32)
a=band(noise(len(t),5757),250,2300)*np.exp(-((t-.09)/.08)**2)
a+=band(noise(len(t),5759),3000,7200)*np.exp(-t*65)*.5
save('keeper_relight',a,.2)
t=clock(.47)
a=band(noise(len(t),5761),250,2700)*np.exp(-t*32)
for at,pitch in [(.0,220),(.07,390),(.19,180)]:
    age=np.maximum(0,t-at)
    a+=.3*np.sin(2*np.pi*pitch*age)*np.exp(-age*31)*(t>=at)
save('cabinet_open',a,.3)
