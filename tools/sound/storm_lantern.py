"""Quiet metal shutter and sliding lens mechanisms, generated offline."""
import numpy as np
from synth import clock,noise,band,save
for name,seed,tone,rate in [('lantern_open',1361,620,19),('lantern_close',1363,350,29),
                           ('lantern_focus',1367,840,15),('lantern_wide',1369,730,18)]:
    t=clock(.22)
    scrape=band(noise(len(t),seed),600,4100)*np.sin(np.pi*t/.22)**2
    tap=np.sin(2*np.pi*tone*t)*np.exp(-t*rate)
    save(name,.35*tap+scrape*.7,.20)
t=clock(.45)
save('lantern_empty',band(noise(len(t),1373),900,5900)*np.exp(-t*13)+
     .08*np.sin(2*np.pi*270*t)*np.exp(-t*22),.16)
