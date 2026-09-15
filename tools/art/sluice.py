"""Top-down sluice grating and a small brass mechanical wedge. Quiet flat shapes."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
d.polygon([(2,11),(11,5),(13,5),(13,12),(2,13)],fill='#846539')
d.polygon([(2,10),(11,4),(13,4),(13,6),(4,12),(2,12)],fill='#c3ad73')
d.line((4,12,12,11),fill='#d8c186'); d.line((12,6,12,10),fill='#443b2b')
d.line((3,13,13,13),fill='#3c4844'); im.save(root/'emergency_doorstop.png')
for name,closed in [('sluice_gate',True),('sluice_open',False)]:
 im=Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
 for x in [0,13]:
  d.rectangle((x,0,x+2,15),fill='#34484c'); d.line((x,0,x,15),fill='#728d90')
  for y in [2,12]: d.point((x+1,y),fill='#b6c1ae')
 if closed:
  for y in [2,6,10,14]:
   d.rectangle((3,y,12,y+1),fill='#455c60'); d.line((3,y,12,y),fill='#849d9b')
  d.line((7,1,7,14),fill='#34484c')
 else:
  d.line((3,1,12,1),fill='#526c6c'); d.line((3,14,12,14),fill='#293c41')
 im.save(root/(name+'.png'))
