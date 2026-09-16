"""Native 16px crane worker: blue overalls, earmuffs, hand on controls."""
from underworks_palette import *
for pose in ('','_work','_wait','_alarm'):
    im,p=canvas()
    coat='#4c6a72';skin='#b79a76'
    p.rectangle((5,7,9,12),fill=INK)
    p.rectangle((5,7,8,11),fill=coat)
    p.line((6,7,6,10),fill='#92a3a0')
    p.rectangle((4,13,6,14),fill=WOOD)
    p.rectangle((8,13,10,14),fill=WOOD)
    p.line((3,15,6,15),fill=INK);p.line((8,15,11,15),fill=INK)
    p.rectangle((5,2,10,6),fill=INK)
    p.rectangle((6,3,10,6),fill=skin)
    p.point((10,4),fill=LIGHT)
    p.rectangle((5,1,9,2),fill=BRASS)
    p.line((4,3,11,3),fill=GOLD)
    p.rectangle((4,4,5,5),fill=IRON)
    p.line((3,8,3,11),fill=coat)
    if pose=='_work':
        p.line((9,8,11,10),fill=coat,width=2)
        p.line((11,10,13,10),fill=skin)
        p.point((3,12),fill=skin)
    elif pose=='_alarm':
        p.line([(10,8),(12,7),(12,4)],fill=skin,width=2)
        p.line((2,7,2,3),fill=skin)
    elif pose=='_wait':
        p.line((9,8,9,10),fill=skin)
        p.line((4,10,9,10),fill=skin)
    else:
        p.line((10,8,10,11),fill=coat)
        p.point((10,12),fill=skin)
    save(im,'crane_operator'+pose)
im,p=canvas()
p.rectangle((11,11,15,15),fill=INK)
p.rectangle((12,11,14,13),fill=IRON)
p.line((12,12,14,12),fill=BRASS)
p.point((14,11),fill=GOLD)
p.line((12,10,11,8),fill=STEEL)
p.point((11,8),fill=LIGHT)
save(im,'crane_console')
