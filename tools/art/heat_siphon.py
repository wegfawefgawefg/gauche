"""Sparse brass pump with a dark insulated handle and six small heat windows."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for charged in [False,True]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 d.rectangle((3,5,11,10),fill='#5c6156')
 d.rectangle((4,4,10,4),fill='#ad9466');d.line((3,11,11,11),fill='#807256')
 d.rectangle((11,6,14,9),fill='#ad9466');d.rectangle((14,7,15,8),fill='#d2c2a2')
 d.rectangle((1,6,2,8),fill='#92947c');d.rectangle((0,9,1,12),fill='#494e46')
 d.rectangle((5,12,7,14),fill='#766e52');d.line((5,12,7,12),fill='#b9a274')
 for x in [5,7,9]:
  for y in [6,8]:d.point((x,y),fill='#eeb66b' if charged else '#303e3b')
 im.save(root/('siphon_charged.png' if charged else 'heat_siphon.png'))
