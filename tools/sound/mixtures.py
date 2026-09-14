"""Soft fruit, ceramic and sticky burning pitch. Offline LFSR and voiced synthesis."""
import numpy as np
from synth import clock, noise, band, save

for name, duration, seed, lower, upper in [('stink_throw', .26, 331, 400, 3400),
                                         ('fruit_throw', .22, 596, 170, 2200),
                                         ('pitch_throw', .32, 731, 90, 2000)]:
    t = clock(duration)
    save(name, band(noise(len(t), seed), lower, upper)*np.sin(np.pi*t/duration)**2, .31)
t = clock(.36)
breaking = band(noise(len(t), 643), 600, 5500)*np.exp(-t*30)
for f, a in [(1300, .24), (2071, .13), (2880, .06)]:
    breaking += a*np.sin(2*np.pi*f*t)*np.exp(-t*22)
save('stink_break', breaking, .39)
t = clock(.30)
wet = band(noise(len(t), 572), 100, 2100)*np.exp(-t*19)
wet += .4*np.sin(2*np.pi*(140*t-95*t*t))*np.exp(-t*27)
save('fruit_splat', wet, .41)
t = clock(.62)
meal = np.zeros(len(t))
for start in [0, .18, .36]:
    age = np.maximum(0, t-start)
    meal += (t >= start)*(band(noise(len(t), 789+round(start*100)), 140, 2600) +
        .15*np.sin(2*np.pi*(180*age-100*age*age)))*np.exp(-age*28)
save('fruit_munch', meal, .32)
t = clock(.35)
save('pitch_fuse', band(noise(len(t), 979), 900, 5000)*(.5+.5*np.sin(2*np.pi*31*t)**8)*np.sin(np.pi*t/.35)**2, .14)
t = clock(.23)
save('pitch_land', (band(noise(len(t), 574), 80, 1800)+.5*np.sin(2*np.pi*105*t))*np.exp(-t*30), .34)
t = clock(.68)
shatter = band(noise(len(t), 321), 300, 5500)*np.exp(-t*34)
shatter += .65*np.sin(2*np.pi*(95*t-30*t*t))*np.exp(-t*22)
flame = band(noise(len(t), 515), 100, 3000)*(1-np.exp(-t*15))*np.exp(-t*6)
save('pitch_burst', shatter+flame, .58)
t = clock(.40)
save('scent_sniff', band(noise(len(t), 583), 400, 3200)*np.sin(np.pi*t/.4)**2*(.25+.75*np.sin(2*np.pi*4*t)**2), .20)
t = clock(.56)
phase = 2*np.pi*(105*t+6*np.sin(2*np.pi*5*t))
voice = (np.sin(phase)+.28*np.sin(phase*2)+.12*np.sin(phase*3))
voice *= .5+.5*np.sin(2*np.pi*3*t)**4
voice += .22*band(noise(len(t), 932), 160, 2100)
save('nausea_gag', voice*np.sin(np.pi*t/.56)**2*np.exp(-t*2), .30)
