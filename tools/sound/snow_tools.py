"""Offline snow compression, soft tunneling and a small mustelid-like burrower."""
import numpy as np
from synth import clock, noise, band, save

t = clock(.40)
save('snow_rumble', (band(noise(len(t), 4101), 70, 600)+.12*np.sin(2*np.pi*95*t))*np.sin(np.pi*t/.4)**2, .18)
t = clock(.45)
save('snow_warning', (.45*np.sin(2*np.pi*(360*t+280*t*t)) + band(noise(len(t), 4103), 1700, 4200)*.15)*np.sin(np.pi*t/.45), .28)
t = clock(.26)
save('snow_burst', (band(noise(len(t), 4105), 130, 3800)+.12*np.sin(2*np.pi*145*t))*np.exp(-t*17), .40)
t = clock(.50)
save('snow_dig', band(noise(len(t), 4107), 180, 2600)*(.3+.7*np.sin(2*np.pi*7*t)**2)*np.sin(np.pi*t/.5), .22)
t = clock(.36)
save('snow_death', (np.sin(2*np.pi*(640*t-570*t*t))*.3+band(noise(len(t), 4109), 130, 2900))*np.exp(-t*13), .34)
t = clock(.22)
save('snow_scrape', band(noise(len(t), 4111), 650, 5700)*np.sin(np.pi*t/.22)**2, .23)
t = clock(.24)
save('snow_pack', band(noise(len(t), 4113), 180, 2000)*(np.exp(-t*45)+.6*np.exp(-np.abs(t-.12)*70)), .22)
t = clock(.25)
save('scoop_break', (band(noise(len(t), 4115), 100, 5600)+.18*np.sin(2*np.pi*1200*t))*np.exp(-t*23), .35)
t = clock(.12)
save('snow_throw', band(noise(len(t), 4117), 800, 5300)*np.sin(np.pi*t/.12)**2, .18)
t = clock(.20)
save('snow_splat', (band(noise(len(t), 4119), 300, 4700)+.16*np.sin(2*np.pi*(110*t-100*t*t)))*np.exp(-t*22), .28)
