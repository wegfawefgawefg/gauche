"""Native 24x72 welded work-light tower; planted foot at (12,68)."""
from underworks_palette import *
for buckled in (False,True):
    im,p=canvas((24,72))
    p.polygon([(4,67),(7,61),(16,61),(20,67),(18,70),(5,70)],fill=INK)
    p.line((5,67,19,67),fill=IRON,width=2)
    for x in (7,16):
        if buckled:p.line([(x,65),(x-2,50),(x+2,44),(x,13)],fill=IRON,width=2)
        else:p.line((x,65,x,13),fill=IRON,width=2)
    for y in range(17,61,8):
        p.line((8,y,15,y+7),fill=STEEL)
        p.line((15,y,8,y+7),fill='#3b4b48')
        p.line((7,y,17,y),fill='#73806e')
    p.rectangle((6,61,17,63),fill=BRASS)
    for x in (7,11,15):p.line((x,61,x+2,63),fill=INK)
    p.rectangle((6,35,17,42),fill=INK);p.rectangle((8,36,15,40),fill='#566254')
    p.line((9,39,13,39),fill=BRASS);p.point((14,37),fill=GOLD)
    p.line((11,7,11,14),fill=STEEL,width=2)
    p.rectangle((1,4,22,9),fill=INK);p.line((1,3,22,3),fill=STEEL)
    p.rectangle((2,5,9,8),fill='#3d433d');p.rectangle((14,5,21,8),fill='#3d433d')
    p.line((2,10,9,10),fill=RUST);p.line((14,10,21,10),fill=RUST)
    if buckled:
        p.line([(6,52),(10,50),(9,47)],fill=RUST)
        p.line((15,43,18,49),fill=RUST)
    save(im,'tower_buckled' if buckled else 'light_tower')
im,p=canvas((24,72))
for x in (3,15):
    p.rectangle((x,5,x+5,7),fill='#ead39b');p.line((x,5,x+5,5),fill='#fff0bf')
save(im,'tower_lamps')
im,p=canvas();p.polygon([(2,6),(11,4),(14,11),(4,14),(1,11)],fill=INK)
p.polygon([(3,7),(10,5),(12,10),(5,12)],fill='#566254')
for x,y in ((3,8),(9,6),(11,10),(5,11)):p.point((x,y),fill=STEEL)
save(im,'tower_foot')
im,p=canvas();p.polygon([(0,5),(5,3),(9,6),(15,4),(15,12),(9,13),(5,10),(0,12)],fill=INK)
p.line([(0,5),(5,4),(9,7),(15,5)],fill=STEEL)
p.line([(0,11),(5,9),(9,12),(15,11)],fill=IRON)
for x in (2,7,12):p.line((x,6,x+3,10),fill='#9b783d')
p.line((1,12,6,12),fill=RUST);save(im,'tower_wreck')
