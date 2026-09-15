"""Ice fishing gun: pale barbed steel, a rope reel and a dark wooden grip."""
from pathlib import Path
from PIL import Image, ImageDraw
root = Path(__file__).resolve().parents[2] / 'assets/graphics'
im = Image.new('RGBA', (16,16)); d = ImageDraw.Draw(im)
d.polygon([(1,7),(5,6),(12,6),(12,9),(6,9),(5,13),(2,13),(3,9),(1,9)], fill='#31484d')
d.rectangle((3,9,5,12), fill='#96754b')
d.line((3,6,13,6), fill='#a4b7b1', width=2)
d.line((5,5,13,5), fill='#dedbbd')
d.polygon([(12,3),(15,5),(12,7),(13,5)], fill='#dedbbd')
d.ellipse((6,8,10,12), fill='#c3a477'); d.rectangle((7,9,9,11),fill='#4b615c')
d.point((8,10),fill='#d4be84'); im.save(root/'harpoon_gun.png')
im = Image.new('RGBA',(16,16)); d=ImageDraw.Draw(im)
d.line((1,8,13,8),fill='#abbcb8',width=2)
d.line((2,7,12,7),fill='#e5dfbc')
d.polygon([(11,4),(15,7),(11,11),(12,8)],fill='#d5d9c3')
d.line((1,9,4,9),fill='#96754b'); im.save(root/'harpoon_head.png')
