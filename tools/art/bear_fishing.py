"""Native 16px river fish and working poses; preserve the original bear sprite."""
from underworks_palette import *

im, p = canvas()
p.polygon(((2,7),(5,5),(11,6),(12,7),(14,5),(14,10),(12,8),(6,10),(2,8)), fill=INK)
p.polygon(((3,7),(6,6),(10,6),(12,8),(6,9),(3,8)), fill='#789487')
p.line((4,8,10,8), fill='#b9bda0')
p.point((4,7),fill='#1f2a25')
p.line((6,5,8,5),fill='#526f68')
p.line((13,7,14,6),fill='#789487')
p.line((13,8,14,9),fill='#789487')
save(im,'river_fish')

for pose in ('watch','paw','swat','eat'):
    im = Image.open(ROOT/'bear.png').convert('RGBA')
    p = ImageDraw.Draw(im)
    # A lowered muzzle, broad paws and a raised/swiping silhouette communicate work.
    p.rectangle((5,8,11,10),fill='#80543a')
    p.line((5,9,6,9),fill='#e7ca8d')
    p.line((10,9,11,9),fill='#e7ca8d')
    p.rectangle((7,10,9,12),fill='#a6794b')
    p.point((8,10),fill=INK)
    if pose=='watch':
        p.line((3,11,4,13),fill='#a6794b',width=2)
        p.line((12,11,11,13),fill='#a6794b',width=2)
    elif pose=='paw':
        p.polygon(((11,8),(12,3),(14,2),(15,4),(14,10)),fill=INK)
        p.line((13,4,12,9),fill='#a6794b',width=2)
        p.line((13,2,14,3),fill='#e7ca8d')
    elif pose=='swat':
        p.polygon(((10,10),(13,9),(15,11),(15,14),(12,14),(10,12)),fill=INK)
        p.line((12,11,14,12),fill='#a6794b',width=2)
        p.point((14,13),fill='#e7ca8d')
    else:
        p.line((4,12,11,12),fill='#789487',width=2)
        p.line((5,13,10,13),fill='#b9bda0')
        p.point((5,12),fill=INK)
        p.rectangle((3,11,4,13),fill='#a6794b')
        p.rectangle((11,11,12,13),fill='#a6794b')
    save(im,'bear_fish_'+pose)
