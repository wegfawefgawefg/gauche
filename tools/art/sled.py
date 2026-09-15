"""Top-down wood sled, with two iron runners and a blunt rope bow; faces right."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.line([(1,3),(12,3),(14,5)],fill='#75888c',width=2)
d.line([(1,12),(12,12),(14,10)],fill='#75888c',width=2)
d.rectangle((3,4,11,11),fill='#513e32')
d.rectangle((4,4,10,6),fill='#b48652');d.rectangle((4,8,10,10),fill='#967044')
d.line((4,4,10,4),fill='#d0ae70');d.line((4,8,10,8),fill='#b8945d')
d.line((3,5,3,10),fill='#c4b690');d.line((11,5,11,10),fill='#c4b690')
d.line([(13,5),(15,7),(15,8),(13,10)],fill='#a29573')
im.save(root/'sled.png')
