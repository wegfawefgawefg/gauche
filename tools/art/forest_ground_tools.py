"""Quiet seed and thorn silhouettes; tiny ground marks remain distinct from items."""
from pathlib import Path
from PIL import Image, ImageDraw

OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'
def canvas():
    image=Image.new('RGBA',(16,16))
    return image,ImageDraw.Draw(image)

image,d=canvas()
d.polygon([(5,2),(10,2),(9,5),(12,8),(12,13),(3,13),(3,8),(6,5)],fill='#947b53')
d.line((5,5,10,5),fill='#4b4732')
for x,y in [(5,8),(8,7),(10,9),(6,11),(9,11)]:
    d.line((x,y,x+1,y),fill='#dfc488')
image.save(OUT/'bird_seed.png')
image,d=canvas()
for x,y in [(4,7),(8,5),(11,8),(6,10),(9,11),(2,11),(12,12)]:
    d.line((x,y,x+1,y),fill='#bea56f')
    d.point((x,y+1),fill='#78623e')
image.save(OUT/'bird_seed_pile.png')
for name,points in [('thorn_caltrops',[(4,9),(8,6),(11,11)]),
                    ('thorn_patch',[(3,5),(11,7),(6,12)])]:
    image,d=canvas()
    for x,y in points:
        d.line([(x-2,y+1),(x+2,y-1)],fill='#b9ad7b')
        d.line([(x,y+2),(x,y-3)],fill='#b9ad7b')
        d.line([(x-2,y-1),(x+2,y+2)],fill='#7b704e')
        d.point((x,y),fill='#e2d5a2')
    image.save(OUT/f'{name}.png')
