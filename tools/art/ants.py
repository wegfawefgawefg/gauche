"""Six-legged forest workers, sugar porters and whistle captains. Native pixel art."""
from underworks_palette import *
for captain in (False, True):
    for phase, suffix in enumerate(('', '_walk', '_tell', '_bite', '_carry')):
        im,p=canvas(); lift=1 if phase==2 else 0
        for side in (-1,1):
            for leg in range(3):
                y=5+leg*3; tip=y+(1 if (leg+phase)%2 else -1)
                p.line([(7+side*2,y),(7+side*5,tip),(7+side*6,tip+2)], fill='#624e37')
                p.point((7+side*5,tip),fill='#a08552')
        p.ellipse((4,1,11,7), fill='#392b24');p.ellipse((5,1,9,5),fill='#795333')
        p.ellipse((5,7,10,10),fill='#9a6d40' if not captain else '#b79d54')
        p.rectangle((6,6,8,8),fill='#55412e')
        p.ellipse((4,9-lift,11,13-lift),fill='#5c402b')
        p.line([(5,11-lift),(2,10-lift),(1,7-lift)],fill='#b3945e')
        p.line([(10,11-lift),(13,10-lift),(14,7-lift)],fill='#b3945e')
        for x in (5,10):p.point((x,10-lift),fill='#f0c777' if phase==2 else '#d6bf83')
        for x,side in ((5,-1),(10,1)):
            p.line([(x,12-lift),(x+side,14-lift),(7+side,15 if phase==3 else 13)],fill='#d1bf89')
        if captain:
            p.rectangle((4,8,11,9),fill='#c0a25d');p.point((9,9),fill='#eee0ac')
            if phase==2:p.rectangle((10,12,14,13),fill='#d5c58b')
        if phase==4:
            p.polygon([(3,3),(10,1),(13,6),(10,10),(3,8)],fill='#a99f7b')
            p.polygon([(3,3),(9,2),(10,7),(3,8)],fill='#ddd5ae')
            p.line([(3,3),(9,2),(10,7)],fill='#f0e8cc')
        save(im,('ant_captain' if captain else 'ant')+suffix)
im,p=canvas()
p.ellipse((1,8,15,15),fill='#493c2b');p.polygon([(2,12),(4,7),(7,2),(11,3),(13,9),(15,12)],fill='#80714b')
p.line([(4,9),(7,3),(10,4)],fill='#a1935e');p.ellipse((5,8,11,13),fill='#272922')
for x,y in [(3,12),(11,7),(12,12),(7,5),(2,14),(9,14)]:p.point((x,y),fill='#b29a62')
save(im,'ant_nest')
for phase,name in enumerate(['ant_sugar','ant_sugar_low','ant_sugar_empty']):
    im,p=canvas();p.ellipse((1,10,14,15),fill='#65503a')
    cubes=[(2,9),(7,9),(10,7),(5,5),(1,12),(9,12)] if phase==0 else [(2,11),(9,12)] if phase==1 else []
    for x,y in cubes:
        p.rectangle((x,y,x+3,y+3),fill='#c5bb94');p.line([(x,y+3),(x,y),(x+3,y)],fill='#eee3be');p.point((x+3,y+3),fill='#8d8565')
    p.point((6,14),fill='#c5bb94');save(im,name)
# Medium ants wear a pale rope harness; the bundled sled is a one-cell body.
from pathlib import Path
from PIL import Image, ImageDraw
for suffix in ('', '_walk', '_tell', '_bite', '_carry'):
    im=Image.open(Path(__file__).resolve().parents[2]/'assets'/'graphics'/('ant'+suffix+'.png')).convert('RGBA');p=ImageDraw.Draw(im)
    p.line([(4,7),(7,10),(11,7)],fill='#d4c7a1');p.line([(5,6),(10,6)],fill='#95805e')
    save(im,'ant_puller'+suffix)
for full in (False,True):
    im,p=canvas()
    p.rectangle((1,3,14,14),fill='#382c25');p.rectangle((2,4,13,13),fill='#886444')
    for y in (6,9,12):p.line([(2,y),(13,y)],fill='#543e2d')
    if full:
        p.polygon([(3,8),(4,2),(11,1),(13,7),(12,12),(4,12)],fill='#c0b582')
        p.line([(4,8),(5,3),(10,2)],fill='#e5d8a7');p.line([(8,2),(7,7),(8,12)],fill='#786c49')
    for x in (4,11):p.line([(x,3),(x,14)],fill='#b19b70');p.point((x,14),fill='#ded1a3')
    save(im,'ant_load_full' if full else 'ant_load')
