"""Drowned observatory: twenty original offline cues, no runtime synthesis."""
from pathlib import Path
import numpy as np
import synth
from synth import RATE, clock, noise, band, save

synth.ROOT=Path(__file__).resolve().parents[2]/'assets/ambience'


def room(x, delays=(.083,.193,.317), wet=.14):
    y=x.copy()
    for dt in delays:
        n=round(dt*RATE)
        if n<len(x): y[n:]+=x[:-n]*wet
    return y


def loop(name,x,peak=.4):
    # JOIN: Blend matching ends before encoding; all long beds have an overlap.
    n=round(.4*RATE); fade=np.linspace(0,1,n)
    y=x[:-n].copy(); y[:n]=x[-n:]*(1-fade)+x[:n]*fade
    save(name,y,peak)


def resonance(t,frequencies,decays):
    return sum(np.sin(2*np.pi*f*t)*np.exp(-t*d)/(1+i*.65)
               for i,(f,d) in enumerate(zip(frequencies,decays)))


def strike_train(t,times,frequencies,decays,seed):
    out=np.zeros(len(t)); grit=band(noise(len(t),seed),800,5500)
    for i,start in enumerate(times):
        age=np.maximum(0,t-start)
        out+=(resonance(age,frequencies,decays)+grit*.12*np.exp(-age*55))*(t>=start)/(1+i*.12)
    return out


def main():
    t=clock(3.6); phase=2*np.pi*np.cumsum(83+30*np.sin(np.pi*t/3.6)+3*np.sin(2*np.pi*7*t))/RATE
    ice=(np.sin(phase)+.3*np.sin(phase*2.37))*(.6+.4*np.sin(2*np.pi*23*t)**6)
    save('ice_groan',room(ice*np.sin(np.pi*t/3.6)**2,(.19,.43,.71),.23),.37)
    t=clock(8.4); n=noise(len(t),1009)
    water=band(n,45,650)*(.7+.3*np.sin(2*np.pi*.37*t)**2)
    bubbles=np.sin(2*np.pi*(160*t+5*np.sin(t*2)))*np.maximum(0,np.sin(t*3.1))**12*.12
    loop('under_ice',water+bubbles,.36)
    t=clock(6.4); out=np.zeros(len(t))
    for i,start in enumerate([.1,.7,1.05,1.83,2.7,3.1,3.72,4.7,5.0,5.8]):
        a=np.maximum(0,t-start)
        out+=np.sin(2*np.pi*((1200+i%3*230)*a-1700*a*a))*np.exp(-a*32)*(t>=start)
    loop('thaw_drip',room(out,(.05,.12,.21),.22),.34)
    t=clock(10.4); n=noise(len(t),1019)
    loop('thin_wind',band(n,90,1550)*(.58+.28*np.sin(t*.71)+.12*np.sin(t*1.43)),.38)
    t=clock(8.4); gust=.2+.8*np.sin(t*.55)**4
    whistle=np.sin(2*np.pi*(690*t+9*np.sin(t*.9)))+.23*np.sin(2*np.pi*1383*t)
    loop('window_whistle',whistle*gust*.13+band(noise(len(t),1031),700,3600)*gust,.35)
    t=clock(1.8)
    save('hanging_chain',room(strike_train(t,[.03,.14,.38,.51,.82],[740,1217,2183],[12,17,24],1033)),.38)
    t=clock(6)
    bell=resonance(t,[227,609,1127,1773],[.7,1.2,1.8,2.7])*(1-np.exp(-t*45))
    save('distant_bell',room(bell,(.28,.61,.94),.3),.34)
    t=clock(2.3); phase=2*np.pi*np.cumsum(96+21*np.sin(t*2.4)+7*np.sin(t*39))/RATE
    creak=(np.sin(phase)+.3*np.sin(phase*3.07))*(.2+.8*np.sin(t*77)**8)
    save('timber_creak',room(creak*np.sin(np.pi*t/2.3)**2),.38)
    t=clock(1.9); n=noise(len(t),1039)
    snow=band(n,1200,6800)*np.sin(np.pi*t/1.9)**2*(.6+.4*np.sin(t*13)**2)
    save('snow_settling',snow+band(n,90,450)*np.exp(-t*5)*.5,.33)
    t=clock(8.4); n=noise(len(t),1049)
    idle=band(n,60,800)*(.5+.3*np.sin(t*7)**2)+.13*np.sin(2*np.pi*63*t)
    loop('boiler_idle',idle+.08*np.sin(2*np.pi*126*t)*(.6+.4*np.sin(t*1.7)),.38)
    t=clock(7.4)
    loop('pressure_hiss',band(noise(len(t),1051),1700,7400)*(.78+.13*np.sin(t*.8)),.32)
    t=clock(2.1)
    save('pipe_knock',room(strike_train(t,[0,.29,.91],[177,493,891],[9,13,20],1061),(.07,.22,.41),.24),.36)
    t=clock(8.4); phase=(t%.88)/.88
    splash=band(noise(len(t),1063),220,3800)*(.2+.8*np.sin(np.pi*phase)**4)
    bearing=np.sin(2*np.pi*(121*t+.8*np.sin(t*4)))*np.exp(-phase*15)*.16
    loop('waterwheel',splash+bearing,.38)
    t=clock(9.4); n=noise(len(t),1069)
    lap=band(n,160,1900)*(.15+.85*np.sin(t*1.4)**4)
    grit=band(n,2700,5300)*np.maximum(0,np.sin(t*1.4+.4))**14*.2
    loop('slush_lap',lap+grit,.32)
    t=clock(1.9)
    save('glass_tinkle',room(strike_train(t,[.03,.17,.24,.66],[1931,3017,4903],[8,14,21],1087)),.33)
    t=clock(8.4)
    motor=.16*np.sin(2*np.pi*92*t)+.07*np.sin(2*np.pi*276*t)
    gear=band(noise(len(t),1091),600,1900)*(.2+.8*np.sin(t*34)**10)*.3
    loop('observatory_motor',motor+gear,.34)
    t=clock(7.4); n=noise(len(t),1093)
    flap=band(n,220,2300)*(.08+np.maximum(0,np.sin(t*8+np.sin(t*2)))**8)
    loop('cloth_flutter',flap+band(n,1400,4500)*.09,.36)
    t=clock(3.4); out=np.zeros(len(t))
    for start,length,pitch in [(0,1.1,330),(1.55,1.35,280)]:
        a=np.clip(t-start,0,length); env=np.sin(np.pi*a/length)**2
        ph=2*np.pi*(pitch*a-32*a*a+3*np.sin(a*15))
        out+=(np.sin(ph)+.2*np.sin(ph*2))*env
    save('far_ice_call',room(out,(.31,.58,.83),.26),.32)
    t=clock(8.4); n=noise(len(t),1097)
    draft=band(n,65,460)*(.55+.3*np.sin(t*.6)+.15*np.sin(t*1.3))
    loop('chimney_draft',draft+band(n,1500,2800)*.08,.34)
    t=clock(3.3)
    knock=strike_train(t,[.05,.53,1.4],[91,233,471],[7,12,20],1103)
    save('submerged_knock',room(band(knock,45,650),(.12,.29,.48),.25),.34)


if __name__=='__main__': main()
