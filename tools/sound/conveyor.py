"""Offline rollers, crank ratchet, shoe drag and a broken machine housing."""
import numpy as np
from synth import clock, noise, band, save
t=clock(.13)
x=band(noise(len(t),9901),90,1600)*np.exp(-35*t)+.3*np.sin(2*np.pi*190*t)*np.exp(-40*t)
save('belt_carry',x,.12)
t=clock(.19);x=np.zeros(len(t))
for at in [0,.042,.088]:
 u=np.maximum(t-at,0);x+=(t>=at)*np.exp(-95*u)*(.6*np.sin(2*np.pi*780*u)+.4*band(noise(len(t),9913+int(at*100)),600,5200))
save('belt_crank',x,.23)
t=clock(.35)
x=band(noise(len(t),9921),230,2900)*np.sin(np.pi*t/.35)**2
x+=.2*np.sin(2*np.pi*(420*t-180*t*t))*np.exp(-8*t)
save('brake_fit',x,.25)
t=clock(.22)
x=band(noise(len(t),9923),400,4300)*np.exp(-50*t)+.7*np.sin(2*np.pi*(260*t-260*t*t))*np.exp(-30*t)
save('brake_snap',x,.29)
t=clock(.25)
x=.6*band(noise(len(t),9927),350,4100)*np.exp(-55*t)+np.sin(2*np.pi*170*t)*np.exp(-32*t)
save('belt_hit',x,.27)
t=clock(.73);x=np.zeros(len(t))
for at,f in [(0,120),(.09,340),(.18,550),(.27,230)]:
 u=np.maximum(t-at,0);x+=(t>=at)*np.exp(-26*u)*(.6*np.sin(2*np.pi*f*u)+.5*band(noise(len(t),9931+f),180,3300))
save('belt_break',x,.35)

# Loop stays local and follows powered machinery, never the gameplay noise bus.
import synth
t=clock(6.0)
x=.34*np.sin(2*np.pi*90*t)+.12*np.sin(2*np.pi*180*t)
x+=.28*band(noise(len(t),9941),180,1400)*(.7+.3*np.cos(2*np.pi*3*t))
synth.ROOT=synth.ROOT.parent/'ambience'
save('belt_rollers',x,.24)
