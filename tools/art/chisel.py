"""A bronze-handled chisel. Its pale beveled edge points right when held."""
from pathlib import Path
from PIL import Image, ImageDraw
image=Image.new('RGBA',(16,16));p=ImageDraw.Draw(image)
p.rectangle((1, 6, 6, 10),fill='#473f36')
p.rectangle((2, 7, 7, 9),fill='#ac8751')
p.line([(2, 7), (5, 7)],fill='#dcc394')
p.rectangle((7, 6, 8, 10),fill='#6d7d80')
p.polygon([(9, 7),(14, 6),(15, 8),(14, 10),(9, 9)],fill='#a4bec3')
p.line([(10, 7),(14, 6),(15, 8)],fill='#d1ddd0')
image.save(Path(__file__).resolve().parents[2]/'assets/graphics/chisel.png')
