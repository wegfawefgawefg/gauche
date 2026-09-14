"""Offline line swish, small metal hook, spool ratchet and snapped nylon."""
import numpy as np
from synth import clock, noise, band, save

def click(t, start, pitch, decay):
    age=np.maximum(0,t-start)
    return (t>=start)*np.sin(2*np.pi*pitch*age)*np.exp(-age/decay)

t=clock(.35)
a=band(noise(len(t),5201),600,4000)*np.exp(-((t-.14)/.07)**2)
save('fishing_cast',a,.23)
t=clock(.14)
save('fishing_tap',click(t,.015,1800,.022)+.3*click(t,.04,2800,.012),.19)
t=clock(.21)
save('fishing_catch',click(t,.01,1300,.028)+.5*click(t,.065,1900,.022),.23)
t=clock(.055)
a=band(noise(len(t),5203),600,2400)*np.exp(-t/.014)+click(t,.01,520,.009)*.15
save('fishing_reel',a,.11)
t=clock(.25)
save('fishing_finish',click(t,.015,720,.035)+.5*click(t,.08,480,.018),.21)
t=clock(.24)
a=click(t,.01,440,.035)+band(noise(len(t),5205),1400,6500)*np.exp(-t/.018)
save('fishing_snap',a,.27)
t=clock(.3)
a=band(noise(len(t),5207),450,2600)*np.exp(-((t-.07)/.045)**2)+click(t,.14,360,.035)
save('fishing_empty',a,.23)
