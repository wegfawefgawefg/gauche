"""Nimble red-vested blasting monkeys with a curled tail, native 16px."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,pose in [('powder_monkey','idle'),('powder_light','light'),('powder_run','run'),('powder_scratch','scratch')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.line([(4,11),(2,12),(1,10),(1,7),(3,6)],fill='#9d7949',width=2)
 p.rectangle((5,11,7,14),fill='#876441');p.rectangle((10,11,11,14),fill='#876441')
 p.polygon([(5,7),(10,7),(12,12),(4,12)],fill='#994d36')
 p.line([(7,8),(7,11)],fill='#d2a966');p.point((9,9),fill='#b7a775')
 p.rectangle((5,3,10,7),fill='#846744');p.rectangle((4,4,5,5),fill='#ba8f5b')
 p.rectangle((6,4,11,6),fill='#d0b27a');p.point((8,4),fill='#302f28')
 p.line([(4,2),(10,2)],fill='#a59a70');p.rectangle((6,1,9,2),fill='#6d745f')
 if pose=='light':
  p.line([(10,8),(13,7),(13,5)],fill='#ba935c',width=2)
  p.rectangle((11,8,14,10),fill='#b77b48');p.point((14,6),fill='#f4cc6c')
 elif pose=='run':
  p.rectangle((5,12,7,14),fill=(0,0,0,0));p.rectangle((10,12,11,14),fill=(0,0,0,0))
  p.line([(6,11),(4,13),(3,13)],fill='#a07a4e',width=2)
  p.line([(10,11),(13,12),(14,10)],fill='#a07a4e',width=2)
  p.line([(10,8),(12,6)],fill='#ba935c',width=2)
 elif pose=='scratch':p.line([(10,8),(14,5)],fill='#d0b27a',width=2)
 im.save(root/(name+'.png'))
