"""A small, plain Z for slowly drifting sleep motes."""
from underworks_palette import *
im,p=canvas()
p.line([(4,4),(10,4),(4,10),(10,10)],fill='#b8bea5',width=1)
save(im,'sleep_z')
