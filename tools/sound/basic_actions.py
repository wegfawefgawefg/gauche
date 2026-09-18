"""Hand action signatures: skin slap, boot thud, cloth grab, pan and airy hop."""
import sys
import numpy as np
from synth import clock, noise, band, save
for i,(name,seconds,low,high,body,decay,peak) in enumerate([
    ('slap_hit',.22,1200,7500,290,48,.78),
    ('jump_lift',.22,450,2600,510,17,.26),
    ('jump_land',.17,100,1500,120,32,.31),
    ('grapple_lift',.25,500,3100,180,22,.32),
    ('grapple_throw',.31,250,2200,340,15,.34),
    ('shove_hit',.21,160,1700,140,26,.36),
    ('kick_hit',.21,130,3200,88,25,.5),
    ('elbow_hit',.12,300,3400,175,42,.44),
    ('god_impact',.58,100,6500,62,12,.65),
    ('balloon_pop',.14,1000,9000,340,55,.45),
    ('power_dodge',.16,800,3800,620,25,.24),
]):
    if len(sys.argv)>1 and name not in sys.argv[1:]: continue
    t=clock(seconds)
    if name=='slap_hit':
        # Broad skin crack, a second palm contact, then a short low thump.
        rng=np.random.default_rng(1170)
        crack=band(rng.uniform(-1,1,len(t)),900,14000)
        palm=band(rng.uniform(-1,1,len(t)),250,5200)
        attack=np.clip(t/.001,0,1)*np.exp(-np.maximum(0,t-.003)*140)
        second=np.maximum(0,t-.011)
        samples=2.2*crack*attack
        samples+=1.1*palm*(t>=.011)*np.exp(-second*85)
        samples+=.35*np.sin(2*np.pi*155*t)*np.exp(-t*75)
        samples+=.13*palm*np.exp(-t*25)
        save(name,np.tanh(samples*1.7),peak)
        continue
    contour=np.exp(-t*decay)
    air=name in ('jump_lift','grapple_throw','power_dodge')
    if air: contour=np.sin(np.pi*t/seconds)**2
    samples=band(noise(len(t),1170+i*41),low,high)*contour
    pitch=body*t + (220 if name=='jump_lift' else -body*.45)*t*t
    samples+=.6*np.sin(2*np.pi*pitch)*np.exp(-t*decay)
    if name=='god_impact':
        samples+=.22*np.sin(2*np.pi*421*t)*np.exp(-t*11)
        samples+=.16*np.sin(2*np.pi*1097*t)*np.exp(-t*16)
    save(name,samples,peak)
