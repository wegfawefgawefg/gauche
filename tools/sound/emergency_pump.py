"""Original mobile pump motor, suction, pressure release and broken casing."""
import numpy as np
from synth import clock, noise, band, save

t=clock(.9)
envelope=np.sin(np.pi*t/.9)**.6
motor=np.sin(2*np.pi*(73*t+16*t*t))+.3*np.sin(2*np.pi*149*t)
x=motor*envelope*.3+band(noise(len(t),15591),200,1300)*envelope*(.2+.8*np.sin(2*np.pi*9*t)**8)
save('pump_slurp',x,.27)
t=clock(.6)
envelope=(1-np.exp(-t*25))*(.3+t)
x=(np.sin(2*np.pi*(83*t+68*t*t))+.17*np.sin(2*np.pi*(293*t+121*t*t)))*envelope
x+=band(noise(len(t),15592),450,2200)*envelope*.7
save('pump_pressure',x,.29)
t=clock(.55)
x=band(noise(len(t),15593),180,4800)*(1-np.exp(-t*100))*np.exp(-t*6)
x+=.45*np.sin(2*np.pi*(97*t-23*t*t))*np.exp(-t*22)
save('pump_jet',x,.34)
t=clock(.8)
x=band(noise(len(t),15594),200,5600)*np.exp(-t*17)
for onset,frequency in ((0,137),(.04,313),(.12,691),(.21,233)):
    age=np.maximum(0,t-onset)
    x+=(np.sin(2*np.pi*frequency*age)+.3*np.sin(2*np.pi*frequency*1.731*age))*np.exp(-age*13)*(t>=onset)*.3
save('pump_break',x,.36)
