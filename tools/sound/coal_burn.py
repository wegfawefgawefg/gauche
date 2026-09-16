"""Hot coal contact: short granular crackle and a falling breath of fire."""
import numpy as np
from synth import clock,noise,band,save
t=clock(.68)
x=band(noise(len(t),15741),450,6500)*np.minimum(t/.02,1)*np.exp(-t*6)
for i,start in enumerate([.01,.06,.13,.22]):
 q=np.maximum(0,t-start)
 x+=band(noise(len(t),15742+i),1200,9500)*np.exp(-q*120)*(t>=start)*.6
x+=band(noise(len(t),15749),60,350)*np.exp(-t*9)*.45
save('coal_burn',x,.27)
