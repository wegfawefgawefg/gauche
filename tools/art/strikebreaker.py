"""Stocky red-capped crew guard; the shield is a separate facing attachment."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,pose in [('strikebreaker','march'),('breaker_push','push'),('breaker_raise','raise'),('breaker_recover','rest'),('breaker_unshielded','bare')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((3,12,6,14),fill='#514d40');p.rectangle((10,12,13,14),fill='#514d40')
 p.polygon([(3,6),(12,6),(14,12),(2,12)],fill='#5a6058')
 p.line((4,8,4,11),fill='#92927a');p.rectangle((7,9,9,11),fill='#393f38')
 p.rectangle((5,3,11,6),fill='#af9571');p.rectangle((6,4,11,5),fill='#39382f')
 p.point((9,4),fill='#ddd2ad');p.rectangle((4,1,11,2),fill='#8c4738')
 p.line((3,3,12,3),fill='#b07953');p.rectangle((8,1,9,2),fill='#c7b273')
 if pose=='raise':
  p.line((12,8,13,2),fill='#9d7b50',width=2);p.rectangle((11,0,15,3),fill='#a4aa97')
 elif pose=='push':
  p.rectangle((11,7,15,9),fill='#a48e68');p.line((3,8,3,12),fill='#9b784e')
 elif pose=='rest':
  p.line((12,9,14,13),fill='#95734a',width=2);p.rectangle((12,12,15,14),fill='#858d81')
 else:
  p.line((13,7,13,12),fill='#a17e50');p.rectangle((11,6,15,8),fill='#969e8e')
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(3,2),(13,2),(14,5),(14,12),(11,14),(3,14)],fill='#373f3c')
p.rectangle((4,3,12,12),fill='#788275');p.rectangle((5,5,11,10),fill='#565e54')
p.line((12,3,12,12),fill='#b1b39a');p.line((4,3,4,12),fill='#939d8c')
p.rectangle((8,6,10,9),fill='#b69250');p.point((5,4),fill='#d0c8a7');p.point((5,11),fill='#d0c8a7')
im.save(root/'breaker_shield.png')
