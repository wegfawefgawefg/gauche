"""Native 16px freight-yard wood, bound stone and corrugated steel.

Roof bands share the same three-cell footprint as their cuttable side walls.
"""
from underworks_palette import *

for name, levels in [('pallet', 1), ('pallet_stack', 3)]:
    im, p = canvas()
    for level in range(levels):
        y = 9-level*3
        p.rectangle((1, y+3, 14, y+5), fill=INK)
        for x in (2, 11):
            p.rectangle((x, y+3, x+2, y+4), fill='#574a36')
        for x in (1, 5, 9, 13):
            p.rectangle((x, y-2, min(x+2, 15), y+2), fill='#82704f')
            p.line((x, y-2, x, y+2), fill='#a08a60')
            p.point((x+1, y+1), fill=INK)
    save(im, name)
im, p = canvas()
for x,y in ((1,10),(6,8),(10,12),(4,13)):
    p.line((x,y,min(x+5,15),y-2), fill=WOOD, width=2)
p.point((12,8),fill=STEEL)
save(im,'pallet_broken')

for broken in (False, True):
    im,p=canvas()
    stones=[(1,9,6,14),(5,7,10,13),(10,9,15,14)]
    if not broken: stones += [(1,5,6,10),(6,3,12,9),(11,5,15,10),(3,1,8,6)]
    for x,y,x2,y2 in stones:
        p.polygon([(x,y+2),(x+2,y),(x2,y+1),(x2,y2),(x+1,y2)],fill='#65665d',outline=INK)
        p.line((x+2,y+1,x2-1,y+2),fill='#929081')
    if not broken:
        for x in (4,11):
            p.line([(x,2),(x+1,7),(x,14)],fill=STEEL)
        p.line([(1,9),(7,10),(14,9)],fill=IRON)
    else: p.line([(3,8),(2,12),(8,14),(12,11)],fill=IRON)
    save(im,'bound_rocks'+('_broken' if broken else ''))

for broken in (False,True):
    im,p=canvas()
    p.rectangle((0,0,15,15),fill='#303f3b')
    for x in range(2,15,4):
        p.line((x,0,x,15),fill='#58675d')
        p.line((x+1,0,x+1,15),fill='#26332f')
    p.line((0,0,0,15),fill=IRON)
    p.line((15,0,15,15),fill=INK)
    p.rectangle((5,4,6,6),fill='#755b41')
    if broken:
        p.polygon([(2,0),(14,0),(12,5),(15,7),(10,9),(14,15),(1,15),(4,11),(2,8),(6,4)],fill=(0,0,0,0))
        p.line([(2,0),(6,4),(2,8)],fill=IRON)
    save(im,'container_side'+('_broken' if broken else ''))
im,p=canvas()
p.rectangle((0,0,15,15),fill='#39413a')
for y in (3,9,15):
    p.line((0,y,15,y),fill='#4c5043')
p.point((3,2),fill='#756349');p.line((9,7,11,7),fill=INK)
save(im,'container_floor')

for end in (False,True):
    im,p=canvas((16,48))
    for band,color in enumerate(('#3b5148','#58685a','#34453e')):
        p.rectangle((0,band*16,15,band*16+15),fill=color)
    for y in range(3,47,4):
        p.line((0,y,15,y),fill='#687260' if y<32 else '#465a4d')
        p.line((0,y+1,15,y+1),fill='#2e4039')
    p.line((0,0,15,0),fill=IRON);p.line((0,47,15,47),fill=INK)
    if end:
        p.rectangle((0,0,3,47),fill=IRON)
        p.rectangle((0,17,6,30),fill=(0,0,0,0))
        p.line((6,17,6,30),fill=INK)
        p.rectangle((1,5,2,10),fill=BRASS)
        p.rectangle((1,37,2,42),fill=BRASS)
    for band in range(3):
        save(im.crop((0,band*16,16,band*16+16)),f'roof_container{"_end" if end else ""}_{"abc"[band]}')
for near in (False,True):
    im,p=canvas((48,16))
    p.rectangle((0,0,47,15),fill='#405348')
    for x in range(3,47,4):
        p.line((x,0,x,15),fill='#687260');p.line((x+1,0,x+1,15),fill='#2e4039')
    p.rectangle((0,0,47,2),fill=IRON)
    if near:
        p.rectangle((16,5,31,15),fill=(0,0,0,0))
        p.line((15,4,32,4),fill=INK)
        for x in (13,33):
            p.line((x,4,x,15),fill=BRASS)
    for band in range(3):
        save(im.crop((band*16,0,band*16+16,16)),f'roof_container_{"near" if near else "far"}_{"abc"[band]}')
