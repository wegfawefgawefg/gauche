"""A hand-held detached runner and a pair of spiked shoe bindings."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.line([(3,5),(9,5)],fill='#8f8470',width=2)
d.rectangle((4,7,5,9),fill='#849b9e');d.rectangle((9,7,10,9),fill='#849b9e')
d.line([(2,10),(11,10),(14,7)],fill='#c0d1cf',width=2)
d.line([(3,12),(11,12),(14,9)],fill='#708f99')
im.save(root/'skate_blade.png')
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
for x,y in [(2,2),(9,5)]:
 d.rectangle((x,y,x+3,y+7),fill='#738a8c')
 d.line([(x,y+2),(x+3,y+2)],fill='#b19c77',width=2)
 d.line([(x,y+5),(x+3,y+5)],fill='#b19c77',width=2)
 d.line([(x-1,y+7),(x-1,y+9)],fill='#c0d2d0');d.line([(x+4,y+7),(x+4,y+9)],fill='#c0d2d0')
im.save(root/'crampons.png')
