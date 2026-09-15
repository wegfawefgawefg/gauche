"""Original basalt scrape, molten suction and thermal-shock sounds."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.36)
x=band(noise(len(t),15521),140,1500)*np.sin(np.pi*t/.36)**2
x*=.55+.45*np.sin(2*np.pi*31*t)**2
save('slag_crawl',x,.12)
t=clock(.72)
x=band(noise(len(t),15522),160,1800)*(1-np.exp(-t*20))*np.exp(-t*4)
x+=.35*np.sin(2*np.pi*(92*t-28*t*t))*np.exp(-t*6)
save('slag_tuck',x,.29)
t=clock(.30)
x=band(noise(len(t),15523),300,2600)*np.sin(np.pi*t/.30)**2
x+=.32*np.sin(2*np.pi*(63*t+80*t*t))*np.exp(-t*8)
save('slag_lunge',x,.27)
t=clock(.24)
x=band(noise(len(t),15524),220,3200)*np.exp(-t*25)
x+=.6*np.sin(2*np.pi*141*t)*np.exp(-t*36)
save('slag_impact',x,.22)
t=clock(.48)
x=band(noise(len(t),15525),1800,7400)*(1-np.exp(-t*70))*np.exp(-t*8)
x+=.13*np.sin(2*np.pi*2300*t)*np.exp(-t*22)
save('slag_cool',x,.19)
t=clock(.24)
x=band(noise(len(t),15526),1000,6500)*np.exp(-t*43)
x+=.28*np.sin(2*np.pi*1300*t)*np.exp(-t*60)
save('slag_crack',x,.15)
t=clock(.61)
x=band(noise(len(t),15527),100,2400)*np.exp(-t*10)
for start,freq in [(.03,174),(.11,341),(.19,672)]:
 u=np.maximum(t-start,0);x+=(t>=start)*.22*np.sin(2*np.pi*freq*u)*np.exp(-u*35)
save('slag_death',x,.29)
