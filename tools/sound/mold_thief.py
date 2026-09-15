"""Original casting-room tongs, lid, lock, dropped gear and worker exertion."""
import numpy as np
from synth import clock, noise, band, save

for name,duration,seed,freq,peak in [
    ('mold_tongs',.28,15611,737,.22),('mold_snatch',.23,15612,391,.24),
    ('mold_seal',.47,15613,93,.35),('mold_spill',.55,15614,251,.27),
    ('mold_rattle',.3,15615,413,.2),('mold_break',.8,15616,119,.38)]:
    t=clock(duration)
    x=band(noise(len(t),seed),180,5900)*np.exp(-t*21)
    x+=(np.sin(2*np.pi*freq*t)+.45*np.sin(2*np.pi*freq*2.137*t))*np.exp(-t*13)*.5
    if name in ('mold_spill','mold_break'):
        for offset in (.07,.16,.23):
            age=np.maximum(0,t-offset)
            x+=.3*np.sin(2*np.pi*freq*1.79*age)*np.exp(-age*42)*(t>=offset)
    save(name,x,peak)
t=clock(.65)
x=band(noise(len(t),15617),320,2200)*np.sin(np.pi*t/.65)**.6*(.1+.9*np.sin(2*np.pi*14*t)**12)
x+=.35*np.sin(2*np.pi*(83*t+31*t*t))*np.sin(np.pi*t/.65)**2
save('mold_crank',x,.24)
t=clock(.44)
x=band(noise(len(t),15618),450,3800)*np.sin(np.pi*t/.44)**2
x+=.4*np.sin(2*np.pi*173*t)*np.exp(-t*35)
save('mold_unlock',x,.23)
t=clock(.5)
x=np.sin(2*np.pi*(125*t-58*t*t))*(.5+.5*np.sin(2*np.pi*22*t))
x=(x+band(noise(len(t),15619),250,1900)*.7)*np.sin(np.pi*t/.5)**1.3
save('mold_thief_death',x,.26)
t=clock(.25)
x=band(noise(len(t),15620),800,6800)*np.exp(-t*38)+.4*np.sin(2*np.pi*651*t)*np.exp(-t*23)
save('mold_key_spent',x,.22)
