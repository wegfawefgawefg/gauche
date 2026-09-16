"""Low ground pipes and a spring grate, at the game's native pixel scale."""
from underworks_palette import *
for broken in (False,True):
    im,p=canvas()
    p.rectangle((0,6,15,10),fill=INK)
    p.rectangle((0,7,15,9),fill=IRON)
    p.line((0,7,15,7),fill=STEEL)
    for x in (1,12):
        p.rectangle((x,5,x+2,11),fill=INK);p.rectangle((x+1,6,x+1,10),fill=BRASS)
    if broken:
        p.rectangle((6,5,9,11),fill=(0,0,0,0))
        p.line((5,6,6,6),fill=RUST);p.line((9,10,10,10),fill=RUST)
    save(im,'water_pipe_broken' if broken else 'water_pipe')
im,p=canvas()
p.rectangle((3,3,12,12),fill=INK)
p.rectangle((4,4,11,11),fill=IRON)
p.rectangle((5,5,10,10),fill='#283d3e')
for x in (6,9):p.line((x,5,x,10),fill=STEEL)
p.line((4,3,11,3),fill=STEEL)
for x,y in ((4,4),(11,11)):p.point((x,y),fill=BRASS)
save(im,'spring_intake')
