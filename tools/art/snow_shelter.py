"""Packed snow windbreak: broad, quiet planes rather than granular snow noise."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.polygon([(2,5),(5,3),(11,3),(14,6),(13,13),(4,14),(2,11)],fill='#526f7a')
d.polygon([(3,6),(6,4),(12,4),(13,6),(11,8),(4,8)],fill='#c8d8d7')
d.rectangle((5,8,11,12),fill='#7c939d');d.line((7,3,7,13),fill='#c0aa7b',width=2)
im.save(root/'snow_shelter.png')
for vertical in [False,True]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 if vertical:
  d.polygon([(5,0),(11,0),(13,4),(12,16),(5,16),(3,12),(4,3)],fill='#5c7985')
  d.polygon([(5,0),(9,0),(10,5),(9,16),(5,16),(4,10)],fill='#bfd3d5')
  d.line((6,1,6,7),fill='#e0e5d9');d.line((7,10,7,14),fill='#d8e2d9')
 else:
  d.polygon([(0,5),(4,3),(12,4),(16,5),(16,11),(11,13),(0,12)],fill='#5c7985')
  d.polygon([(0,5),(5,4),(12,5),(16,5),(16,8),(11,9),(0,8)],fill='#bfd3d5')
  d.line((1,5,7,5),fill='#e0e5d9');d.line((10,6,14,6),fill='#d8e2d9')
 im.save(root/('snow_wall_v.png' if vertical else 'snow_wall_h.png'))
