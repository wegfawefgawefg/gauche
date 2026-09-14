"""A blunt root-colored segmented worm, with a distinct split head and bite pose."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
for part in ['head', 'body', 'bite']:
    im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
    d.polygon([(1,6),(3,3),(10,3),(14,6),(14,10),(11,13),(4,13),(1,10)], fill='#80634f')
    d.polygon([(3,5),(6,4),(10,4),(12,6),(11,8),(3,8)],fill='#a68b66')
    d.polygon([(3,10),(11,10),(13,9),(11,12),(4,12)],fill='#594f3d')
    for x in [3,6,9]:
        d.line([(x,5),(x-1,8),(x,11)],fill='#594f3d')
        d.line((x+1,5,x+1,7),fill='#b3a079')
    if part != 'body':
        d.polygon([(10,5),(13,4),(15,6),(15,10),(13,12),(10,10)],fill='#ac8e68')
        d.point((12,5),fill='#e0c587');d.point((12,10),fill='#e0c587')
        d.line((14,7,14,9),fill='#352e29')
    if part == 'bite':
        d.rectangle((12,6,15,10),fill='#49332c')
        d.point((13,6),fill='#e2d0a2');d.point((15,6),fill='#e2d0a2')
        d.point((13,10),fill='#e2d0a2');d.point((15,10),fill='#e2d0a2')
    im.save(OUT / f'burrow_worm_{part}.png')
