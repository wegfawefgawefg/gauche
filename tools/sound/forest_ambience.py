"""Twenty offline forest cues. Loops are quiet beds; foreground tells stay separate."""
from pathlib import Path
import numpy as np
import synth
from synth import RATE, clock, noise, band, save

synth.ROOT = Path(__file__).resolve().parents[2] / "assets" / "ambience"


def room(signal, delays=(.073, .137), amount=.18):
    result=signal.copy()
    for delay in delays:
        shift=round(delay*RATE)
        if shift < len(signal): result[shift:] += signal[:-shift]*amount
    return result


def bed(name, seconds, low, high, seed, speed=.18, grain=.0):
    t=clock(seconds)
    body=band(noise(len(t),seed),low,high)
    envelope=.65+.20*np.sin(2*np.pi*speed*t)+.15*np.sin(2*np.pi*(speed*1.7)*t+.8)
    body *= envelope
    if grain:
        body += grain*band(noise(len(t),seed+16),2000,9000)*np.maximum(0,np.sin(2*np.pi*3.1*t))**8
    save(name,room(body),.48)


def creak(name, seconds, pitch, seed):
    t=clock(seconds)
    phase=2*np.pi*np.cumsum(pitch+pitch*.45*np.sin(np.pi*t/seconds)+8*np.sin(2*np.pi*13*t))/RATE
    strain=(np.sin(phase)+.4*np.sin(phase*2.03)+.2*np.sin(phase*3.04))
    strain *= (.3+.7*np.sin(2*np.pi*31*t)**8)*np.sin(np.pi*t/seconds)**2
    grit=band(noise(len(t),seed),250,2800)*np.sin(np.pi*t/seconds)**3*.15
    save(name,room(strain+grit),.43)


def bird(name, notes, seconds, seed):
    t=clock(seconds);out=np.zeros(len(t))
    for delay,duration,pitch,sweep in notes:
        age=t-delay;mask=(age>=0)&(age<duration)
        a=np.clip(age,0,duration)
        tone=np.sin(2*np.pi*(pitch*a+sweep*a*a))
        breath=band(noise(len(t),seed),1000,6000)
        out += (tone+.07*breath)*np.sin(np.pi*a/duration)**2*mask
    save(name,room(out, (.12,.24),.13),.45)


def drip(name, seconds, drops, seed):
    t=clock(seconds);out=np.zeros(len(t))
    for i,delay in enumerate(drops):
        age=np.maximum(0,t-delay)
        note=np.sin(2*np.pi*((800+i*190)*age-340*age*age))
        out += (note+.16*band(noise(len(t),seed+i),2000,10000))*np.exp(-age*27)*(t>=delay)
    save(name,room(out),.38)


if __name__ == "__main__":
    bed("forest_wind",7,70,1800,113,.14)
    bed("leaf_rustle",6,1300,9000,243,.23,.25)
    creak("pine_creak",1.7,110,233)
    creak("branch_creak",.8,240,619)
    t=clock(1.5);n=noise(len(t),941)
    crack=band(n,600,7000)*np.exp(-t*32)
    thump=np.sin(2*np.pi*65*t)*np.exp(-t*10)
    trailing=band(n,2200,8500)*np.exp(-np.maximum(0,t-.18)*4)*(t>.18)*.22
    save("branch_fall",room(crack+thump*.3+trailing),.51)
    bed("stream",6,140,5500,876,.39,.12)
    bed("wall_trickle",5,800,7900,32,.67,.28)
    drip("pool_drips",1.8,[0,.33,.71,1.1],462)
    bed("reed_hiss",5,1700,6500,144,.41)
    bird("frogs",[(0,.35,240,90),(.55,.31,200,140),(1,.4,220,70)],1.7,301)
    t=clock(6)
    chirps=np.maximum(0,np.sin(2*np.pi*2.1*t))**6 * (.5+.5*np.sin(2*np.pi*35*t))
    save("crickets",np.sin(2*np.pi*5100*t)*chirps+.2*np.sin(2*np.pi*6200*t)*chirps,.36)
    bird("tiny_bird",[(0,.12,2400,6500),(.19,.14,3100,-4700),(.41,.22,1900,6500)],.95,934)
    bird("distant_crow",[(0,.30,430,-170),(.54,.38,370,-120)],1.3,236)
    bird("distant_owl",[(0,.45,360,-95),(.75,.75,310,-45)],2.0,789)
    t=clock(.95);out=np.zeros(len(t))
    for i in range(9):
        age=np.maximum(0,t-i*.067)
        out += np.sin(2*np.pi*610*age)*np.exp(-age*100)*(t>=i*.067)
    save("woodpecker",room(out),.40)
    t=clock(1.1)
    rustle=band(noise(len(t),168),900,7000)*np.sin(np.pi*t/1.1)**2
    rustle *= .3+.7*np.sin(2*np.pi*4.2*t)**8
    save("deer_rustle",rustle,.42)
    t=clock(5)
    hum=np.sin(2*np.pi*(177*t+2*np.sin(2*np.pi*.4*t)))
    hum += .28*np.sin(2*np.pi*355*t)
    save("bee_hum",hum*(.6+.4*np.sin(2*np.pi*.6*t)**2),.35)
    t=clock(6)
    breath=band(noise(len(t),823),90,650)*np.maximum(0,np.sin(2*np.pi*.33*t))**2
    save("den_breath",breath,.44)
    creak("ruin_creak",2.0,72,277)
    t=clock(2.4)
    whisper=band(noise(len(t),974),500,3900)*np.sin(np.pi*t/2.4)**2
    whisper *= .3+.7*np.sin(2*np.pi*3.2*t)**4
    save("mansion_whisper",room(whisper,(.13,.31,.47),.25),.34)
