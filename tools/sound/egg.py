"""Small food bite and swallow; separate from cloth healing and meat chewing."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.52)
samples = np.zeros(len(t)); texture = band(noise(len(t), 1881), 350, 3600)
for when, strength in [(0,.5),(.12,.7),(.27,.4)]:
    age = np.maximum(0,t-when)
    samples += np.where(t>=when, np.exp(-age*55),0)*texture*strength
age = np.maximum(0,t-.32)
samples += np.where(t>=.32, np.exp(-age*24),0)*np.sin(2*np.pi*(180*age-140*age*age))*.16
save('egg_eat', samples, .30)
