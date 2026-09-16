"""A grouped oak crown and vertical bark at native pixel scales; no raster model."""
from pathlib import Path
from PIL import Image,ImageDraw
import random
OUT=Path(__file__).resolve().parents[2]/'assets'/'graphics'
r=random.Random(7241)
size=320
im=Image.new('RGBA',(size,size),'#253c29');p=ImageDraw.Draw(im)
mask=Image.new('L',(size,size));m=ImageDraw.Draw(mask);m.ellipse((2,2,317,317),fill=255)
for y in range(-12,size+20,18):
    for x in range(-12,size+20,18):
        cx=x+r.randrange(-7,8);cy=y+r.randrange(-7,8);rad=r.randrange(11,24)
        p.ellipse((cx-rad,cy-rad//2,cx+rad,cy+rad),fill=r.choice(['#203727','#293f29','#2d442b']))
        p.ellipse((cx-rad+2,cy-rad//2,cx+rad-2,cy+rad-5),fill=r.choice(['#3b542f','#455d33','#354e2e','#4b6335']))
        p.arc((cx-rad+4,cy-rad//2+2,cx+rad-4,cy+rad-8),180,280,fill='#627747',width=2)
for _ in range(1700):
    x,y=r.randrange(size),r.randrange(size)
    p.line((x,y,x+2,y-1),fill=r.choice(['#5b7040','#334a2c','#6a7d49','#293e28']))
im.putalpha(mask);im.save(OUT/'giant_tree_canopy.png')
im=Image.new('RGBA',(16,48),'#50442e');p=ImageDraw.Draw(im)
for x in [1,6,12]:
    p.line([(x,0),(x-1,12),(x+1,28),(x,47)],fill='#342f24',width=2)
    p.line([(x+2,0),(x+1,14),(x+3,31),(x+2,47)],fill='#78613d')
p.line([(8,11),(10,16),(8,22),(7,19),(8,11)],fill='#403725')
p.line((3,38,5,33),fill='#96804e');p.line((13,41,15,36),fill='#6c613a')
im.save(OUT/'giant_tree_bark.png')
