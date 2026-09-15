"""Directional stone charge and work-worn fuse scissors, native 16px sprites."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,lit,urgent in [('quarry_charge',False,False),('quarry_charge_lit',True,False),('quarry_charge_ready',True,True)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((3,5,10,11),fill='#784334');p.line([(3,5),(10,5)],fill='#b5764a')
 p.line([(3,8),(10,8)],fill='#3e362a');p.line([(3,11),(10,11)],fill='#3e362a')
 p.rectangle((5,5,6,11),fill='#ad9d75');p.rectangle((9,5,10,11),fill='#ad9d75')
 p.polygon([(11,5),(14,8),(11,11)],fill='#aea78a')
 p.line([(3,7),(1,6),(1,3),(4,2)],fill='#928a65')
 if lit:
  p.line([(4,1),(5,2),(4,3)],fill='#f0bd55');p.point((4,2),fill='#faf0bc')
 if urgent:p.rectangle((6,6,8,7),fill='#eaae55')
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.ellipse((1,2,6,7),outline='#b39758',width=2);p.ellipse((1,9,6,14),outline='#b39758',width=2)
p.line([(5,5),(13,12)],fill='#858c85',width=2);p.line([(5,11),(14,3)],fill='#c4c9ad',width=2)
p.point((8,8),fill='#484b40');im.save(root/'fuse_scissors.png')
