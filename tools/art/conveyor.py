"""Low-contrast roller strips and two chunky workshop tools, native 16px."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets'/'graphics'
for name,frame in [('conveyor',0),('conveyor_moving',1),('conveyor_off',2),('conveyor_brake',3)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((0,1,15,14),fill='#303935');p.rectangle((0,4,15,11),fill='#424a41')
 p.line((0,2,15,2),fill='#727665');p.line((0,13,15,13),fill='#555f54')
 for x in range(frame%2*2,16,4):
  p.line((x,1,x,3),fill='#97957a');p.line((x,12,x,14),fill='#737b68')
 arrow='#9e9a74' if frame<2 else '#616958'
 p.polygon([(6,5),(10,8),(6,10),(7,8)],fill=arrow)
 if frame==2:p.rectangle((1,6,3,9),fill='#48412e');p.point((2,7),fill='#b59655')
 if frame==3:
  p.rectangle((3,5,6,10),fill='#714b39');p.line((4,5,5,5),fill='#b88d58')
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.line([(3,12),(3,6),(10,6),(10,3),(14,3)],fill='#929e88',width=2)
p.rectangle((1,10,4,14),fill='#8c7046');p.line((1,10,1,13),fill='#c1a774')
p.rectangle((12,1,14,5),fill='#53665b');p.point((4,6),fill='#d1c6a0')
im.save(root/'belt_crank.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(2,6),(10,3),(14,5),(14,10),(4,13),(2,11)],fill='#644536')
p.polygon([(3,6),(10,4),(13,6),(5,9)],fill='#a48150')
p.line((5,11,12,9),fill='#373f35',width=2);p.point((6,7),fill='#d0b77a')
im.save(root/'brake_shoe.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(4,5),(11,6),(9,9),(12,11),(5,10),(6,7)],fill='#4a5345')
p.line((5,5,10,6),fill='#7b8066')
im.save(root/'debris_belt_rubber.png')
