"""Offline insect buzz, soft owl voice and dry hollow woodpecker percussion."""
import numpy as np
from synth import RATE, clock, band, noise, save

t=clock(.38)
phase=2*np.pi*np.cumsum(430+28*np.sin(2*np.pi*9*t))/RATE
buzz=np.sin(phase)+.32*np.sin(phase*2)+.15*np.sin(phase*4)
save('mosquito_buzz',buzz*np.sin(np.pi*t/.38)**.6,.19)
t=clock(.12)
save('mosquito_bite',(band(noise(len(t),101),900,5600)*.5+np.sin(2*np.pi*620*t))*np.exp(-t*42),.25)
t=clock(.90)
voice=np.zeros(len(t))
for start,length,pitch in [(0,.30,340),(.40,.43,285)]:
    local=t-start; active=(local>=0)&(local<length); tt=local[active]
    phase=2*np.pi*(pitch*tt+15*tt*tt)
    voice[active]=(np.sin(phase)+.16*np.sin(phase*2))*np.sin(np.pi*tt/length)**.7
save('owl_hoot',voice,.36)
t=clock(.38)
wings=band(noise(len(t),619),260,4300)*np.sin(np.pi*t/.38)**1.6
save('owl_swoop',wings*(.4+.6*np.sin(2*np.pi*12*t)**4),.33)
t=clock(.20)
save('owl_claw',(band(noise(len(t),922),1300,7000)+np.sin(2*np.pi*195*t)*.6)*np.exp(-t*26),.36)

def knock(seconds,seed):
    t=clock(seconds)
    return (np.sin(2*np.pi*610*t)+.4*np.sin(2*np.pi*1190*t)+
            band(noise(len(t),seed),700,4800)*.45)*np.exp(-t*60)

t=clock(.44); rattle=np.zeros(len(t))
for i,start in enumerate([0,.09,.165,.23,.285,.335]):
    tone=knock(.08,412+i); index=round(start*RATE)
    rattle[index:index+len(tone)]+=tone*(.6+i*.07)
save('woodpecker_rattle',rattle,.34)
save('woodpecker_drill',knock(.12,519),.38)
t=clock(.30)
save('beak_stuck',(np.sin(2*np.pi*(190*t-170*t*t))*np.exp(-t*15)+
    band(noise(len(t),61),300,1700)*np.exp(-t*30)),.35)
