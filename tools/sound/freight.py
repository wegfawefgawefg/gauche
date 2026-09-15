"""Bell warning, dry wheel clatter, braking and cargo handling. Offline only."""
import numpy as np
from synth import clock, noise, band, save

def metal(seconds,seed,base):
    t=clock(seconds)
    x=.4*band(noise(len(t),seed),180,4500)*np.exp(-t*22)
    for ratio,amp,decay in [(1,.5,12),(2.37,.24,17),(3.81,.12,25)]:
        x+=amp*np.sin(2*np.pi*base*ratio*t)*np.exp(-t*decay)
    return t,x
for name,seconds,seed,base,peak in [('cart_push',.28,15651,145,.22),('cart_roll',.16,15652,220,.13),('cart_impact',.38,15653,112,.36),('cart_cargo',.26,15654,330,.20),('cart_break',.68,15655,82,.35),('rail_cut',.36,15656,730,.28)]:
    t,x=metal(seconds,seed,base);save(name,x,peak)
t=clock(.44)
x=band(noise(len(t),15657),500,4000)*np.sin(np.pi*t/.44)*.3
x+=np.sin(2*np.pi*(1150*t-600*t*t))*np.exp(-t*8)*.2
save('cart_stop',x,.20)
t=clock(.85);x=np.zeros(len(t))
for onset in [0,.19]:
    u=np.maximum(0,t-onset)
    for freq,amp in [(790,.45),(1841,.2),(3110,.09)]:
        x+=(t>=onset)*amp*np.sin(2*np.pi*freq*u)*np.exp(-u*8)
save('shunter_bell',x,.28)
t=clock(.45);x=band(noise(len(t),15658),180,1600)*np.sin(np.pi*t/.45)
x+=.4*np.sin(2*np.pi*(110*t+9*t*t))*np.sin(np.pi*t/.45)
save('shunter_warn',x,.24)
t,x=metal(.3,15659,190);save('shunter_swing',x,.29)
t=clock(.6);x=band(noise(len(t),15660),90,1100)*np.exp(-t*6)
x+=.5*np.sin(2*np.pi*(105*t-24*t*t))*np.exp(-t*7)
save('shunter_death',x,.29)
