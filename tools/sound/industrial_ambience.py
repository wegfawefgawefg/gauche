"""Local furnace, hoist, hot basin and workshop sounds; no music or runtime synthesis."""
from pathlib import Path
import numpy as np
from scipy import signal
import synth
from synth import RATE, clock, band, save

synth.ROOT = Path(__file__).resolve().parents[2] / 'assets/ambience'


def grain(t, seed, low, high):
    # Longer seeded noise avoids the short repeating grain of a 16-bit LFSR bed.
    return band(np.random.default_rng(seed).normal(0, 1, len(t)), low, high)


def space(x, echoes=(.13, .31), wet=.12):
    y=x.copy()
    for delay in echoes:
        n=round(delay*RATE)
        if n<len(x): y[n:]+=wet*x[:-n]
    return y


def loop(name, x, peak):
    n=round(.6*RATE); f=np.linspace(0, 1, n)
    y=x[:-n].copy(); y[:n]=x[-n:]*(1-f)+x[:n]*f
    save(name, y, peak)


def hits(t, times, tones, decay, seed):
    y=np.zeros(len(t)); n=grain(t,seed,600,4300)
    for i,start in enumerate(times):
        a=np.maximum(0,t-start)
        partials=sum(np.sin(2*np.pi*f*a)*np.exp(-a*(decay+j*3))/(1+j)
                     for j,f in enumerate(tones))
        y+=(t>=start)*(partials+.23*n*np.exp(-a*85))/(1+.17*(i%3))
    return y


def main():
    t=clock(12.6)
    gust=.53+.19*np.sin(t*.67)+.12*np.sin(t*1.37+.7)
    x=grain(t,15101,35,400)*gust+.06*grain(t,15102,800,2200)*gust**2
    loop('furnace_breath',x,.34)

    t=clock(3.3)
    x=hits(t,[.06,.87,1.92,2.23],[381,853,1463],12,15103)
    x=signal.sosfilt(signal.butter(2,1400,fs=RATE,output='sos'),space(x,(.17,.39,.61),.21))
    save('distant_picks',x,.29)

    t=clock(10.6)
    x=hits(t,[.24,.39,1.73,2.04,4.61,4.69,5.26,7.31,8.86,9.12],[617,1123,2171],28,15104)
    x+=.09*grain(t,15105,260,1800)*np.maximum(0,np.sin(t*1.17))**6
    loop('chain_sway',space(x),.28)

    t=clock(2.9)
    x=hits(t,[.06,.41,1.39,2.21],[1127,1973,3247],24,15106)
    save('cooling_ticks',space(x,(.04,.11),.12),.23)

    t=clock(12.6); x=.15*grain(t,15107,45,490)
    rng=np.random.default_rng(15108)
    for start in rng.uniform(.05,12.1,24):
        a=np.maximum(0,t-start); f=rng.uniform(65,160)
        x+=(t>=start)*np.sin(2*np.pi*(f*a+65*a*a))*np.exp(-a*14)*np.minimum(1,a*120)*rng.uniform(.2,.8)
    loop('slag_bubbling',x,.31)

    t=clock(2.2)
    x=hits(t,[.03,.27,.76],[91,243,587],7,15109)
    save('water_hammer',space(x,(.14,.29,.49),.16),.31)

    t=clock(5.2)
    x=sum(np.sin(2*np.pi*f*t)*np.exp(-t*d)*a for f,d,a in
          [(173,.9,1),(437,1.4,.51),(913,1.9,.22),(1511,2.6,.09)])
    x*=1-np.exp(-t*70)
    x+=.13*grain(t,15110,200,2100)*np.exp(-t*35)
    save('shift_bell',space(x,(.23,.57,.91),.22),.28)


if __name__=='__main__': main()
