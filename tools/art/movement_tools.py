"""Knotted cord, resin-soled boots and a rabbit-foot charm, with transparent negative space."""
from pathlib import Path
from PIL import Image, ImageDraw
OUT=Path(__file__).resolve().parents[2]/'assets/graphics'
def canvas():
 im=Image.new('RGBA',(16,16));return im,ImageDraw.Draw(im)
def save(im,name): im.save(OUT/(name+'.png'))
cord='#b2a781'; light='#d9cc9a'; dark='#6c6550'
im,d=canvas()
d.polygon([(3,4),(11,3),(14,10),(8,14),(2,11)],fill='#484c3c')
for x in range(3,13,3): d.line((x,4,x+1,11),fill=cord)
for y in range(5,13,3): d.line((3,y,11,y-1),fill=cord)
d.line([(4,3),(7,1),(9,3)],fill=light);save(im,'throwing_net')
im,d=canvas()
for y in range(-12,24,6):
 d.line((1,y,14,y+13),fill=cord)
 d.line((1,y+13,14,y),fill=dark)
d.point((7,6),fill=light);d.point((7,12),fill=light);save(im,'net_flight')
im,d=canvas()
d.polygon([(5,1),(11,2),(14,12),(11,15),(2,14),(3,5)],outline=cord)
for a,b in [((4,4),(12,8)),((3,8),(13,12)),((3,12),(11,15)),((6,2),(4,14)),((10,2),(8,14)),((13,8),(11,14))]: d.line((a,b),fill=dark)
d.point((4,8),fill=light);d.point((11,12),fill=light);save(im,'net_caught')
im,d=canvas()
for x,y in [(2,2),(9,4)]:
 d.rectangle((x,y,x+3,y+7),fill='#876743')
 d.rectangle((x,y+6,x+5,y+8),fill='#98794c')
 d.line((x,y+9,x+5,y+9),fill='#849956',width=2)
 d.line((x+1,y+1,x+2,y+4),fill='#c2a67a')
save(im,'sticky_boots')
im,d=canvas()
d.line([(4,5),(3,2),(5,1),(7,3),(6,6)],fill=cord)
d.polygon([(6,5),(10,5),(12,10),(11,14),(6,14),(4,11)],fill='#d2c9a6')
d.line((5,7,9,6),fill='#aa8854',width=2)
d.line((7,10,7,13),fill='#7e7b65');d.line((10,10,10,13),fill='#7e7b65');save(im,'rabbit_charm')
