"""Quiet wood, cord and seed-shell traps: readable unset/armed/caught silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets/graphics'
def canvas():
    im=Image.new('RGBA',(16,16)); return im,ImageDraw.Draw(im)
def save(im,name): im.save(OUT/(name+'.png'))
cord='#bba273'; dark='#79603f'; pale='#dec695'; wood='#795637'; metal='#8b958c'
im,d=canvas()
d.ellipse((2,3,12,12),outline=dark,width=3);d.ellipse((3,3,12,11),outline=cord,width=2)
d.line([(4,8),(8,7),(13,11),(14,14)],fill=pale,width=1); save(im,'rope_snare')
im,d=canvas()
d.ellipse((1,4,14,13),outline=dark,width=3);d.ellipse((2,4,13,12),outline=cord,width=1)
d.line((12,10,15,2),fill=pale,width=1);d.rectangle((14,1,15,4),fill=wood);save(im,'snare_set')
im,d=canvas()
d.ellipse((4,3,11,12),outline=cord,width=2);d.line([(6,4),(11,8),(5,10),(10,13)],fill=pale,width=2)
d.line((4,8,1,12),fill=dark,width=2);save(im,'snare_tight')
for ready in (False,True):
 im,d=canvas();d.polygon([(2,4),(11,4),(14,8),(11,12),(2,12)],fill=wood)
 d.line((2,11,11,11),fill=dark,width=2)
 d.line([(3,6),(5,9),(7,6),(9,9)],fill=metal,width=1)
 if ready: d.polygon([(8,6),(11,6),(11,4),(15,8),(11,12),(11,10),(8,10)],fill=pale)
 else: d.line((3,3,12,3),fill=cord,width=2)
 save(im,'spring_ready' if ready else 'spring_trap')
for ready in (False,True):
 im,d=canvas()
 if ready:
  d.line((1,8,14,8),fill=cord,width=2);d.line((8,1,8,14),fill=cord,width=2)
 d.polygon([(4,6),(11,6),(12,10),(8,14),(3,10)],fill='#a88b4e')
 d.polygon([(3,5),(5,3),(10,3),(12,5),(12,7),(3,7)],fill='#695936')
 d.line((5,8,6,11),fill=pale,width=1);d.line((8,3,10,1),fill=dark,width=1)
 if ready: d.rectangle((7,5,8,6),fill='#d8b981')
 save(im,'acorn_ready' if ready else 'acorn_mine')
