"""Compact pneumatic riveter and a hunched, goggle-wearing firing crew."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,pose in [('rivet_gunner','idle'),('gunner_brace','brace'),('gunner_fire','fire'),('gunner_reload','reload')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((3,12,6,14),fill='#4d4536');p.rectangle((10,12,13,14),fill='#4d4536')
 p.polygon([(3,6),(10,6),(12,12),(2,12)],fill='#857447')
 p.line((4,7,4,11),fill='#b19c62');p.rectangle((6,10,10,12),fill='#4d554e')
 p.rectangle((4,2,10,5),fill='#bd9d72');p.rectangle((3,1,10,2),fill='#777e68')
 p.rectangle((5,3,11,4),fill='#343f3b');p.line((6,3,10,3),fill='#aabdb3')
 # A pressure bottle and hose distinguish the gunner from a pickhand.
 p.rectangle((1,5,3,9),fill='#4c6861');p.line((1,5,2,5),fill='#a1ad92')
 p.line([(2,10),(4,12),(8,11)],fill='#282f2b',width=2)
 if pose=='reload':
  p.rectangle((9,8,11,13),fill='#667b72');p.rectangle((10,6,12,9),fill='#c2c4a6')
  p.rectangle((6,7,10,8),fill='#b69a70');p.rectangle((12,10,14,11),fill='#b79a56')
 else:
  y=8 if pose=='idle' else 7
  p.rectangle((6,y,11,y+2),fill='#b1966c')
  if pose=='brace':p.line((4,13,4,15),fill='#aa9666')
  if pose=='fire':p.point((15,y+1),fill='#dbe1bb')
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
