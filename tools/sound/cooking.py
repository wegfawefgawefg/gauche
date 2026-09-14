"""A brief oily sizzle with tiny crackles, one cue per cooked portion."""
import numpy as np
from synth import clock, noise, band, save

if __name__ == "__main__":
    t = clock(.63)
    sizzle = band(noise(len(t), 342), 1800, 11000)
    bubbles = np.maximum(0, np.sin(2*np.pi*23*t))**16
    save("cooking_sizzle", sizzle*(.55 + bubbles)*np.sin(np.pi*t/.63)**.7, .32)
