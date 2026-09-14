"""Cold glass and quiet temporary ice sheets, drawn at native 16px resolution."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[2] / 'assets' / 'graphics'
image = Image.new('RGBA', (16, 16))
p = ImageDraw.Draw(image)
p.polygon([(6, 1), (10, 1), (10, 6), (13, 9), (13, 13), (11, 15),
           (4, 15), (2, 13), (2, 9), (6, 6)], fill='#293d47')
p.rectangle((6, 1, 9, 3), fill='#9b967a')
p.polygon([(7, 4), (9, 4), (9, 7), (11, 9), (11, 12), (9, 13),
           (5, 13), (4, 11), (5, 9), (7, 7)], fill='#739fa7')
p.rectangle((5, 10, 10, 12), fill='#b0ced0')
p.line([(6, 8), (5, 9)], fill='#e0e6d9')
p.point((8, 5), fill='#c9d7cc')
image.save(ROOT / 'cold_flask.png')

# PLANE: Most of the tile stays quiet; a handful of seams read as ice, not wall tops.
for thawing in (False, True):
    image = Image.new('RGBA', (16, 16), '#4e6972')
    p = ImageDraw.Draw(image)
    p.line([(2, 5), (5, 4), (8, 4)], fill='#759398')
    p.line([(10, 11), (13, 10)], fill='#66868c')
    if thawing:
        p.line([(0, 9), (4, 9), (6, 7), (9, 8), (12, 6), (15, 7)], fill='#2c4855')
        p.line([(6, 7), (7, 12), (9, 15)], fill='#2c4855')
        p.point((10, 4), fill='#789799')
    image.save(ROOT / ('thawing_water.png' if thawing else 'frozen_water.png'))
