"""Flat wood and rope, quiet negative gaps between the planks."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
for x in [3,6,9]:
 d.rectangle((x,3,x+2,12),fill='#8e8162');d.line((x,3,x,11),fill='#b2a078')
d.line((2,5,12,5),fill='#c4b58a');d.line((2,10,12,10),fill='#c4b58a')
d.line([(12,5),(14,6),(14,9),(12,10)],fill='#746849')
im.save(root/'folded_bridge.png')
for vertical in [False,True]:
 im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
 if vertical:
  for y in [0,4,8,12]:
   d.rectangle((2,y,13,y+2),fill='#8e8162');d.line((2,y,13,y),fill='#ad9d78')
  d.line((3,0,3,15),fill='#c0b18a');d.line((12,0,12,15),fill='#c0b18a')
 else:
  for x in [0,4,8,12]:
   d.rectangle((x,2,x+2,13),fill='#8e8162');d.line((x,2,x,13),fill='#ad9d78')
  d.line((0,3,15,3),fill='#c0b18a');d.line((0,12,15,12),fill='#c0b18a')
 im.save(root/('bridge_plank_v.png' if vertical else 'bridge_plank_h.png'))
