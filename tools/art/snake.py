"""Coiled forest snake with an upright warning and extended strike pose."""
from underworks_palette import *
for phase,name in enumerate(['snake','snake_coil','snake_strike']):
    im,p=canvas()
    path=[(2,13),(5,14),(11,13),(13,10),(10,8),(5,10),(4,7),(8,5),(11,6)]
    if phase==1:path=[(2,13),(7,14),(12,12),(10,10),(5,11),(4,8),(8,7),(10,4),(9,1)]
    if phase==2:path=[(1,13),(6,14),(10,12),(7,10),(5,8),(7,5),(12,5),(14,7)]
    p.line(path,fill='#263427',width=4)
    p.line(path,fill='#758347',width=2)
    for x,y in path[1:-1:2]:p.point((x,y),fill='#bbad66')
    x,y=path[-1];p.ellipse((x-2,y-1,min(15,x+2),y+2),fill='#819154')
    p.point((x-1,y),fill='#e2cc7b');p.point((min(15,x+1),y),fill='#1e2820')
    if phase==2:p.line((x,y+3,x,y+5),fill='#a45138')
    save(im,name)
