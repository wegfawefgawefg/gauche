"""A battered brass pea whistle on a short dark cord, 16px."""
from pathlib import Path
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
im = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(im)
p.line([(5, 11), (3, 13), (1, 12), (1, 9), (3, 8)], fill='#574c38')
p.polygon([(4, 5), (8, 4), (11, 6), (14, 5), (15, 8), (11, 9),
           (9, 12), (5, 12), (3, 9)], fill='#4b3a27')
p.polygon([(5, 5), (8, 5), (10, 7), (13, 6), (14, 8), (10, 9),
           (8, 11), (5, 10), (4, 8)], fill='#ad873e')
p.line([(5, 6), (8, 6), (9, 7)], fill='#dbc07a')
p.rectangle((6, 8, 8, 9), fill='#51412c')
p.point((5, 9), fill='#d6b864')
im.save(root / 'foreman_whistle_item.png')
