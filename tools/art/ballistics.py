"""Small cardinal-facing rocket and crossbow bolt silhouettes."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.polygon([(2, 5), (5, 7), (11, 6), (15, 8), (11, 10), (5, 9), (2, 11)], fill='#a6aea0')
d.rectangle((5, 7, 10, 9), fill='#687568')
d.line((7, 6, 10, 6), fill='#d0cba5')
d.point((13, 8), fill='#c78450')
im.save(root / 'rocket.png')
im = Image.new('RGBA', (16, 16)); d = ImageDraw.Draw(im)
d.line((3, 8, 12, 8), fill='#92805f')
d.polygon([(11, 6), (15, 8), (11, 10)], fill='#bdc0ae')
d.line((2, 5, 5, 8, 2, 11), fill='#5d8277', width=2)
im.save(root / 'bolt.png')
