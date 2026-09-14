"""Soft wrapping, loosening thread, and the empty sleeve; offline cloth Foley."""
import numpy as np
from synth import clock, noise, band, save
for name, length, seed, pulses, low, high, peak in [
    ('muffle_wrap',.48,4801,[.10,.29],160,2400,.24),
    ('muffle_unwind',.30,4803,[.06,.16],380,3400,.18),
    ('muffle_empty',.22,4805,[.06],200,1800,.18)]:
    t=clock(length); env=np.zeros_like(t)
    for i, center in enumerate(pulses): env += (.8**i)*np.exp(-((t-center)/.055)**2)
    a=band(noise(len(t),seed),low,high)*env
    save(name,a,peak)
