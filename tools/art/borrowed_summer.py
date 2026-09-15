"""A small corked jar holding a sun; follows the established sparse 16px item palette."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.rectangle((6,1,9,3),fill='#958263')
d.line([(5,4),(5,6),(3,8),(3,12),(5,14),(11,14),(13,12),(13,8),(10,5),(10,4)],fill='#8baca5')
d.polygon([(5,7),(11,7),(12,9),(12,12),(10,13),(5,13),(4,11),(4,9)],fill='#4e5746')
d.rectangle((7,8,9,10),fill='#e5bb69');d.point((8,9),fill='#f3df9c')
for x,y in [(8,6),(8,12),(5,9),(11,9)]:d.point((x,y),fill='#c89351')
d.point((4,8),fill='#cad3b5');d.point((11,12),fill='#aec1a8')
im.save(root/'borrowed_summer.png')
mote=Image.new('RGBA',(16,16));d=ImageDraw.Draw(mote)
d.line((7,3,7,11),fill='#d8b35f');d.line((3,7,11,7),fill='#d8b35f');d.rectangle((6,6,8,8),fill='#f0d899')
mote.save(root/'summer_mote.png')
