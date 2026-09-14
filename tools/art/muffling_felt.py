"""Two rolled strips tied in quiet blue-green twine; original 16px item icon."""
from pathlib import Path
from PIL import Image, ImageDraw
im=Image.new('RGBA',(16,16)); p=ImageDraw.Draw(im)
p.polygon([(2,5),(7,3),(10,10),(5,13),(2,11)],fill='#4c6263')
p.polygon([(6,4),(11,2),(14,9),(11,12),(7,11)],fill='#677a79')
p.line([(7,4),(11,3),(13,8)],fill='#94a49b')
p.line([(3,6),(6,11)],fill='#8b9e95')
p.line([(2,8),(6,7),(12,6)],fill='#b0b9a2')
p.line([(7,6),(9,9),(8,13)],fill='#c4c5a7')
p.line([(9,9),(12,12)],fill='#9fada0')
im.save(Path(__file__).resolve().parents[2]/'assets/graphics/muffling_felt.png')
