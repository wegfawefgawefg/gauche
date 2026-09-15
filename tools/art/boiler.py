"""Cold-work boiler crew, sealed vessel states and compact maintenance supplies."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
for name,pose in [('boiler_porter',0),('porter_push',1),('porter_warn',2)]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((5,2,10,5),fill='#bcbfa6');p.rectangle((4,1,11,2),fill='#6a827d')
 p.rectangle((5,3,10,4),fill='#435356');p.point((6,3),fill='#d3a95c');p.point((9,3),fill='#d3a95c')
 p.rectangle((4,6,11,11),fill='#7f8471');p.rectangle((6,6,9,11),fill='#ad9970')
 p.line([(5,12),(5,14),(3,14)],fill='#9cab9d',width=2);p.line([(10,12),(10,14),(12,14)],fill='#9cab9d',width=2)
 if pose==1:
  p.line([(4,7),(2,5),(2,3)],fill='#c5c5a7',width=2);p.line([(11,7),(13,5),(13,3)],fill='#c5c5a7',width=2)
 elif pose==2:
  p.line([(4,7),(1,7),(1,3)],fill='#c5c5a7',width=2);p.line([(0,2),(3,2)],fill='#b0bbc0',width=2)
 else:
  p.line([(3,7),(2,10)],fill='#b9bbaa',width=2);p.line([(12,7),(13,10)],fill='#b9bbaa',width=2)
 im.save(root/(name+'.png'))
for name,glow in [('boiler_tank','#788886'),('boiler_hot','#c29455'),('boiler_tell','#edbc68'),('boiler_plugged','#83a8a1')]:
 im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
 p.rectangle((4,1,10,2),fill='#99a999');p.rectangle((3,3,12,12),fill='#6f7c73')
 p.line([(3,4),(12,4)],fill='#b2bba4');p.line([(3,11),(12,11)],fill='#a4ab94')
 p.rectangle((4,6,7,9),fill='#485453');p.rectangle((5,7,6,8),fill=glow)
 p.rectangle((9,5,10,9),fill='#3d494b');p.line([(9,9),(9,6)],fill=glow)
 p.rectangle((3,13,5,14),fill='#9b9f89');p.rectangle((10,13,12,14),fill='#9b9f89')
 if name=='boiler_plugged': p.line([(7,2),(9,2)],fill='#d4c6a2',width=2)
 im.save(root/(name+'.png'))
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((10,7,14,9),fill='#a4a48a');p.rectangle((14,6,15,10),fill='#d1bf88');p.line([(15,7),(15,9)],fill='#5e6257')
im.save(root/'boiler_nozzle.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.ellipse((3,3,12,12),outline='#cea85a',width=2);p.line([(4,4),(11,11)],fill='#a78951');p.line([(11,4),(4,11)],fill='#a78951');p.rectangle((7,7,9,9),fill='#e4c98b')
im.save(root/'pressure_valve.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.polygon([(5,4),(11,6),(10,13),(4,11)],fill='#b5b89c');p.rectangle((5,2,8,4),fill='#7a968d')
p.line([(5,7),(9,8)],fill='#7c9c91',width=2);p.point((7,11),fill='#d5cfac')
im.save(root/'sealant.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((2,1,13,14),fill='#657873');p.rectangle((4,3,11,12),fill='#81978a')
p.line([(7,3),(7,12)],fill='#445754');p.rectangle((8,7,9,8),fill='#d2aa5e')
p.line([(4,4),(6,4)],fill='#b3b8a0');p.line([(3,14),(12,14)],fill='#b6b5a0')
im.save(root/'maintenance_locker.png')
im=Image.new('RGBA',(16,16));p=ImageDraw.Draw(im)
p.rectangle((6,6,9,8),fill='#c0a065');p.rectangle((7,9,8,11),fill='#917a52');p.point((6,6),fill='#e0c48b')
im.save(root/'debris_brass_rivet.png')
