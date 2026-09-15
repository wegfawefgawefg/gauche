"""A dull silver handbell, with the waterline engraved on its skirt."""
from pathlib import Path
from PIL import Image,ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.rectangle((2,7,7,9),fill='#867754');d.line((2,7,7,7),fill='#b1a17a')
d.polygon([(7,7),(9,5),(12,5),(14,3),(14,13),(12,11),(9,11),(7,9)],fill='#6e999e')
d.line([(8,7),(10,5),(12,5)],fill='#c1d3c9');d.line((14,3,14,13),fill='#b5cbc7')
d.line((11,6,11,10),fill='#355962');d.point((15,8),fill='#d0be88')
im.save(root/'stillwater_bell.png')
