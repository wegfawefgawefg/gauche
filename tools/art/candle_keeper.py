"""Wax-masked chapel attendant, four persistent body poses and a wooden cabinet."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name in ['candle_keeper','keeper_tend','keeper_strike','keeper_dim']:
    im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
    dim=name=='keeper_dim';strike=name=='keeper_strike';tend=name=='keeper_tend'
    # COAT: Broad slate robe, two dark feet, a pale wax face under the hood.
    p.polygon([(5,6),(9,6),(11,13),(3,13)],fill='#52666c')
    p.polygon([(4,8),(6,7),(6,12),(3,12)],fill='#72827e')
    p.line([(5,14),(5,15)],fill='#99886c');p.line([(9,14),(10,14)],fill='#99886c')
    p.polygon([(4,3),(6,1),(9,2),(10,4),(9,7),(5,7)],fill='#34484e')
    p.polygon([(6,3),(9,3),(9,6),(7,7),(6,5)],fill='#d0c096')
    p.point((9,4),fill='#645f50' if dim else '#ffdb8b')
    p.line([(7,6),(7,8)],fill='#a89873')
    # LAMP: The weapon remains attached to a readable figure during the whole cast.
    lx,ly=(12,5) if strike else (13,10) if tend else (11,10)
    p.line([(8,8),(lx-1,ly+1)],fill='#a99c7c',width=2)
    p.rectangle((lx-1,ly,lx+1,ly+3),fill='#ab8b52')
    p.rectangle((lx,ly,lx,ly+2),fill='#e4c78a' if not dim else '#64583f')
    p.line([(lx,ly-2),(lx,ly)],fill='#645139')
    if not dim:
        p.polygon([(lx,ly-4),(lx-1,ly-2),(lx,ly),(lx+1,ly-2)],fill='#eb994b')
        p.point((lx,ly-1),fill='#ffe5a1')
    im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((2,2,13,13),fill='#65583e');p.rectangle((3,3,12,12),fill='#887348')
p.rectangle((4,4,11,10),fill='#3f4238');p.line([(3,11),(12,11)],fill='#b29a65')
for x in [5,8,10]:
    p.rectangle((x,6 if x==8 else 7,x,10),fill='#c5b185');p.point((x,5 if x==8 else 6),fill='#777050')
p.point((3,13),fill='#a89060');p.point((12,13),fill='#a89060');im.save(root/'candle_cabinet.png')
