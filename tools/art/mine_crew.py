"""Broad soot-stained pickhands and a tall cream-coated shift foreman, 16px."""
from pathlib import Path
from PIL import Image, ImageDraw
ROOT=Path(__file__).resolve().parents[2]/'assets'/'graphics'

def worker(name,leader=False,pose='idle'):
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    p.rectangle((4,12,6,14),fill='#665246');p.rectangle((9,12,11,14),fill='#665246')
    p.polygon([(4,7),(11,7),(12,12),(3,12)],fill='#a9a184' if leader else '#72604f')
    p.line([(7,8),(7,12)],fill='#473e37')
    p.rectangle((5,3,10,7),fill='#8e7960')
    p.rectangle((5,5,10,6),fill='#423832');p.point((8,5),fill='#d2c7a8')
    p.rectangle((4,2,11,3),fill='#b7aa7e' if leader else '#a8843c')
    p.line([(3,4),(12,4)],fill='#6f6045')
    p.point((6,2),fill='#f3d798')
    if leader:
        p.line([(4,8),(5,10)],fill='#caaf66');p.rectangle((9,9,10,10),fill='#644737')
    if pose=='carry':
        p.polygon([(3,8),(11,8),(12,12),(3,12),(2,10)],fill='#423e32')
        p.line([(3,9),(11,9),(10,12),(4,12),(3,9)],fill='#aa8f62')
        p.polygon([(4,8),(5,6),(7,7),(9,6),(11,8)],fill='#343d39')
        p.point((6,7),fill='#819086');p.point((9,7),fill='#66776b')
        p.line([(2,8),(3,10),(5,10)],fill='#b69b76')
        p.line([(12,8),(12,10),(10,10)],fill='#b69b76')
    elif pose=='raise':
        p.line([(11,8),(13,5)],fill='#9b8461',width=2)
        p.line([(13,8),(13,1)],fill='#8e6a46')
        if not leader: p.line([(9,1),(14,1),(15,3)],fill='#a8ada4')
        else: p.rectangle((11,0,14,3),fill='#705044')
    elif pose=='swing':
        p.line([(10,9),(15,10)],fill='#8e6a46',width=2)
        p.line([(14,7),(14,12)],fill='#a8ada4' if not leader else '#705044')
    elif pose=='whistle':
        p.line([(10,8),(12,6),(10,6)],fill='#a78d6a',width=2)
        p.point((11,5),fill='#d4c289');p.line([(13,4),(14,3)],fill='#c9c7a2')
    else:
        p.line([(12,8),(12,13)],fill='#8e6a46')
        if not leader: p.line([(9,8),(13,7),(15,8)],fill='#929b94')
    im.save(ROOT/(name+'.png'))

worker('pickhand');worker('pickhand_raise',pose='raise');worker('pickhand_swing',pose='swing')
worker('shift_foreman',True);worker('foreman_whistle',True,'whistle')
worker('foreman_raise',True,'raise');worker('foreman_swing',True,'swing')

worker('pickhand_carry',pose='carry')
