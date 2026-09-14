"""A compact iron stove, a hand-sized coal lump and dark ash fragments."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for lit in [False,True]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((10,0,12,5),fill='#5e716e');p.line([(11,0),(11,4)],fill='#83948a')
 p.rectangle((3,5,13,12),fill='#53645e');p.rectangle((2,4,13,5),fill='#899582')
 p.rectangle((4,7,10,10),fill='#373e36' if not lit else '#9c5933')
 if lit:
  p.rectangle((5,8,9,9),fill='#e3a154');p.point((6,8),fill='#f3ce83')
 p.line([(6,7),(6,10)],fill='#4b4d3e');p.line([(9,7),(9,10)],fill='#4b4d3e')
 p.rectangle((3,13,4,14),fill='#697567');p.rectangle((11,13,12,14),fill='#697567')
 p.point((12,8),fill='#b3a879');im.save(root/('stove_lit.png' if lit else 'stove.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(3,9),(5,5),(9,4),(12,7),(11,11),(6,12)],fill='#596265')
p.polygon([(5,6),(8,5),(8,8),(4,9)],fill='#87908a');p.line([(8,8),(10,9)],fill='#3a4549')
im.save(root/'coal_lump.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(6,6),(9,7),(9,9),(6,10),(4,8)],fill='#5e6461');p.point((6,7),fill='#8c9180')
im.save(root/'debris_coal.png')
