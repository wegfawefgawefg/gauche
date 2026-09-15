"""Sparse metal fork and brittle crystal silhouettes in the existing Ice colors."""
from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[2]/'assets/graphics'
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.rectangle((2,7,7,9),fill='#746b58');d.line((2,7,7,7),fill='#a39877')
d.line([(7,8),(10,8),(11,4),(14,4)],fill='#b8cccd',width=2)
d.line([(10,8),(11,12),(14,12)],fill='#8badaf',width=2)
d.point((14,4),fill='#e3e4cd');im.save(root/'tuning_fork.png')
im=Image.new('RGBA',(16,16));d=ImageDraw.Draw(im)
d.polygon([(2,12),(1,8),(3,5),(6,11),(7,2),(10,0),(12,10),(14,6),(15,12),(10,15),(5,14)],fill='#506d82')
d.polygon([(7,11),(7,2),(10,0),(10,11)],fill='#8badaf')
d.line([(7,2),(10,0),(11,6)],fill='#c2d1cc')
d.line([(3,6),(5,12)],fill='#9abfc2');d.line([(14,7),(12,12)],fill='#82a4b2')
im.save(root/'crystal_growth.png')
