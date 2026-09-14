"""A wet brass bell, canvas/metal strain, and water displaced by a heavy diver."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.33)
bubbles = np.zeros(len(t))
for at, freq in [(0, 420), (.07, 700), (.15, 530)]:
    age = np.maximum(0, t-at)
    bubbles += .35*np.sin(2*np.pi*(freq*age+1000*age*age))*np.exp(-age*55)*(t>=at)
save('diver_bubbles', bubbles, .10)
t = clock(.70)
bell = sum(np.sin(2*np.pi*f*t)*np.exp(-t*d)*a for f,d,a in [(580,6,.6),(1432,9,.25),(2090,12,.2),(3209,18,.1)])
bell += band(noise(len(t), 2411), 450, 3900)*np.exp(-t*70)*.15
save('diver_ring', bell, .31)
t = clock(.45)
water = band(noise(len(t), 2417), 250, 5200)*(1-np.exp(-t*90))*np.exp(-t*9)
water *= .75+.25*np.sin(2*np.pi*21*t)
save('diver_rise', water, .25)
t = clock(.48)
creak = band(noise(len(t), 2423), 180, 1600)*np.sin(np.pi*t/.48)**2
creak += .18*np.sin(2*np.pi*(180*t+70*t*t))*np.exp(-t*4)
save('diver_windup', creak, .26)
t = clock(.28)
hit = band(noise(len(t), 2437), 120, 3200)*np.exp(-t*32)
hit += .4*np.sin(2*np.pi*130*t)*np.exp(-t*24)
hit += .13*np.sin(2*np.pi*780*t)*np.exp(-t*17)
save('diver_strike', hit, .40)
t = clock(.42)
water = band(noise(len(t), 2441), 180, 2500)*np.exp(-t*12)
water += .18*np.sin(2*np.pi*(180*t+600*t*t))*np.exp(-t*35)
save('diver_dive', water, .24)
t = clock(.55)
collapse = band(noise(len(t), 2447), 120, 1500)*np.exp(-t*14)
for at, f in [(0, 350), (.06, 610), (.17, 460)]:
    age=np.maximum(0,t-at)
    collapse += .2*np.sin(2*np.pi*f*age)*np.exp(-age*18)*(t>=at)
save('diver_death', collapse, .32)
