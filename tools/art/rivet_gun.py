"""Compact pneumatic riveter and a hunched, goggle-wearing firing crew."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,pose in [('rivet_gunner','idle'),('gunner_brace','brace'),('gunner_fire','fire'),('gunner_reload','reload')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 # Wide planted feet, a lowered helmet and raised hands make the half-second
 # brace distinct from idle at native scale. The game draws the actual gun.
 crouch=pose in ('brace','fire');recoil=1 if pose=='fire' else 0
 p.rectangle((1 if crouch else 3,12,5 if crouch else 6,14),fill='#302e2a')
 p.rectangle((10,12,14 if crouch else 12,14),fill='#302e2a')
 p.line((2 if crouch else 4,13,5,13),fill='#96805b')
 p.line((11,13,14 if crouch else 12,13),fill='#96805b')
 p.polygon([(4-recoil,6),(10-recoil,6),(12-recoil,12),(2,12)],fill='#857447')
 p.line((4-recoil,7,4-recoil,11),fill='#b19c62')
 p.rectangle((6,10,10,12),fill='#4d554e')
 hx=5-recoil;hy=4 if crouch else 2
 if pose=='reload':hx=7;hy=3
 p.rectangle((hx,hy,hx+5,hy+3),fill='#bd9d72')
 p.rectangle((hx-1,hy-1,hx+5,hy),fill='#777e68')
 p.line((hx-1,hy,hx+6,hy),fill='#bac1a2')
 p.rectangle((hx+1,hy+1,hx+6,hy+2),fill='#343f3b')
 p.line((hx+2,hy+1,hx+5,hy+1),fill='#cfdbc6')
 # Pressure bottle, hose and amber gauge are visible in every pose.
 p.rectangle((1,5,3,9),fill='#4c6861');p.line((1,5,2,5),fill='#a1ad92')
 p.point((2,7),fill='#dfb75c' if crouch else '#8a8e65')
 p.line([(2,10),(4,12),(8,11)],fill='#282f2b',width=2)
 if pose=='reload':
  p.rectangle((11,8,13,13),fill='#667b72')
  p.rectangle((11,4,13,8),fill='#c2c4a6')
  p.line((12,5,12,8),fill='#797b60')
  p.line([(6,8),(9,6),(11,5)],fill='#b69a70',width=2)
  p.line((8,10,11,11),fill='#b69a70',width=2)
 elif crouch:
  p.line([(5-recoil,9),(8-recoil,7),(12-recoil,7)],fill='#c1a779',width=2)
  p.line([(7-recoil,10),(10-recoil,9),(13-recoil,9)],fill='#8e794f',width=2)
 else:
  p.line([(6,7),(7,10),(11,10)],fill='#b1966c',width=2)
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((2,6,11,9),fill='#536a60');p.rectangle((3,5,10,6),fill='#a3b59d')
p.rectangle((11,6,14,8),fill='#c3c6ac');p.line((14,6,14,8),fill='#727e70')
p.rectangle((6,10,8,13),fill='#5c4936');p.rectangle((2,9,5,11),fill='#85927b')
p.line([(2,7),(0,8),(0,12),(5,12)],fill='#373f36');p.point((9,7),fill='#d0b96b')
im.save(root/'rivet_gun.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line((5,7,11,7),fill='#d5d9b4',width=2);p.line((5,6,5,9),fill='#869990',width=2);p.point((12,7),fill='#fff0bb')
im.save(root/'rivet.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((5,5,10,10),fill='#797a56');p.rectangle((6,5,9,8),fill='#c2b079');p.line((5,10,10,10),fill='#525c4c')
im.save(root/'debris_rivet_casing.png')
