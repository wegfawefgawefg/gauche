"""Original 16px frostbound pilgrim, exposed cloth poses, and torn wool."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['frozen_pilgrim','pilgrim_thawing','pilgrim_thawed','pilgrim_strike','pilgrim_freezing']:
    im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
    cold=name=='frozen_pilgrim'; thaw=name=='pilgrim_thawing'; freeze=name=='pilgrim_freezing'; strike=name=='pilgrim_strike'
    # CLOTH: Hunched hood and split travel robe; one pale hand reaches ahead.
    p.polygon([(5,3),(7,1),(10,2),(11,5),(10,7),(12,12),(10,13),(8,12),(5,13),(3,11)],fill='#6b5750')
    p.polygon([(6,3),(9,2),(10,5),(6,6)],fill='#8e7970')
    p.rectangle((7,4,10,5),fill='#2f343d')
    p.point((10,5),fill='#c9baa1')
    p.line([(5,7),(4,11),(6,11)],fill='#a08b75')
    p.line([(9,8),(10,11)],fill='#443b3f')
    p.line([(5,13),(4,14)],fill='#b7ad98')
    p.line([(10,13),(12,14)],fill='#b7ad98')
    if strike:
        p.line([(10,7),(12,5),(14,5)],fill='#b7ad98',width=2)
        p.line([(13,3),(15,3)],fill='#dfd6b8')
        p.line([(4,7),(2,6),(2,4)],fill='#a08b75')
    else:
        p.line([(10,7),(12,9),(13,8)],fill='#b7ad98')
    if cold or thaw or freeze:
        crust='#92aab5' if cold else '#718d9f'
        p.polygon([(4,3),(7,0),(11,2),(12,5),(10,6),(9,3),(6,4),(5,8),(3,9)],fill=crust)
        p.line([(5,3),(7,1),(10,2)],fill='#c5d1cd')
        p.polygon([(3,9),(6,7),(7,10),(8,12),(5,14),(3,12)],fill=crust)
        p.line([(4,10),(5,12)],fill='#c5d1cd')
        if cold:
            p.polygon([(10,7),(13,9),(12,12),(10,11)],fill='#8aa2ae')
        elif thaw:
            p.point((1,11),fill='#a9c0c8'); p.point((12,14),fill='#a9c0c8')
        else:
            p.line([(13,1),(13,3)],fill='#c5d1cd'); p.point((14,2),fill='#c5d1cd')
    im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(5,7),(7,5),(8,7),(10,6),(10,9),(7,10),(5,9)],fill='#8e7970')
p.line([(6,7),(7,8),(9,7)],fill='#b4a18a');p.point((9,10),fill='#6b5750')
im.save(root/'debris_wool_tuft.png')
