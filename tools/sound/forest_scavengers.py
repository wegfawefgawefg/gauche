"""Offline paper-hive rattles, wasp wing buzz, goblin throat sounds and crow calls."""
import numpy as np
from synth import RATE, clock, band, noise, save


def wing(length, seed, start, end):
    t = clock(length)
    phase = 2*np.pi*np.cumsum(np.linspace(start, end, len(t)) + 9*np.sin(2*np.pi*21*t))/RATE
    body = np.sin(phase) + .28*np.sin(phase*3) + .16*np.sin(phase*5)
    return (body*.65 + band(noise(len(t), seed), 500, 4400)*.3)*np.sin(np.pi*t/length)**.6

save('nest_buzz', wing(.62, 827, 125, 210), .32)
save('wasp_release', wing(.30, 932, 210, 390), .28)
save('wasp_warning', wing(.26, 782, 350, 480), .25)
t = clock(.13)
save('wasp_sting', (np.sin(2*np.pi*(950*t-1900*t*t)) + band(noise(len(t), 814), 2200, 7600)) * np.exp(-t*38), .34)
t = clock(.35)
paper = band(noise(len(t), 771), 850, 6000)*(np.exp(-t*15)+.5*np.exp(-((t-.12)/.025)**2))
save('nest_break', paper + .25*np.sin(2*np.pi*160*t)*np.exp(-t*22), .39)
t = clock(.28)
save('goblin_hiss', band(noise(len(t), 108), 900, 6500)*np.sin(np.pi*t/.28)**.7 + .3*np.sin(2*np.pi*190*t)*np.exp(-t*8), .29)
t = clock(.66); chuckle = np.zeros(len(t))
for start, length, pitch in [(0, .16, 195), (.20, .15, 225), (.40, .21, 170)]:
    mask = (t >= start) & (t < start+length); a = t[mask]-start
    phase = 2*np.pi*(pitch*a-60*a*a)
    chuckle[mask] = (np.sin(phase)+.35*np.sin(phase*3)+.2*np.sin(phase*5))*np.sin(np.pi*a/length)**.7
save('goblin_chuckle', chuckle, .30)
t = clock(.20)
save('scavenge_take', band(noise(len(t), 192), 500, 3700)*np.sin(np.pi*t/.20)**1.5 + .25*np.sin(2*np.pi*780*t)*np.exp(-t*27), .23)


def caw(length, seed, pitch):
    t = clock(length)
    phase = 2*np.pi*(pitch*t - pitch*.22*t*t/length + 1.1*np.sin(2*np.pi*32*t))
    rasp = np.sin(phase)+.45*np.sin(phase*2)+.2*np.sin(phase*4)
    return (rasp*.6 + band(noise(len(t), seed), 650, 3200)*.45)*np.sin(np.pi*t/length)**.8

save('crow_call', np.concatenate([caw(.25, 402, 560), np.zeros(round(RATE*.12)), caw(.30, 403, 470)]), .36)
save('crow_rattle', caw(.26, 511, 690), .30)
t = clock(.15)
save('crow_snatch', band(noise(len(t), 638), 1600, 6200)*np.exp(-t*30) + .3*np.sin(2*np.pi*1100*t)*np.exp(-t*40), .32)
