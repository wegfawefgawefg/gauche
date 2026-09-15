"""Steel runner slash, edge contact and spiked binding buckles."""
import numpy as np
from synth import clock,noise,band,save

t=clock(.24);n=noise(len(t),1193)
save('skate_slash',band(n,700,5700)*np.sin(np.pi*t/.24)**2+.12*np.sin(2*np.pi*(930*t-800*t*t))*np.exp(-t*17),.38)
t=clock(.18);n=noise(len(t),1201)
save('skate_cut',band(n,250,5800)*np.exp(-t*35)+.2*np.sin(2*np.pi*170*t)*np.exp(-t*24),.42)
for name,starts,seconds,seed in [('crampons_fit',[0,.14,.32],.6,1213),('crampons_release',[0,.09],.34,1217)]:
 t=clock(seconds);n=band(noise(len(t),seed),800,4000);out=np.zeros(len(t))
 for i,start in enumerate(starts):
  a=np.maximum(0,t-start)
  out+=(np.sin(2*np.pi*(690+i*130)*a)*.3+n)*np.exp(-a*45)*(t>=start)
 save(name,out,.3)

for name,pitch,seed in [('skate_break',1700,1223),('crampons_spent',780,1229)]:
 t=clock(.4);n=band(noise(len(t),seed),350,6300)
 save(name,n*np.exp(-t*38)+.25*np.sin(2*np.pi*pitch*t)*np.exp(-t*13),.33)
